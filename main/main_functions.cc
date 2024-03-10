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

#include "model.h"

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
    constexpr int kTensorArenaSize = 60 * 1024;
    uint8_t tensor_arena[kTensorArenaSize];
    int8_t feature_buffer[kFeatureElementCount];
    int8_t* model_input_buffer = nullptr;
}  // namespace

void setup() {
    model = tflite::GetModel(g_model);

    if (model->version() != TFLITE_SCHEMA_VERSION) {
      ESP_LOGE(TAG, "Model provided is schema version %lu not equal to supported version %d.", 
              model->version(), TFLITE_SCHEMA_VERSION);
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
    if (micro_op_resolver.AddConv2D() != kTfLiteOk) {
      return;
    }
  
    // Build an interpreter to run the model with.
    static tflite::MicroInterpreter static_interpreter(model, micro_op_resolver, tensor_arena, kTensorArenaSize);
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

    // Log the second dimension data
    // ESP_LOGI(TAG, "Dims data[2]: %d", model_input->dims->data[2]);

     // Log the second dimension data
    // ESP_LOGI(TAG, "Dims data[3]: %d", model_input->dims->data[3]);
     
    // Log the expected second dimension size for comparison
    ESP_LOGI(TAG, "Expecting Dims data[1] to be (kFeatureCount * kFeatureSize): %d", kFeatureCount * kFeatureSize);
    
    // Log the data type
    ESP_LOGI(TAG, "Data type: %d", model_input->type);
    
    // Log the expected data type for comparison
    ESP_LOGI(TAG, "Expecting Data type (kTfLiteInt8): %d", kTfLiteInt8);
  
    if ((model_input->dims->size != 2) || 
      (model_input->dims->data[0] != 1) ||
      (model_input->dims->data[1] != (kFeatureCount * kFeatureSize)) ||
      (model_input->type != kTfLiteInt8)) {
      ESP_LOGE(TAG, "Bad input tensor parameters in model");
      return;
    }

    model_input_buffer = tflite::GetTensorData<int8_t>(model_input);
  
    // Prepare to access the audio spectrograms from a microphone or other source
    // that will provide the inputs to the neural network.
    // NOLINTNEXTLINE(runtime-global-variables)
    static FeatureProvider static_feature_provider(kFeatureElementCount, feature_buffer);
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
    TfLiteStatus feature_status = feature_provider->PopulateFeatureData(previous_time, current_time, &how_many_new_slices);
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
#if 0 // using simple argmax instead of recognizer
  float output_scale = output->params.scale;
  int output_zero_point = output->params.zero_point;
  int max_idx = 0;
  float max_result = 0.0;
  // Dequantize output values and find the max
  for (int i = 0; i < kCategoryCount; i++) {
    float current_result =
        (tflite::GetTensorData<int8_t>(output)[i] - output_zero_point) *
        output_scale;
    if (current_result > max_result) {
      max_result = current_result; // update max result
      max_idx = i; // update category
    }
  }
  if (max_result > 0.8f) {
    MicroPrintf("Detected %7s, score: %.2f", kCategoryLabels[max_idx],
        static_cast<double>(max_result));
  }
#else
  // Determine whether a command was recognized based on the output of inference
  const char* found_command = nullptr;
  float score = 0;
  bool is_new_command = false;
  TfLiteStatus process_status = recognizer->ProcessLatestResults(
      output, current_time, &found_command, &score, &is_new_command);
  if (process_status != kTfLiteOk) {
    MicroPrintf("RecognizeCommands::ProcessLatestResults() failed");
    return;
  }
  // Do something based on the recognized command. The default implementation
  // just prints to the error console, but you should replace this with your
  // own function for a real application.
  RespondToCommand(current_time, found_command, score, is_new_command);
#endif
}
