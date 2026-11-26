// Copyright 2019 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// windows.h needs to be included before delayimp.h.
#include <windows.h>

#if !defined(__MINGW32__)  // mingw-w64 delayimp.h has no include guard
#include <delayimp.h>
#endif

#include "chrome/common/win/delay_load_failure_support.h"

// Set the delay load failure hook to the common failure handler so that crash
// reports are generated rather than structured exceptions being thrown.
//
// The |__pfnDliFailureHook2| failure notification hook gets called
// automatically by the delay load runtime in case of failure, see
// https://docs.microsoft.com/en-us/cpp/build/reference/failure-hooks?view=vs-2019
// for more information about this.
#if defined(__MINGW32__)
// mingw-w64's delayimp.h declares the hook non-const.
extern "C" PfnDliHook __pfnDliFailureHook2 = HandleDelayLoadFailureCommon;
#else
extern "C" const PfnDliHook __pfnDliFailureHook2 = HandleDelayLoadFailureCommon;
#endif
