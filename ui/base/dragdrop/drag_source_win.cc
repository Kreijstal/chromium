// Copyright 2011 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/dragdrop/drag_source_win.h"

#include "ui/base/dragdrop/os_exchange_data_provider_win.h"

namespace ui {

Microsoft::WRL::ComPtr<DragSourceWin> DragSourceWin::Create() {
#if defined(__MINGW32__)
  return Microsoft::WRL::ComPtr<DragSourceWin>(new DragSourceWin());
#else
  return Microsoft::WRL::Make<DragSourceWin>();
#endif
}

DragSourceWin::DragSourceWin() = default;

HRESULT DragSourceWin::QueryContinueDrag(BOOL escape_pressed, DWORD key_state) {
  if (cancel_drag_ || escape_pressed)
    return DRAGDROP_S_CANCEL;

  if (!(key_state & MK_LBUTTON) && !(key_state & MK_RBUTTON)) {
    OnDragSourceDrop();
    return DRAGDROP_S_DROP;
  }

  return S_OK;
}

HRESULT DragSourceWin::GiveFeedback(DWORD effect) {
  return DRAGDROP_S_USEDEFAULTCURSORS;
}

void DragSourceWin::OnDragSourceDrop() {
  CHECK(data_, base::NotFatalUntil::M163);
  OSExchangeDataProviderWin::GetDataObjectImpl(*data_)->set_in_drag_loop(false);
}

#if defined(__MINGW32__)
HRESULT DragSourceWin::QueryInterface(REFIID iid, void** object) {
  if (!object) {
    return E_POINTER;
  }
  if (iid == IID_IUnknown || iid == IID_IDropSource) {
    *object = static_cast<IDropSource*>(this);
    AddRef();
    return S_OK;
  }
  *object = nullptr;
  return E_NOINTERFACE;
}

ULONG DragSourceWin::AddRef() {
  return ::InterlockedIncrement(&ref_count_);
}

ULONG DragSourceWin::Release() {
  const LONG count = ::InterlockedDecrement(&ref_count_);
  if (count == 0) {
    delete this;
  }
  return count;
}
#endif

}  // namespace ui
