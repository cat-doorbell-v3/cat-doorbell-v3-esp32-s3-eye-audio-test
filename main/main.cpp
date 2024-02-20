#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2s.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"

#include "cat_sounds.h" // Include your model header here

static const char* TAG = "cat_sound_detector";

extern "C" void app_main();
// Other includes and setup code...


// Global variables for TFLite components
namespace {
  tflite::MicroErrorReporter micro_error_reporter;
  tflite::MicroInterpreter* interpreter = nullptr;
  constexpr int tensor_arena_size = 10 * 1024; // Adjust size as needed
  uint8_t tensor_arena[tensor_arena_size];
}

void setup() {
    const tflite::Model* model = tflite::GetModel(cat_sounds_model);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(&micro_error_reporter, "Model provided is schema version %d not equal to supported version %d.",
                             model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    static tflite::AllOpsResolver resolver;
    static tflite::MicroInterpreter static_interpreter(model, resolver, tensor_arena, tensor_arena_size, &micro_error_reporter);
    interpreter = &static_interpreter;

    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        ESP_LOGE(TAG, "AllocateTensors() failed");
        return;
    }
}


// Initialize I2S to capture audio data
void init_i2s() {
    // Configure the I2S peripheral
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = 16000, // Adjust according to your model requirements
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // Adjust as needed
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, // Mono
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = 4,
        .dma_buf_len = 1024,
    };

    // Configure the I2S driver
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    // Add pin configuration here based on your hardware setup
}

void capture_and_infer() {
    init_i2s(); // Initialize audio capture

    int16_t buffer[1024]; // Audio data buffer
    size_t bytes_read;

    while (true) {
        i2s_read(I2S_NUM_0, &buffer, sizeof(buffer), &bytes_read, portMAX_DELAY);

        // Prepare input tensor
        TfLiteTensor* input_tensor = interpreter->input(0);
        // Ensure the input tensor type and shape are correct for your model
        // Copy buffer data to input tensor
        memcpy(input_tensor->data.f, buffer, sizeof(buffer));

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
