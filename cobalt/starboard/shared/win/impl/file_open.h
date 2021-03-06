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

#ifndef STARBOARD_SHARED_WIN_IMPL_FILE_OPEN_H_
#define STARBOARD_SHARED_WIN_IMPL_FILE_OPEN_H_

#include "starboard/file.h"

#include "starboard/common/log.h"
#include "starboard/shared/win/handle_eintr.h"

#include "starboard/shared/internal_only.h"
#include "starboard/shared/win/impl/file_impl.h"

/*#include "base/files/file.h"
#include "base/files/file_enumerator.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"
#include "base/strings/utf_string_conversions.h"*/

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <io.h>
#include <stdint.h>

#include <tchar.h>
#include <stdio.h>

#include <string>
#include <codecvt> /// \note for UTF-8
#include <locale>

namespace starboard {
namespace shared {
namespace win {
namespace impl {

// TODO: repeated code 
static std::wstring utf8_to_wstring(const std::string& str) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  return converter.from_bytes(str);
}
// TODO: repeated code 
static std::string wstring_to_utf8(const std::wstring& str) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  return converter.to_bytes(str);
}

SbFile FileOpen(const char* path,
                int flags,
                bool* out_created,
                SbFileError* out_error) {
  //int open_flags = 0;

  // see https://github.com/chromium/chromium/blob/ccd149af47315e4c6f2fc45d55be1b271f39062c/base/files/file.h#L53
  
  DWORD disposition = 0;

  if (flags & kSbFileCreateOnly) {
    disposition = CREATE_NEW;
  }

  if (out_created) {
    *out_created = false;
  }

  if (flags & kSbFileCreateAlways) {
    disposition = CREATE_ALWAYS;
  }

  if (flags & kSbFileOpenTruncated) {
    SB_DCHECK(flags & kSbFileWrite);
    disposition = TRUNCATE_EXISTING;
  }

  /*if (!open_flags && !(flags & kSbFileOpenOnly) &&
      !(flags & kSbFileOpenAlways)) {
    SB_NOTREACHED();
    errno = EOPNOTSUPP;
    if (out_error) {
      *out_error = kSbFileErrorFailed;
    }

    return kSbFileInvalid;
  }*/

  if (!disposition) {
    errno = EOPNOTSUPP;
    if (out_error) {
      *out_error = kSbFileErrorFailed;
    }
    return kSbFileInvalid;
  }

  DWORD access = 0;
  if (flags & kSbFileWrite && flags & kSbFileRead) {
    access |= GENERIC_READ;
    access |= GENERIC_WRITE;
  } else if (flags & kSbFileWrite) {
    access |= GENERIC_WRITE;
  }

  //SB_COMPILE_ASSERT(O_RDONLY == 0, O_RDONLY_must_equal_zero);

  /*int mode = S_IRUSR | S_IWUSR;
  int descriptor = HANDLE_EINTR(open(path, open_flags, mode));*/

  DWORD sharing = 0;
  /*sharing = (flags & FLAG_EXCLUSIVE_READ) ? 0 : FILE_SHARE_READ;
  if (!(flags & FLAG_EXCLUSIVE_WRITE))
    sharing |= FILE_SHARE_WRITE;
  if (flags & FLAG_SHARE_DELETE)
    sharing |= FILE_SHARE_DELETE;*/

  DWORD create_flags = 0;
  /*if (flags & FLAG_ASYNC)
    create_flags |= FILE_FLAG_OVERLAPPED;
  if (flags & FLAG_TEMPORARY)
    create_flags |= FILE_ATTRIBUTE_TEMPORARY;
  if (flags & FLAG_HIDDEN)
    create_flags |= FILE_ATTRIBUTE_HIDDEN;
  if (flags & FLAG_DELETE_ON_CLOSE)
    create_flags |= FILE_FLAG_DELETE_ON_CLOSE;
  if (flags & FLAG_BACKUP_SEMANTICS)
    create_flags |= FILE_FLAG_BACKUP_SEMANTICS;
  if (flags & FLAG_SEQUENTIAL_SCAN)
    create_flags |= FILE_FLAG_SEQUENTIAL_SCAN;*/

  HANDLE file;
  file = ::CreateFile(utf8_to_wstring(path).c_str(), access, sharing, NULL, disposition,
                       create_flags, NULL);

  /*if (flags & kSbFileOpenAlways) {
    if (!descriptor.IsValid()) {
      open_flags |= base::File::FLAG_CREATE;
      //descriptor = HANDLE_EINTR(open(path, open_flags, mode));
      descriptor.Initialize(file_path, open_flags);
      if (out_created && descriptor.IsValid()) {
        *out_created = true;
      }
    }
  }*/

  if (out_created && /*(descriptor.IsValid())*/file &&
      (flags & (kSbFileCreateAlways | kSbFileCreateOnly))) {
    *out_created = true;
  }

  /// \todo base error <-> sb error

  if (!file) {
    return kSbFileInvalid;
  }

  SbFile result = new SbFilePrivate();
  result->descriptor = std::move(file);
  return result;
}

// TODO: remove commented out code 

#if 0
  // see https://github.com/chromium/chromium/blob/ccd149af47315e4c6f2fc45d55be1b271f39062c/base/files/file.h#L53

