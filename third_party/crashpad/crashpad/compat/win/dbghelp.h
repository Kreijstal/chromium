// Copyright 2026 The Crashpad Authors
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

#ifndef CRASHPAD_COMPAT_WIN_DBGHELP_H_
#define CRASHPAD_COMPAT_WIN_DBGHELP_H_

#ifdef _MSC_VER
#include <../um/dbghelp.h>
#else
#include_next <dbghelp.h>
#endif

#if defined(__MINGW32__)
// Missing from mingw-w64; layouts from the Windows SDK.
#ifndef MINIDUMP_MISC3_PROCESS_INTEGRITY
#define MINIDUMP_MISC3_PROCESS_INTEGRITY 0x00000010
#define MINIDUMP_MISC3_PROCESS_EXECUTE_FLAGS 0x00000020
#define MINIDUMP_MISC3_TIMEZONE 0x00000040
#define MINIDUMP_MISC3_PROTECTED_PROCESS 0x00000080
#define MINIDUMP_MISC4_BUILDSTRING 0x00000100
#define MINIDUMP_MISC5_PROCESS_COOKIE 0x00000200
#endif

#pragma pack(push, 4)
typedef struct _MINIDUMP_MISC_INFO_3 : MINIDUMP_MISC_INFO_2 {
  ULONG32 ProcessIntegrityLevel;
  ULONG32 ProcessExecuteFlags;
  ULONG32 ProtectedProcess;
  ULONG32 TimeZoneId;
  TIME_ZONE_INFORMATION TimeZone;
} MINIDUMP_MISC_INFO_3, *PMINIDUMP_MISC_INFO_3;

typedef struct _MINIDUMP_MISC_INFO_4 : MINIDUMP_MISC_INFO_3 {
  WCHAR BuildString[MAX_PATH];
  WCHAR DbgBldStr[40];
} MINIDUMP_MISC_INFO_4, *PMINIDUMP_MISC_INFO_4;

typedef struct _XSTATE_CONFIG_FEATURE_MSC_INFO {
  ULONG32 SizeOfInfo;
  ULONG32 ContextSize;
  ULONG64 EnabledFeatures;
  XSTATE_FEATURE Features[MAXIMUM_XSTATE_FEATURES];
} XSTATE_CONFIG_FEATURE_MSC_INFO, *PXSTATE_CONFIG_FEATURE_MSC_INFO;

typedef struct _MINIDUMP_MISC_INFO_5 : MINIDUMP_MISC_INFO_4 {
  XSTATE_CONFIG_FEATURE_MSC_INFO XStateData;
  ULONG32 ProcessCookie;
} MINIDUMP_MISC_INFO_5, *PMINIDUMP_MISC_INFO_5;
typedef MINIDUMP_MISC_INFO_5 MINIDUMP_MISC_INFO_N;
typedef MINIDUMP_MISC_INFO_N* PMINIDUMP_MISC_INFO_N;

typedef struct _MINIDUMP_THREAD_NAME {
  ULONG ThreadId;
  ULONG64 RvaOfThreadName;
} MINIDUMP_THREAD_NAME, *PMINIDUMP_THREAD_NAME;

typedef struct _MINIDUMP_THREAD_NAME_LIST {
  ULONG NumberOfThreadNames;
  MINIDUMP_THREAD_NAME ThreadNames[0];
} MINIDUMP_THREAD_NAME_LIST, *PMINIDUMP_THREAD_NAME_LIST;
#pragma pack(pop)

static_assert(sizeof(MINIDUMP_MISC_INFO_5) == 1364, "MINIDUMP_MISC_INFO_5");
static_assert(sizeof(MINIDUMP_THREAD_NAME) == 12, "MINIDUMP_THREAD_NAME");
#endif  // __MINGW32__

#endif  // CRASHPAD_COMPAT_WIN_DBGHELP_H_
