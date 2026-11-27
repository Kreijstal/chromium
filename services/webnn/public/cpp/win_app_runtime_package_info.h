// Copyright 2025 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_WEBNN_PUBLIC_CPP_WIN_APP_RUNTIME_PACKAGE_INFO_H_
#define SERVICES_WEBNN_PUBLIC_CPP_WIN_APP_RUNTIME_PACKAGE_INFO_H_

#include <windows.h>

#include <appmodel.h>

#include <string_view>

#include "base/strings/cstring_view.h"
#include "base/win/windows_version.h"
#include "third_party/windows_app_sdk_headers/src/inc/abi/runtime/WindowsAppSDK-VersionInfo.h"

namespace webnn {

// The product ID of "MicrosoftCorporationII.WinAppRuntime.Main.2" package in
// Microsoft Store.
inline constexpr base::wcstring_view kWinAppRuntimeProductId = L"9NRZT3Q9R3DL";

inline constexpr auto kWinAppRuntimeSupportedMinVersion =
    base::win::Version::WIN11_24H2;

inline constexpr base::wcstring_view kWinAppRuntimePackageFamilyName =
    WINDOWSAPPSDK_RUNTIME_PACKAGE_FRAMEWORK_PACKAGEFAMILYNAME_W;

// In declaration order (GCC rejects out-of-order designators).
inline constexpr PACKAGE_VERSION kWinAppRuntimePackageMinVersion = {
    .Revision = WINDOWSAPPSDK_RUNTIME_VERSION_REVISION,
    .Build = WINDOWSAPPSDK_RUNTIME_VERSION_BUILD,
    .Minor = WINDOWSAPPSDK_RUNTIME_VERSION_MINOR,
    .Major = WINDOWSAPPSDK_RUNTIME_VERSION_MAJOR};

inline constexpr std::string_view kWinAppRuntimePackageMinVersionString =
    WINDOWSAPPSDK_RUNTIME_VERSION_DOTQUADSTRING;

}  // namespace webnn

#endif  // SERVICES_WEBNN_PUBLIC_CPP_WIN_APP_RUNTIME_PACKAGE_INFO_H_
