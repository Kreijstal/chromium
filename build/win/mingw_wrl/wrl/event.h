// Copyright 2026 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Clean-room WRL Callback for mingw-w64. Written from the documented API,
// not the SDK headers. EventSource is not provided.

#ifndef _WRL_EVENT_H_
#define _WRL_EVENT_H_

#include <wrl/client.h>
#include <wrl/implements.h>

#include <new>
#include <type_traits>
#include <utility>

namespace Microsoft {
namespace WRL {
namespace Details {

template <typename... A>
struct ArgList {};

// Parameter list of Delegate::Invoke.
template <typename C, typename... A>
ArgList<A...> InvokeArgs(HRESULT (C::*)(A...));
#if defined(__i386__)
template <typename C, typename... A>
ArgList<A...> InvokeArgs(HRESULT (__stdcall C::*)(A...));
#endif

// D is a delegate interface or an Implements<> whose first interface is one.
template <typename D>
struct DelegateTraits {
  using Interface =
      std::conditional_t<IsImplements<D>::value,
                         typename ItemTraits<D>::Interface, D>;
  using Args = decltype(InvokeArgs(&Interface::Invoke));
};

template <typename D, typename F, typename Args>
class CallbackImpl;

template <typename D, typename F, typename... A>
class CallbackImpl<D, F, ArgList<A...>>
    : public RuntimeClass<RuntimeClassFlags<Delegate>, D> {
 public:
  template <typename G>
  explicit CallbackImpl(G&& g) : callback_(std::forward<G>(g)) {}

  HRESULT STDMETHODCALLTYPE Invoke(A... args) override {
    return callback_(args...);
  }

 private:
  F callback_;
};

}  // namespace Details

template <typename D, typename F>
ComPtr<typename Details::DelegateTraits<D>::Interface> Callback(F&& f) {
  using Traits = Details::DelegateTraits<D>;
  using Impl = Details::CallbackImpl<D, std::decay_t<F>, typename Traits::Args>;
  ComPtr<typename Traits::Interface> p;
  if (Impl* impl = new (std::nothrow) Impl(std::forward<F>(f)))
    p.Attach(static_cast<typename Traits::Interface*>(impl));
  return p;
}

template <typename D, typename O, typename M>
ComPtr<typename Details::DelegateTraits<D>::Interface> Callback(O* object,
                                                               M method) {
  return Callback<D>([object, method](auto&&... args) -> HRESULT {
    return (object->*method)(std::forward<decltype(args)>(args)...);
  });
}

}  // namespace WRL
}  // namespace Microsoft

#endif  // _WRL_EVENT_H_
