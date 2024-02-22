#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2s_std.h"
#include "driver/i2s_tdm.h"
#
#include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"

#include "cat_sounds.h" 

#define TAG "CAT_SOUND_DETECTION"

#define I2S_CHANNEL_NUM 1
#define I2S_CH ((i2s_port_t)1)

static i2s_chan_handle_t rx_handle = NULL;        // I2S rx channel handler

#define I2S_CONFIG_DEFAULT(sample_rate, channel_fmt, bits_per_chan) { \
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate), \
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(bits_per_chan, channel_fmt), \
        .gpio_cfg = { \
            .mclk = GPIO_NUM_NC, \
            .bclk = GPIO_NUM_41, \
            .ws   = GPIO_NUM_42, \
            .dout = GPIO_NUM_NC, \
            .din  = GPIO_NUM_2, \
            .invert_flags = { \
                .mclk_inv = false, \
                .bclk_inv = false, \
                .ws_inv   = false, \
            }, \
        }, \
    }

static esp_err_t i2s_init(i2s_port_t i2s_num, uint32_t sample_rate, int channel_format, int bits_per_chan) {
    esp_err_t ret_val = ESP_OK;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(i2s_num, I2S_ROLE_MASTER);

    ret_val |= i2s_new_channel(&chan_cfg, NULL, &rx_handle);
    i2s_std_config_t std_cfg = I2S_CONFIG_DEFAULT(16000, I2S_SLOT_MODE_MONO, I2S_DATA_BIT_WIDTH_32BIT);
    std_cfg.slot_cfg.slot_mask = I2S_STD_SLOT_LEFT;
    // std_cfg.clk_cfg.mclk_multiple = EXAMPLE_MCLK_MULTIPLE;   //The default is I2S_MCLK_MULTIPLE_256. If not using 24-bit data width, 256 should be enough
    ret_val |= i2s_channel_init_std_mode(rx_handle, &std_cfg);
    ret_val |= i2s_channel_enable(rx_handle);

    return ret_val;
}

extern "C" void app_main();
// Other includes and setup code...

namespace {
  constexpr size_t tensor_arena_size = 100 * 1024; // Adjust as necessary
  uint8_t tensor_arena[tensor_arena_size];
  tflite::MicroErrorReporter micro_error_reporter; // Correct declaration of the error reporter
  tflite::MicroInterpreter* interpreter = nullptr;
  tflite::MicroMutableOpResolver<10> resolver; // Adjust the number based on the ops you need
}

void setup() {
    const tflite::Model* model = tflite::GetModel(cat_sounds);
    tflite::ErrorReporter* error_reporter = tflite::GetMicroErrorReporter();
    
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        // Get the global error reporter instance
        // Use the error reporter
        error_reporter->Report("Model provided is schema version %d not equal to supported version %d.", model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    resolver.AddConv2D();
    resolver.AddMaxPool2D(); // Register the MAX_POOL_2D op
    resolver.AddReshape();
    resolver.AddFullyConnected();
    resolver.AddSoftmax();
    resolver.AddQuantize();

    static tflite::MicroInterpreter static_interpreter(model, resolver, tensor_arena, tensor_arena_size);

    interpreter = &static_interpreter;

    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(&micro_error_reporter, "AllocateTensors() failed");
        return;
    }
}

bool ProcessOutput(TfLiteTensor* output_tensor) {
    if (output_tensor->type == kTfLiteUInt8) {
        // Assuming the model is quantized to uint8
        uint8_t* quantized_output = output_tensor->data.uint8;
        // Dequantize the output, if necessary, based on your model's quantization parameters
        float scale = output_tensor->params.scale;
        int zero_point = output_tensor->params.zero_point;
        float dequantized_output = (quantized_output[0] - zero_point) * scale;

        ESP_LOGI(TAG, "Dequantized cat sound probability: %f", dequantized_output);

        // Define a threshold for detecting a cat sound
        const float detection_threshold = 0.5f; // Adjust this threshold as needed

        // Determine if the detected probability exceeds the threshold
        if (dequantized_output > detection_threshold) {
            return true; // Cat sound detected
        } else {
            return false; // No cat sound detected
        }
    } else {
        ESP_LOGE(TAG, "Output tensor type mismatch, expected uint8 for quantized model.");
        return false;
    }
}

void capture_and_infer(void* pvParameters) {
    i2s_init(I2S_NUM_1, 16000, 2, 32);

    int16_t buffer[1024]; // Audio data buffer
    size_t bytes_read;

    while (true) {
        i2s_channel_read(rx_handle, &buffer, sizeof(buffer), &bytes_read, portMAX_DELAY);

        // Prepare input tensor
        TfLiteTensor* input_tensor = interpreter->input(0);

        // Ensure the input tensor type and shape are correct for your model
        if (input_tensor->type != kTfLiteUInt8) {
            ESP_LOGE(TAG, "Input tensor type mismatch, expected uint8.");
            continue; // Skip this loop iteration if the tensor type is not as expected
        }

        // Example conversion from int16_t to uint8 (simple scaling and offset)
        // Note: You'll need to adjust the scaling and offset based on your model's expected input range
        uint8_t* quantized_input = input_tensor->data.uint8;
        for (size_t i = 0; i < sizeof(buffer) / sizeof(buffer[0]); ++i) {
            // Simple example to fit int16 data into uint8 range, adjust accordingly
            int32_t scaled = buffer[i] / 256; // Example scaling
            scaled += 128; // Example offset to convert signed to unsigned range
            quantized_input[i] = static_cast<uint8_t>(std::max(static_cast<int32_t>(0), std::min(static_cast<int32_t>(255), scaled)));
        }

        // Run inference
        if (interpreter->Invoke() == kTfLiteOk) {
            TfLiteTensor* output_tensor = interpreter->output(0);
            // Process the inference result
            bool is_cat_sound = ProcessOutput(output_tensor); // Implement this based on your model output
            if (is_cat_sound) {
                ESP_LOGI(TAG, "Cat sound detected!");
            } else {
                ESP_LOGI(TAG, "No cat sound detected.");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


void app_main() {
    setup(); // Initialize TFLite components
    xTaskCreate(capture_and_infer, "capture_and_infer", 8192, NULL, 5, NULL);
}
