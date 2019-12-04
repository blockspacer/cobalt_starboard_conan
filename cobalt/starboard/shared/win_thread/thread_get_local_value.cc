// Copyright 2015 The Cobalt Authors. All Rights Reserved.
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

#include "starboard/thread.h"

#include "starboard/shared/win_thread/thread_local_key_internal.h"

/*#include "base/threading/thread.h"
#include "base/threading/platform_thread.h"
#include "base/threading/thread_local_storage.h"
#include "base/atomicops.h"
#include "base/logging.h"
#include "base/synchronization/lock.h"*/

#include "build/build_config.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void* SbThreadGetLocalValue(SbThreadLocalKey key) {
  if (!SbThreadIsValidLocalKey(key)) {
    return NULL;
  }

  // based on return base::internal::PlatformThreadLocalStorage::GetTLSValue(key->key);
  // see https://github.com/blockspacer/skia-opengl-emscripten/blob/7318ee910f647ac5bc3337cc2002aa77391d12e6/src/chromium/base/threading/thread_local_storage.h#L81
  return TlsGetValue(key->key);
}
