// Copyright 2016 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_VR_VR_EXPORT_H_
#define DEVICE_VR_VR_EXPORT_H_

#if defined(COMPONENT_BUILD)
#if defined(COMPILER_MSVC)

#if defined(DEVICE_VR_IMPLEMENTATION)
#define DEVICE_VR_EXPORT __declspec(dllexport)
#else
#define DEVICE_VR_EXPORT __declspec(dllimport)
#endif

#else // !defined(COMPILER_MSVC)

#define DEVICE_VR_EXPORT __attribute__((visibility("default")))

#endif  // defined(COMPILER_MSVC)

#else
#define DEVICE_VR_EXPORT
#endif

#endif  // DEVICE_VR_VR_EXPORT_H_
