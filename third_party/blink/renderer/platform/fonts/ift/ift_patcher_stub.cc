// Copyright 2026 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Without Crubit there is no IFT patcher: Create() always returns nullptr, so
// no IftPatcher instance can exist.

#include "third_party/blink/renderer/platform/fonts/ift/ift_patcher.h"

#include "base/notreached.h"

namespace blink {

IftSubsetDefinition::IftSubsetDefinition() = default;

IftSubsetDefinition::~IftSubsetDefinition() = default;

IftSubsetDefinition::IftSubsetDefinition(IftSubsetDefinition&&) = default;

IftSubsetDefinition& IftSubsetDefinition::operator=(IftSubsetDefinition&&) =
    default;

IftSubsetDefinition::IftSubsetDefinition(const IftSubsetDefinition&) = default;

IftSubsetDefinition& IftSubsetDefinition::operator=(
    const IftSubsetDefinition&) = default;

bool IftSubsetDefinition::AddCodepoint(uint32_t codepoint) {
  return false;
}

bool IftSubsetDefinition::AddFeatureTag(uint32_t tag) {
  return false;
}

bool IftSubsetDefinition::AddDesignSpace(uint32_t tag,
                                         double min_value,
                                         double max_value) {
  return false;
}

void IftSubsetDefinition::Union(const IftSubsetDefinition& other) {}

void IftSubsetDefinition::Clear() {}

IftPatcher::~IftPatcher() = default;

IftPatcher::IftPatcher(IftPatcher&&) = default;

IftPatcher& IftPatcher::operator=(IftPatcher&&) = default;

std::unique_ptr<IftPatcher> IftPatcher::Create(
    base::span<const uint8_t> font_data) {
  return nullptr;
}

std::unique_ptr<IftPatcher> IftPatcher::Clone() const {
  NOTREACHED();
}

base::span<const uint8_t> IftPatcher::GetFontData() const {
  NOTREACHED();
}

bool IftPatcher::RenderedFontRequiresPatches(
    const IftSubsetDefinition& subset) const {
  NOTREACHED();
}

bool IftPatcher::HasUnrenderedChanges() const {
  NOTREACHED();
}

void IftPatcher::MarkFontDataRendered() {
  NOTREACHED();
}

void IftPatcher::BeginExtension() {
  NOTREACHED();
}

base::expected<Vector<String>, IftPatcher::RequestError>
IftPatcher::RequestPatches(const IftSubsetDefinition& subset) {
  NOTREACHED();
}

void IftPatcher::AddPatchData(const String& url,
                              base::span<const uint8_t> data) {
  NOTREACHED();
}

IftPatcher::ApplyStatus IftPatcher::ApplyPatches(
    const IftSubsetDefinition& subset) {
  NOTREACHED();
}

bool IftPatcher::HasPendingPatchRequests() const {
  NOTREACHED();
}

void IftPatcher::SetPatchLimitsForTesting(uint32_t max_invalidating_patches,
                                          uint32_t max_total_patches) {
  NOTREACHED();
}

}  // namespace blink
