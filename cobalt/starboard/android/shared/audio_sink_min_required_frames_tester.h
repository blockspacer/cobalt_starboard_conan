// Copyright 2019 The Cobalt Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef STARBOARD_ANDROID_SHARED_AUDIO_SINK_MIN_REQUIRED_FRAMES_TESTER_H_
#define STARBOARD_ANDROID_SHARED_AUDIO_SINK_MIN_REQUIRED_FRAMES_TESTER_H_

#include <atomic>
#include <functional>

#include "starboard/common/condition_variable.h"
#include "starboard/common/mutex.h"
#include "starboard/media.h"
#include "starboard/shared/starboard/thread_checker.h"
#include "starboard/thread.h"

namespace starboard {
namespace android {
namespace shared {

class AudioTrackAudioSink;

// The class is to detect min required frames for audio sink to play audio
// without underflow.
class MinRequiredFramesTester {
 public:
  typedef std::function<void(int number_of_channels,
                             SbMediaAudioSampleType sample_type,
                             int sample_rate,
                             int min_required_frames)>
      OnMinRequiredFramesReceivedCallback;

  MinRequiredFramesTester(int audio_sink_buffer_size,
                          int max_required_frames,
                          int default_required_frames,
                          int required_frames_increment,
                          int min_stable_played_frames);
  ~MinRequiredFramesTester();

  void StartTest(int number_of_channels,
                 SbMediaAudioSampleType sample_type,
                 int sample_rate,
                 OnMinRequiredFramesReceivedCallback received_cb);

 private:
  static void* TesterThreadEntryPoint(void* context);
  void TesterThreadFunc();

  static void UpdateSourceStatusFunc(int* frames_in_buffer,
                                     int* offset_in_frames,
                                     bool* is_playing,
                                     bool* is_eos_reached,
                                     void* context);
  static void ConsumeFramesFunc(int frames_consumed,
                                SbTime frames_consumed_at,
                                void* context);
  void UpdateSourceStatus(int* frames_in_buffer,
                          int* offset_in_frames,
                          bool* is_playing,
                          bool* is_eos_reached);
  void ConsumeFrames(int frames_consumed);

  MinRequiredFramesTester(const MinRequiredFramesTester&) = delete;
  MinRequiredFramesTester& operator=(const MinRequiredFramesTester&) = delete;

  const int audio_sink_buffer_size_;
  const int max_required_frames_;
  const int default_required_frames_;
  const int required_frames_increment_;
  const int min_stable_played_frames_;

  ::starboard::shared::starboard::ThreadChecker thread_checker_;

  // Shared variables between tester thread and audio sink thread.
  AudioTrackAudioSink* audio_sink_ = nullptr;
  int number_of_channels_;
  SbMediaAudioSampleType sample_type_;
  int sample_rate_;
  int min_required_frames_;

  // Used only by tester thread.
  OnMinRequiredFramesReceivedCallback received_cb_;

  // Used only by audio sink thread.
  int total_consumed_frames_;
  int last_underrun_count_;
  int last_total_consumed_frames_;

  Mutex mutex_;
  ConditionVariable condition_variable_;
  SbThread tester_thread_ = kSbThreadInvalid;
  std::atomic_bool destroyed_;
};

}  // namespace shared
}  // namespace android
}  // namespace starboard

#endif  // STARBOARD_ANDROID_SHARED_AUDIO_SINK_MIN_REQUIRED_FRAMES_TESTER_H_
