// Copyright 2025 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_HEAP_HEAP_VECTOR_HOLDER_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_HEAP_HEAP_VECTOR_HOLDER_H_

#include "third_party/blink/renderer/platform/heap/collection_support/heap_vector.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"

namespace blink {

// DisallowNewWrapper<HeapVector<T>> for GCC, which does not see HeapVector as
// disallow-new.
template <typename T>
class HeapVectorHolder final : public GarbageCollected<HeapVectorHolder<T>> {
 public:
  HeapVector<T>& Value() { return value_; }
  const HeapVector<T>& Value() const { return value_; }
  void Trace(Visitor* visitor) const { visitor->Trace(value_); }

 private:
  HeapVector<T> value_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_HEAP_HEAP_VECTOR_HOLDER_H_
