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

// Adapted from base/platform_file_posix.cc

#ifndef STARBOARD_SHARED_WIN_IMPL_FILE_FLUSH_H_
#define STARBOARD_SHARED_WIN_IMPL_FILE_FLUSH_H_

#include "starboard/file.h"

#include "starboard/shared/win/handle_eintr.h"

#include "starboard/shared/internal_only.h"
#include "starboard/shared/win/impl/file_impl.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <io.h>
#include <stdint.h>

#include <tchar.h>
#include <stdio.h>

/*#include "base/files/file.h"
#include "base/files/file_enumerator.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"
#include "base/strings/utf_string_conversions.h"*/

namespace starboard {
namespace shared {
namespace win {
namespace impl {

bool FileFlush(SbFile file) {
  /*if (!file || file->descriptor < 0) {
    return false;
  }

  return !HANDLE_EINTR(fsync(file->descriptor));*/
  
  if (!file || !file->descriptor/*.IsValid()*/) {
    return false;
  }

  //return file->descriptor.Flush();
  // see https://github.com/chromium/chromium/blob/75fb429759aa71bae59cdd57b69a482c1153579e/base/files/file_win.cc#L434
  return ::FlushFileBuffers(file->descriptor) != FALSE;
}

}  // namespace impl
}  // namespace win
}  // namespace shared
}  // namespace starboard

#endif  // STARBOARD_SHARED_WIN_IMPL_FILE_FLUSH_H_