  if (flags & kSbFileCreateOnly) {
    open_flags = base::File::FLAG_CREATE | base::File::FLAG_EXCLUSIVE_WRITE;
  }

  if (out_created) {
    *out_created = false;
  }

  if (flags & kSbFileCreateAlways) {
    SB_DCHECK(!open_flags);
    open_flags = base::File::FLAG_CREATE | base::File::FLAG_OPEN_TRUNCATED;
  }

  if (flags & kSbFileOpenTruncated) {
    SB_DCHECK(!open_flags);
    SB_DCHECK(flags & kSbFileWrite);
    open_flags = base::File::FLAG_OPEN_TRUNCATED;
  }

  if (!open_flags && !(flags & kSbFileOpenOnly) &&
      !(flags & kSbFileOpenAlways)) {
    SB_NOTREACHED();
    errno = EOPNOTSUPP;
    if (out_error) {
      *out_error = kSbFileErrorFailed;
    }

    return kSbFileInvalid;
  }

  if (flags & kSbFileWrite && flags & kSbFileRead) {
    open_flags |= base::File::FLAG_READ;
    open_flags |= base::File::FLAG_WRITE;
  } else if (flags & kSbFileWrite) {
    open_flags |= base::File::FLAG_WRITE;
  }

  //SB_COMPILE_ASSERT(O_RDONLY == 0, O_RDONLY_must_equal_zero);

  /*int mode = S_IRUSR | S_IWUSR;
  int descriptor = HANDLE_EINTR(open(path, open_flags, mode));*/
  base::FilePath file_path;
  file_path = file_path.Append(base::UTF8ToUTF16(path));
  base::File descriptor(file_path, open_flags);

  if (flags & kSbFileOpenAlways) {
    if (!descriptor.IsValid()) {
      open_flags |= base::File::FLAG_CREATE;
      //descriptor = HANDLE_EINTR(open(path, open_flags, mode));
      descriptor.Initialize(file_path, open_flags);
      if (out_created && descriptor.IsValid()) {
        *out_created = true;
      }
    }
  }

  if (out_created && (descriptor.IsValid()) &&
      (flags & (kSbFileCreateAlways | kSbFileCreateOnly))) {
    *out_created = true;
  }

