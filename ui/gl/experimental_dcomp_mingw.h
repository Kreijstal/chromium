// Copyright 2026 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// MinGW: the one experimental DComp interface Chromium uses, from
// DirectX-Headers include/experimental-composition/experimental-dcomp.h.
// That header's overloads are in MSVC vtable order, so it is not used.

#ifndef UI_GL_EXPERIMENTAL_DCOMP_MINGW_H_
#define UI_GL_EXPERIMENTAL_DCOMP_MINGW_H_

#include <windows.h>

#include <dcomp.h>

#undef INTERFACE
#define INTERFACE EXPERIMENTAL_IDCompositionDevice6
DECLARE_INTERFACE_IID_(EXPERIMENTAL_IDCompositionDevice6,
                       IDCompositionDevice5,
                       "4CA97A18-CBFD-4B0D-89E1-F7FA86D8D63E") {
  STDMETHOD(PresentCompositionTextures)(THIS_ IUnknown* const* pCommandQueue,
                                        UINT queueCount) PURE;
};
#undef INTERFACE
__CRT_UUID_DECL(EXPERIMENTAL_IDCompositionDevice6,
                0x4ca97a18, 0xcbfd, 0x4b0d,
                0x89, 0xe1, 0xf7, 0xfa, 0x86, 0xd8, 0xd6, 0x3e)

#endif  // UI_GL_EXPERIMENTAL_DCOMP_MINGW_H_
