/* Copyright 2020-2023 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include <algorithm>
#include <cstdint>
#include <iterator>
#include "esp_log.h"
#include <errno.h>
#include <string.h> // For strerror()
#include <dirent.h> // Directory Entry
#include <sys/stat.h>
#include "main_functions.h"
#include "esp_heap_caps.h"

#include "audio_provider.h"
#include "command_responder.h"
#include "feature_provider.h"
#include "recognize_commands.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/core/c/common.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"

#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_default_configs.h"
#include "sd_card_provider.h"

#define MODEL_FILENAME MOUNT_POINT"/CAT_MOD.TFL"
#define TAG "main_functions"

// Globals, used for compatibility with Arduino-style sketches.
namespace {
    const tflite::Model* model = nullptr;
    tflite::MicroInterpreter* interpreter = nullptr;
    TfLiteTensor* model_input = nullptr;
    FeatureProvider* feature_provider = nullptr;
    RecognizeCommands* recognizer = nullptr;
    int32_t previous_time = 0;
    
    // Create an area of memory to use for input, output, and intermediate arrays.
    // The size of this will depend on the model you're using, and may need to be
    // determined by experimentation.
    constexpr int kTensorArenaSize = 96 * 1024;
    // uint8_t tensor_arena[kTensorArenaSize];
    uint8_t* tensor_arena = nullptr;

    int8_t feature_buffer[kFeatureElementCount];
    int8_t* model_input_buffer = nullptr;
}  // namespace

void setup() {
    ESP_LOGI(TAG, "Allocate tensor_arena in PSRAM");
    tensor_arena = (uint8_t*)heap_caps_malloc(kTensorArenaSize, MALLOC_CAP_SPIRAM);
    if (tensor_arena == nullptr) {
        // Handle allocation failure
        ESP_LOGE(TAG, "Failed to allocate tensor arena in PSRAM");
        return;
    }

    SDCardProvider& sdCardProvider = SDCardProvider::getInstance();

    // Initialize the SD card
    if (sdCardProvider.initialize() == ESP_OK) {
        // List the directory contents
        sdCardProvider.listDir("/sdcard/");
    } else {
        ESP_LOGE("SDCardProvider", "Initialization failed");
    }

    // Read the model into a buffer
    ESP_LOGI(TAG, "Reading model from SD card: %s", MODEL_FILENAME);
    FILE* file = fopen(MODEL_FILENAME, "rb");
    if (file == nullptr) {
        ESP_LOGE(TAG, "Failed to open model file, errno = %d, %s", errno, strerror(errno));
        esp_vfs_fat_sdmmc_unmount();
        return;
    }

  fseek(file, 0, SEEK_END);
  long model_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  ESP_LOGI(TAG, "Free heap size before allocation: %lu", esp_get_free_heap_size());

  ESP_LOGI(TAG, "Model size: %lu", model_size);

  if (!heap_caps_check_integrity(MALLOC_CAP_DEFAULT, true)) {
    ESP_LOGE(TAG, "Heap corruption detected");
    return;
  } else {
    ESP_LOGI(TAG, "Heap integrity check passed");
  }

  uint8_t* model_buffer = (uint8_t*)heap_caps_malloc(model_size, MALLOC_CAP_SPIRAM); 
  if (model_buffer == nullptr) {
      ESP_LOGE(TAG, "Failed to allocate memory for model");
      fclose(file);
      esp_vfs_fat_sdmmc_unmount();
      return;
  }

  fread(model_buffer, 1, model_size, file);
  fclose(file);

  ESP_LOGI(TAG, "GetModel()");

  // Use TensorFlow Lite Micro to interpret the model
  model = tflite::GetModel(model_buffer);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    ESP_LOGE(TAG, "Model provided is schema version %lu not equal to supported version %d.", 
            model->version(), TFLITE_SCHEMA_VERSION);
    free(model_buffer);
    esp_vfs_fat_sdmmc_unmount();
    return;
  }
  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will
  // incur some penalty in code space for op implementations that are not
  // needed by this graph.
  //
  // tflite::AllOpsResolver resolver;
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroMutableOpResolver<7> micro_op_resolver;
  if (micro_op_resolver.AddDepthwiseConv2D() != kTfLiteOk) {
    return;
  }
  if (micro_op_resolver.AddFullyConnected() != kTfLiteOk) {
    return;
  }
  if (micro_op_resolver.AddSoftmax() != kTfLiteOk) {
    return;
  }
  if (micro_op_resolver.AddReshape() != kTfLiteOk) {
    return;
  }
  if (micro_op_resolver.AddQuantize() != kTfLiteOk) {
    return;
  }
  if (micro_op_resolver.AddConv2D() != kTfLiteOk) {
    return;
  }
  if (micro_op_resolver.AddMaxPool2D() != kTfLiteOk) {
    return;
  }

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, micro_op_resolver, tensor_arena, kTensorArenaSize);
  interpreter = &static_interpreter;

  ESP_LOGI(TAG, "AllocateTensors()");
  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    MicroPrintf("AllocateTensors() failed");
    return;
  }
  ESP_LOGI(TAG, "End AllocateTensors()");

  // Get information about the memory area to use for the model's input.
  model_input = interpreter->input(0);

  // Log the size of dimensions
  ESP_LOGI(TAG, "Dims size: %d", model_input->dims->size);
  
  // Log the first dimension data
  ESP_LOGI(TAG, "Dims data[0]: %d", model_input->dims->data[0]);
  
  // Log the second dimension data
  ESP_LOGI(TAG, "Dims data[1]: %d", model_input->dims->data[1]);
  
  // Log the expected second dimension size for comparison
  ESP_LOGI(TAG, "Expected Dims data[1] (kFeatureCount * kFeatureSize): %d", kFeatureCount * kFeatureSize);
  
  // Log the data type
  ESP_LOGI(TAG, "Data type: %d", model_input->type);
  
  // Log the expected data type for comparison
  ESP_LOGI(TAG, "Expected Data type (kTfLiteInt8): %d", kTfLiteInt8);

  if ((model_input->dims->size != 4) || 
    (model_input->dims->data[0] != 1) ||
    (model_input->dims->data[1] != 140) ||
    (model_input->dims->data[2] != 13) ||
    (model_input->dims->data[3] != 1) || 
    (model_input->type != kTfLiteInt8)) {
    MicroPrintf("Bad input tensor parameters in model");
    return;
  }

  model_input_buffer = tflite::GetTensorData<int8_t>(model_input);

  // Prepare to access the audio spectrograms from a microphone or other source
  // that will provide the inputs to the neural network.
  // NOLINTNEXTLINE(runtime-global-variables)
  static FeatureProvider static_feature_provider(kFeatureElementCount,
                                                 feature_buffer);
  feature_provider = &static_feature_provider;

  static RecognizeCommands static_recognizer;
  recognizer = &static_recognizer;

  previous_time = 0;
}

// The name of this function is important for Arduino compatibility.
void loop() {
  // Fetch the spectrogram for the current time.
  const int32_t current_time = LatestAudioTimestamp();
  int how_many_new_slices = 0;
  TfLiteStatus feature_status = feature_provider->PopulateFeatureData(
      previous_time, current_time, &how_many_new_slices);
  if (feature_status != kTfLiteOk) {
    MicroPrintf( "Feature generation failed");
    return;
  }
  previous_time = current_time;
  // If no new audio samples have been received since last time, don't bother
  // running the network model.
  if (how_many_new_slices == 0) {
    return;
  }

  // Copy feature buffer to input tensor
  for (int i = 0; i < kFeatureElementCount; i++) {
    model_input_buffer[i] = feature_buffer[i];
  }

  // Run the model on the spectrogram input and make sure it succeeds.
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    MicroPrintf( "Invoke failed");
    return;
  }

  // Obtain a pointer to the output tensor
  TfLiteTensor* output = interpreter->output(0);

  float output_scale = output->params.scale;
  int output_zero_point = output->params.zero_point;
  int max_idx = 0;
  float max_result = 0.0;
  // Dequantize output values and find the max
  for (int i = 0; i < kCategoryCount; i++) {
    float current_result = (tflite::GetTensorData<int8_t>(output)[i] - output_zero_point) * output_scale;
    if (current_result > max_result) {
      max_result = current_result; // update max result
      max_idx = i; // update category
    }
  }
  if (max_result > 0.8f) {
    ESP_LOGW(TAG, "Detected %7s, score: %.2f", kCategoryLabels[max_idx], static_cast<double>(max_result));
  } 
}
