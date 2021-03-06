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

// see https://github.com/blockspacer/skia-opengl-emscripten/blob/bb16ab108bc4018890f4ff3179250b76c0d9053b/thirdparty/icu_wrapper/third_party/icu/source/common/putil.cpp#L1712

#include "starboard/system.h"

#include <locale.h>  // for setlocale and LC_*
#include <stdlib.h>  // for getenv

#include "starboard/common/string.h"

namespace {
bool IsValidId(const char* WIN_id) {
  return !((WIN_id == NULL) || (SbStringCompare("C", WIN_id, 1) == 0) ||
           (SbStringCompare("POSIX", WIN_id, 5) == 0));
}
}

const char* SbSystemGetLocaleId() {
  // Adapted from ICU's putil.c:uprv_getPOSIXIDForCategory.
  const char* WIN_id = nullptr; // setlocale(LC_MESSAGES, NULL)
  if (!IsValidId(WIN_id)) {
    WIN_id = getenv("LC_ALL");
    if (WIN_id == NULL) {
      WIN_id = getenv("LC_MESSAGES");
      if (WIN_id == NULL) {
        WIN_id = getenv("LANG");
      }
    }
  }

  return WIN_id;
}
