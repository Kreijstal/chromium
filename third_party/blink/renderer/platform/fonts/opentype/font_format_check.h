// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_OPENTYPE_FONT_FORMAT_CHECK_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_OPENTYPE_FONT_FORMAT_CHECK_H_

#include <stdint.h>

#include <initializer_list>
#include <optional>

#include "build/buildflag.h"
#include "third_party/blink/renderer/platform/fonts/font_rust_buildflags.h"
#if BUILDFLAG(ENABLE_CPP_API_FROM_RUST)
#include "third_party/blink/renderer/platform/fonts/font_format.h"
#else
#include "third_party/blink/renderer/platform/wtf/vector.h"
#endif
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/allocator/allocator.h"
#include "third_party/skia/include/core/SkData.h"
#include "third_party/skia/include/core/SkRefCnt.h"
#include "third_party/skia/include/core/SkTypeface.h"

namespace blink {

class PLATFORM_EXPORT FontFormatCheck {
  STACK_ALLOCATED();

 public:
  explicit FontFormatCheck(sk_sp<SkData>);
  virtual ~FontFormatCheck();
  virtual bool IsVariableFont() const;
  virtual bool IsCbdtCblcColorFont() const;
  virtual bool IsEbdtEblcMonochromeFont() const;
  virtual bool IsColrCpalColorFontV0() const;
  virtual bool IsColrCpalColorFontV1() const;
  bool IsVariableColrV0Font() const;
  virtual bool IsSbixColorFont() const;
  virtual bool IsCff2OutlineFont() const;
  bool IsColorFont() const;
  bool IsAvar2Font() const;

  // Still needed in FontCustomPlatformData.
  enum class VariableFontSubType {
    kNotVariable,
    kVariableTrueType,
    kVariableCff2
  };

  static VariableFontSubType ProbeVariableFont(sk_sp<SkTypeface>);

 private:
#if BUILDFLAG(ENABLE_CPP_API_FROM_RUST)
  font_format::FontFormatInfo format_info_;
#else
  bool HasTables(std::initializer_list<uint32_t> tags) const;

  Vector<uint32_t> table_tags_;
  std::optional<uint16_t> colr_version_;
  std::optional<uint16_t> avar_major_version_;
#endif
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_OPENTYPE_FONT_FORMAT_CHECK_H_
