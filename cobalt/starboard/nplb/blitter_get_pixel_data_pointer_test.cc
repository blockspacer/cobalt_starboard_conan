// Copyright 2016 The Cobalt Authors. All Rights Reserved.
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

#include "starboard/blitter.h"
#include "starboard/nplb/blitter_helpers.h"
#include "starboard/window.h"
#include "testing/gtest/include/gtest/gtest.h"

#if SB_HAS(BLITTER)

namespace starboard {
namespace nplb {
namespace {

TEST(SbBlitterGetPixelDataPointerTest, SunnyDay) {
  SbBlitterDevice device = SbBlitterCreateDefaultDevice();
  ASSERT_TRUE(SbBlitterIsDeviceValid(device));

  const int kWidth = 128;
  const int kHeight = 128;

  // Test that we can get pitch on all supported image formats.
  std::vector<SbBlitterPixelDataFormat> supported_formats =
      GetAllSupportedPixelFormatsForPixelData(device);
  for (std::vector<SbBlitterPixelDataFormat>::const_iterator iter =
           supported_formats.begin();
       iter != supported_formats.end(); ++iter) {
    SbBlitterPixelData pixel_data =
        SbBlitterCreatePixelData(device, kWidth, kHeight, *iter);
    ASSERT_TRUE(SbBlitterIsPixelDataValid(pixel_data));

    int bytes_per_pixel = SbBlitterBytesPerPixelForFormat(*iter);
    int pitch_in_bytes = SbBlitterGetPixelDataPitchInBytes(pixel_data);
    ASSERT_GE(pitch_in_bytes, kWidth * bytes_per_pixel);

    void* memory = SbBlitterGetPixelDataPointer(pixel_data);
    ASSERT_TRUE(memory);

    uint8_t* memory_bytes = static_cast<uint8_t*>(memory);

    // Ensure that we can successfully write to each byte of the allocated
    // pixel data.
    for (int y = 0; y < kHeight; ++y) {
      for (int x = 0; x < kWidth; ++x) {
        for (int c = 0; c < bytes_per_pixel; ++c) {
          memory_bytes[y * pitch_in_bytes + x * bytes_per_pixel + c] = 255;
        }
      }
    }

    EXPECT_TRUE(SbBlitterDestroyPixelData(pixel_data));
  }

  EXPECT_TRUE(SbBlitterDestroyDevice(device));
}

TEST(SbBlitterGetPixelDataPointerTest, RainyDayInvalidPixelData) {
  EXPECT_EQ(NULL, SbBlitterGetPixelDataPointer(kSbBlitterInvalidPixelData));
}

}  // namespace
}  // namespace nplb
}  // namespace starboard

#endif  // SB_HAS(BLITTER)
