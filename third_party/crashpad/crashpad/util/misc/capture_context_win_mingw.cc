// Copyright 2026 The Crashpad Authors
// Use of this source code is governed by the Apache License, Version 2.0.

#include "util/misc/capture_context.h"

namespace crashpad {

// MinGW has no MASM; the context is captured one frame deeper.
void CaptureContext(NativeCPUContext* context) {
  RtlCaptureContext(context);
}

}  // namespace crashpad
