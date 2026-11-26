// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// HarfBuzz FontFormatCheck for builds without Crubit (from Chromium 140).

#include "third_party/blink/renderer/platform/fonts/opentype/font_format_check.h"

// Include HarfBuzz to have a cross-platform way to retrieve table tags without
// having to rely on the platform being able to instantiate this font format.
#include <hb.h>

#include <hb-cplusplus.hh>

#include "base/compiler_specific.h"
#include "base/containers/span.h"
#include "base/numerics/byte_conversions.h"
#include "base/numerics/safe_conversions.h"
#include "third_party/blink/renderer/platform/runtime_enabled_features.h"
#include "third_party/skia/include/core/SkTypeface.h"

namespace blink {

namespace {

// Returns the table bytes, or an empty span if shorter than `min_size`.
base::span<const uint8_t> TableHeader(hb_blob_t* table_blob,
                                      unsigned min_size) {
  unsigned length = 0;
  const char* data = hb_blob_get_data(table_blob, &length);
  if (!data || length < min_size) {
    return {};
  }
  // SAFETY: hb_blob_get_data() returns `length` bytes at `data`.
  return base::as_bytes(UNSAFE_BUFFERS(base::span(data, length)));
}

std::optional<uint16_t> ReadTableVersion(hb_face_t* face,
                                         hb_tag_t tag,
                                         unsigned min_size) {
  hb::unique_ptr<hb_blob_t> table_blob(hb_face_reference_table(face, tag));
  base::span<const uint8_t> data = TableHeader(table_blob.get(), min_size);
  if (data.size() < 2u) {
    return std::nullopt;
  }
  return base::U16FromBigEndian(data.first<2u>());
}

}  // namespace

FontFormatCheck::FontFormatCheck(sk_sp<SkData> sk_data) {
  hb::unique_ptr<hb_blob_t> font_blob(
      hb_blob_create(reinterpret_cast<const char*>(sk_data->bytes()),
                     base::checked_cast<unsigned>(sk_data->size()),
                     HB_MEMORY_MODE_READONLY, nullptr, nullptr));
  hb::unique_ptr<hb_face_t> face(hb_face_create(font_blob.get(), 0));

  unsigned table_count = hb_face_get_table_tags(face.get(), 0, nullptr, nullptr);
  table_tags_.resize(table_count);
  if (!hb_face_get_table_tags(face.get(), 0, &table_count,
                              table_tags_.data())) {
    table_tags_.resize(0);
  }

  // https://docs.microsoft.com/en-us/typography/opentype/spec/colr#header
  if (HasTables({HB_TAG('C', 'O', 'L', 'R'), HB_TAG('C', 'P', 'A', 'L')})) {
    colr_version_ = ReadTableVersion(face.get(), HB_TAG('C', 'O', 'L', 'R'),
                                     /*min_size=*/14u);
  }
  // avar header: majorVersion, minorVersion. Any major >= 2 counts as avar2.
  if (HasTables({HB_TAG('a', 'v', 'a', 'r')})) {
    avar_major_version_ = ReadTableVersion(
        face.get(), HB_TAG('a', 'v', 'a', 'r'), /*min_size=*/4u);
  }
}

FontFormatCheck::~FontFormatCheck() = default;

bool FontFormatCheck::HasTables(std::initializer_list<uint32_t> tags) const {
  if (table_tags_.empty()) {
    return false;
  }
  for (uint32_t tag : tags) {
    if (!table_tags_.Contains(tag)) {
      return false;
    }
  }
  return true;
}

bool FontFormatCheck::IsVariableFont() const {
  return HasTables({HB_TAG('f', 'v', 'a', 'r')});
}

bool FontFormatCheck::IsCbdtCblcColorFont() const {
  return HasTables({HB_TAG('C', 'B', 'D', 'T'), HB_TAG('C', 'B', 'L', 'C')});
}

bool FontFormatCheck::IsEbdtEblcMonochromeFont() const {
  return HasTables({HB_TAG('E', 'B', 'D', 'T'), HB_TAG('E', 'B', 'L', 'C')});
}

bool FontFormatCheck::IsColrCpalColorFontV0() const {
  return colr_version_ == 0;
}

bool FontFormatCheck::IsColrCpalColorFontV1() const {
  return colr_version_ == 1;
}

bool FontFormatCheck::IsSbixColorFont() const {
  return HasTables({HB_TAG('s', 'b', 'i', 'x')});
}

bool FontFormatCheck::IsCff2OutlineFont() const {
  return HasTables({HB_TAG('C', 'F', 'F', '2')});
}

bool FontFormatCheck::IsVariableColrV0Font() const {
  return IsColrCpalColorFontV0() && IsVariableFont();
}

bool FontFormatCheck::IsColorFont() const {
  return IsSbixColorFont() || IsCbdtCblcColorFont() ||
         IsColrCpalColorFontV0() || IsColrCpalColorFontV1();
}

bool FontFormatCheck::IsAvar2Font() const {
  return RuntimeEnabledFeatures::FontFormatAvar2Enabled() &&
         avar_major_version_.has_value() && *avar_major_version_ >= 2;
}

FontFormatCheck::VariableFontSubType FontFormatCheck::ProbeVariableFont(
    sk_sp<SkTypeface> typeface) {
  if (!typeface->getTableSize(
          SkFontTableTag(SkSetFourByteTag('f', 'v', 'a', 'r'))))
    return VariableFontSubType::kNotVariable;

  if (typeface->getTableSize(
          SkFontTableTag(SkSetFourByteTag('C', 'F', 'F', '2'))))
    return VariableFontSubType::kVariableCff2;
  return VariableFontSubType::kVariableTrueType;
}

}  // namespace blink
