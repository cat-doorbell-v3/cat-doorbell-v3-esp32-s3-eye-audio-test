/* Copyright 2023 The TensorFlow Authors. All Rights Reserved.

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

#ifndef TENSORFLOW_LITE_MICRO_EXAMPLES_MICRO_SPEECH_MICRO_MODEL_SETTINGS_H_
#define TENSORFLOW_LITE_MICRO_EXAMPLES_MICRO_SPEECH_MICRO_MODEL_SETTINGS_H_

/*
kAudioSampleFrequency = 16000
kMaxAudioSampleSize = 480
kFeatureSize = 40
kFeatureCount = 49.0
kFeatureElementCount (kFeatureSize * kFeatureCount) = 1960.0
kFeatureStrideMs = 20
kFeatureDurationMs = 30.0
*/

constexpr int kMaxAudioSampleSize = 512;
constexpr int kAudioSampleFrequency = 16000;
constexpr int kFeatureSize = 40;
constexpr int kFeatureCount = 49;
constexpr int kFeatureElementCount = 1960;
constexpr int kFeatureStrideMs = 20;
constexpr int kFeatureDurationMs = 30;

constexpr int kCategoryCount = 3;
constexpr const char* kCategoryLabels[kCategoryCount] = { 
    "meow",
    "unknown",
    "silence"
    };

#endif  // TENSORFLOW_LITE_MICRO_EXAMPLES_MICRO_SPEECH_MICRO_MODEL_SETTINGS_H_