  /// \todo base error <-> sb error
  /*if (out_error) {
    if (descriptor >= 0) {
      *out_error = kSbFileOk;
    } else {
      switch (errno) {
        case EACCES:
        case EISDIR:
        case EROFS:
        case EPERM:
          *out_error = kSbFileErrorAccessDenied;
          break;
        case ETXTBSY:
          *out_error = kSbFileErrorInUse;
          break;
        case EEXIST:
          *out_error = kSbFileErrorExists;
          break;
        case ENOENT:
          *out_error = kSbFileErrorNotFound;
          break;
        case EMFILE:
          *out_error = kSbFileErrorTooManyOpened;
          break;
        case ENOMEM:
          *out_error = kSbFileErrorNoMemory;
          break;
        case ENOSPC:
          *out_error = kSbFileErrorNoSpace;
          break;
        case ENOTDIR:
          *out_error = kSbFileErrorNotADirectory;
          break;
#if SB_API_VERSION >= 11
        case EIO:
          *out_error = kSbFileErrorIO;
          break;
#endif
        default:
          *out_error = kSbFileErrorFailed;
      }
    }
  }*/

  /*if (descriptor < 0) {
    return kSbFileInvalid;
  }*/

  if (!descriptor.IsValid() 
      || descriptor.error_details() != base::File::FILE_OK) {
    return kSbFileInvalid;
  }

  SbFile result = new SbFilePrivate();
  result->descriptor = std::move(descriptor);
  return result;
#endif // 0

// TODO: remove commented out code 

#if 0
  if (flags & kSbFileCreateOnly) {
    open_flags = O_CREAT | O_EXCL;
  }

  if (out_created) {
    *out_created = false;
  }

  if (flags & kSbFileCreateAlways) {
    SB_DCHECK(!open_flags);
    open_flags = O_CREAT | O_TRUNC;
  }

  if (flags & kSbFileOpenTruncated) {
    SB_DCHECK(!open_flags);
    SB_DCHECK(flags & kSbFileWrite);
    open_flags = O_TRUNC;
  }

  if (!open_flags && !(flags & kSbFileOpenOnly) &&
      !(flags & kSbFileOpenAlways)) {
    SB_NOTREACHED();
    errno = EOPNOTSUPP;
    if (out_error) {
      *out_error = kSbFileErrorFailed;
    }

    return kSbFileInvalid;
  }

  if (flags & kSbFileWrite && flags & kSbFileRead) {
    open_flags |= O_RDWR;
  } else if (flags & kSbFileWrite) {
    open_flags |= O_WRONLY;
  }

  SB_COMPILE_ASSERT(O_RDONLY == 0, O_RDONLY_must_equal_zero);

  int mode = S_IRUSR | S_IWUSR;
  int descriptor = HANDLE_EINTR(open(path, open_flags, mode));

  if (flags & kSbFileOpenAlways) {
    if (descriptor < 0) {
      open_flags |= O_CREAT;
      descriptor = HANDLE_EINTR(open(path, open_flags, mode));
      if (out_created && descriptor >= 0) {
        *out_created = true;
      }
    }
  }

  if (out_created && (descriptor >= 0) &&
      (flags & (kSbFileCreateAlways | kSbFileCreateOnly))) {
    *out_created = true;
  }

  if (out_error) {
    if (descriptor >= 0) {
      *out_error = kSbFileOk;
    } else {
      switch (errno) {
        case EACCES:
        case EISDIR:
        case EROFS:
        case EPERM:
          *out_error = kSbFileErrorAccessDenied;
          break;
        case ETXTBSY:
          *out_error = kSbFileErrorInUse;
          break;
        case EEXIST:
          *out_error = kSbFileErrorExists;
          break;
        case ENOENT:
          *out_error = kSbFileErrorNotFound;
          break;
        case EMFILE:
          *out_error = kSbFileErrorTooManyOpened;
          break;
        case ENOMEM:
          *out_error = kSbFileErrorNoMemory;
          break;
        case ENOSPC:
          *out_error = kSbFileErrorNoSpace;
          break;
        case ENOTDIR:
          *out_error = kSbFileErrorNotADirectory;
          break;
#if SB_API_VERSION >= 11
        case EIO:
          *out_error = kSbFileErrorIO;
          break;
#endif
        default:
          *out_error = kSbFileErrorFailed;
      }
    }
  }
#endif // 0

}  // namespace impl
}  // namespace win
}  // namespace shared
}  // namespace starboard

#endif  // STARBOARD_SHARED_WIN_IMPL_FILE_OPEN_H_
