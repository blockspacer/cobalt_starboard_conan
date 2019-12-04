// Copyright 2017 The Cobalt Authors. All Rights Reserved.
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

#include "starboard/file.h"

#include <android/asset_manager.h>

#include "starboard/android/shared/file_internal.h"
#include "starboard/shared/posix/impl/file_read.h"

int SbFileRead(SbFile file, char* data, int size) {
  if (!file || size < 0) {
    return -1;
  }

  if (file->asset) {
    return AAsset_read(file->asset, data, size);
  } else {
    return ::starboard::shared::posix::impl::FileRead(file, data, size);
  }
}
