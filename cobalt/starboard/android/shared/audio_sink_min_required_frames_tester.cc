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

#include "starboard/android/shared/audio_sink_min_required_frames_tester.h"

#include <vector>

#include "starboard/android/shared/audio_track_audio_sink_type.h"

namespace starboard {
namespace android {
namespace shared {

namespace {
const int kCheckpointFramesInterval = 1024;

// Helper function to compute the size of the two valid starboard audio sample
// types.
size_t GetSampleSize(SbMediaAudioSampleType sample_type) {
  switch (sample_type) {
    case kSbMediaAudioSampleTypeFloat32:
      return sizeof(float);
    case kSbMediaAudioSampleTypeInt16Deprecated:
      return sizeof(int16_t);
  }
  SB_NOTREACHED();
  return 0u;
}
}  // namespace

MinRequiredFramesTester::MinRequiredFramesTester(int audio_sink_buffer_size,
                                                 int max_required_frames,
                                                 int default_required_frames,
                                                 int required_frames_increment,
                                                 int min_stable_played_frames)
    : audio_sink_buffer_size_(audio_sink_buffer_size),
      max_required_frames_(max_required_frames),
      default_required_frames_(default_required_frames),
      required_frames_increment_(required_frames_increment),
      min_stable_played_frames_(min_stable_played_frames),
      condition_variable_(mutex_),
      destroyed_(false) {}

MinRequiredFramesTester::~MinRequiredFramesTester() {
  SB_DCHECK(thread_checker_.CalledOnValidThread());
  destroyed_.store(true);
  if (SbThreadIsValid(tester_thread_)) {
    {
      ScopedLock scoped_lock(mutex_);
      condition_variable_.Signal();
    }
    SbThreadJoin(tester_thread_, NULL);
    tester_thread_ = kSbThreadInvalid;
  }
}

void MinRequiredFramesTester::StartTest(
    int number_of_channels,
    SbMediaAudioSampleType sample_type,
    int sample_rate,
    OnMinRequiredFramesReceivedCallback received_cb) {
  SB_DCHECK(thread_checker_.CalledOnValidThread());
  // MinRequiredFramesTester only supports to do test once now.
  SB_DCHECK(!SbThreadIsValid(tester_thread_));

  number_of_channels_ = number_of_channels;
  sample_type_ = sample_type;
  sample_rate_ = sample_rate;
  received_cb_ = received_cb;

  tester_thread_ =
      SbThreadCreate(0, kSbThreadPriorityLowest, kSbThreadNoAffinity, true,
                     "audio_track_tester",
                     &MinRequiredFramesTester::TesterThreadEntryPoint, this);
  SB_DCHECK(SbThreadIsValid(tester_thread_));
}

// static
void* MinRequiredFramesTester::TesterThreadEntryPoint(void* context) {
  SB_DCHECK(context);
  MinRequiredFramesTester* tester =
      static_cast<MinRequiredFramesTester*>(context);
  tester->TesterThreadFunc();
  return NULL;
}

void MinRequiredFramesTester::TesterThreadFunc() {
  bool wait_timeout = false;
  // Currently, we only support test once. But we can put following codes in
  // a for loop easily to support test multiple times.
  std::vector<uint8_t> silence_buffer(
      max_required_frames_ * number_of_channels_ * GetSampleSize(sample_type_),
      0);
  void* frame_buffers[1];
  frame_buffers[0] = silence_buffer.data();
  // Set default values.
  min_required_frames_ = default_required_frames_;
  total_consumed_frames_ = 0;
  last_underrun_count_ = -1;
  last_total_consumed_frames_ = 0;
  {
    ScopedLock scoped_lock(mutex_);
    // Need to check |destroyed_| before start, as MinRequiredFramesTester may
    // be destroyed immediately after tester thread started.
    if (!destroyed_.load()) {
      audio_sink_ = new AudioTrackAudioSink(
          NULL, number_of_channels_, sample_rate_, sample_type_, frame_buffers,
          max_required_frames_,
          audio_sink_buffer_size_ * number_of_channels_ *
              GetSampleSize(sample_type_),
          &MinRequiredFramesTester::UpdateSourceStatusFunc,
          &MinRequiredFramesTester::ConsumeFramesFunc, this);
      wait_timeout = !condition_variable_.WaitTimed(kSbTimeSecond * 5);
      if (wait_timeout) {
        SB_LOG(ERROR) << "Audio sink min required frames tester timeout.";
        SB_NOTREACHED();
      }
    }
  }
  delete audio_sink_;
  audio_sink_ = nullptr;
  // Call |received_cb_| after audio sink thread is ended.
  // |number_of_channels_|, |sample_type_|, |sample_rate_| and
  // |min_required_frames_| are shared between two threads.
  if (!destroyed_.load() && !wait_timeout) {
    received_cb_(number_of_channels_, sample_type_, sample_rate_,
                 min_required_frames_);
  }
}

// static
void MinRequiredFramesTester::UpdateSourceStatusFunc(int* frames_in_buffer,
                                                     int* offset_in_frames,
                                                     bool* is_playing,
                                                     bool* is_eos_reached,
                                                     void* context) {
  MinRequiredFramesTester* tester =
      static_cast<MinRequiredFramesTester*>(context);
  SB_DCHECK(tester);
  SB_DCHECK(frames_in_buffer);
  SB_DCHECK(offset_in_frames);
  SB_DCHECK(is_playing);
  SB_DCHECK(is_eos_reached);

  tester->UpdateSourceStatus(frames_in_buffer, offset_in_frames, is_playing,
                             is_eos_reached);
}

// static
void MinRequiredFramesTester::ConsumeFramesFunc(int frames_consumed,
                                                SbTime frames_consumed_at,
                                                void* context) {
  SB_UNREFERENCED_PARAMETER(frames_consumed_at);

  MinRequiredFramesTester* tester =
      static_cast<MinRequiredFramesTester*>(context);
  SB_DCHECK(tester);

  tester->ConsumeFrames(frames_consumed);
}

void MinRequiredFramesTester::UpdateSourceStatus(int* frames_in_buffer,
                                                 int* offset_in_frames,
                                                 bool* is_playing,
                                                 bool* is_eos_reached) {
  *frames_in_buffer = min_required_frames_;
  *offset_in_frames = 0;
  *is_playing = true;
  *is_eos_reached = false;
}

void MinRequiredFramesTester::ConsumeFrames(int frames_consumed) {
  total_consumed_frames_ += frames_consumed;
  // Wait until played enough frames.
  if (total_consumed_frames_ - kCheckpointFramesInterval <
      last_total_consumed_frames_) {
    return;
  }
  if (last_underrun_count_ == -1) {
    // |last_underrun_count_| is unknown, record the current underrun count
    // and start to observe the underrun count.
    last_underrun_count_ = audio_sink_->GetUnderrunCount();
    last_total_consumed_frames_ = total_consumed_frames_;
    return;
  }
  // The playback should be played for a while. If we still get new underruns,
  // we need to write more buffers into audio sink.
  int underrun_count = audio_sink_->GetUnderrunCount();
  if (underrun_count > last_underrun_count_) {
    min_required_frames_ += required_frames_increment_;
    if (min_required_frames_ >= max_required_frames_) {
      SB_LOG(WARNING) << "Min required frames reached maximum.";
    } else {
      last_underrun_count_ = -1;
      last_total_consumed_frames_ = total_consumed_frames_;
      return;
    }
  }

  if (min_required_frames_ >= max_required_frames_ ||
      total_consumed_frames_ - min_stable_played_frames_ >=
          last_total_consumed_frames_) {
    // |min_required_frames_| reached maximum, or playback is stable and
    // doesn't have underruns. Stop the test.
    last_total_consumed_frames_ = INT_MAX;
    ScopedLock scoped_lock(mutex_);
    condition_variable_.Signal();
  }
}

}  // namespace shared
}  // namespace android
}  // namespace starboard
