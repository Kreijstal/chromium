// Copyright 2026 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Fontations needs Rust. Builds without it get null typefaces here.

#include "third_party/skia/include/ports/SkTypeface_fontations.h"

#include "third_party/skia/include/core/SkStream.h"

sk_sp<SkTypeface> SkTypeface_Make_Fontations(
    std::unique_ptr<SkStreamAsset> fontData,
    const SkFontArguments& args) {
  return nullptr;
}

sk_sp<SkTypeface> SkTypeface_Make_Fontations(sk_sp<SkData> fontData,
                                             const SkFontArguments& args) {
  return nullptr;
}
