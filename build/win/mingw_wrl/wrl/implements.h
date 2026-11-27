// Copyright 2026 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Clean-room WRL RuntimeClass/Implements/Make for mingw-w64, which ships
// only wrl/client.h. Written from the documented API, not the SDK headers.

#ifndef _WRL_IMPLEMENTS_H_
#define _WRL_IMPLEMENTS_H_

#include <inspectable.h>
#include <objbase.h>
#include <objidl.h>
#include <unknwn.h>
#include <windows.h>
#include <winstring.h>
#include <wrl/client.h>

#include <cwchar>
#include <new>
#include <type_traits>
#include <utility>

namespace Microsoft {
namespace WRL {

enum RuntimeClassType {
  WinRt = 0x0001,
  ClassicCom = 0x0002,
  WinRtClassicComMix = WinRt | ClassicCom,
  InhibitWeakReference = 0x0004,
  Delegate = ClassicCom,
  InhibitFtmBase = 0x0008,
  InhibitRoOriginateError = 0x0010,
};

template <unsigned int flags>
struct RuntimeClassFlags {
  static constexpr unsigned int value = flags;
};

// QI exposes I and each of Bases (which must be bases of I).
template <typename I, typename... Bases>
struct ChainInterfaces : I {};

// QI-able, but not reported by GetIids.
template <typename I>
struct CloakedIid : I {};

// Aggregates the free-threaded marshaler; answers IMarshal and IAgileObject.
class FtmBase : public IMarshal, public IAgileObject {
 protected:
  FtmBase() {
    ComPtr<IUnknown> ftm;
    if (SUCCEEDED(::CoCreateFreeThreadedMarshaler(nullptr, &ftm)))
      ftm.As(&marshaller_);
  }

 public:
  HRESULT STDMETHODCALLTYPE GetUnmarshalClass(REFIID riid, void* pv,
                                              DWORD ctx, void* pv_ctx,
                                              DWORD flags,
                                              CLSID* cid) override {
    return marshaller_ ? marshaller_->GetUnmarshalClass(riid, pv, ctx, pv_ctx,
                                                        flags, cid)
                       : E_UNEXPECTED;
  }
  HRESULT STDMETHODCALLTYPE GetMarshalSizeMax(REFIID riid, void* pv,
                                              DWORD ctx, void* pv_ctx,
                                              DWORD flags,
                                              DWORD* size) override {
    return marshaller_ ? marshaller_->GetMarshalSizeMax(riid, pv, ctx, pv_ctx,
                                                        flags, size)
                       : E_UNEXPECTED;
  }
  HRESULT STDMETHODCALLTYPE MarshalInterface(IStream* stm, REFIID riid,
                                             void* pv, DWORD ctx,
                                             void* pv_ctx,
                                             DWORD flags) override {
    return marshaller_ ? marshaller_->MarshalInterface(stm, riid, pv, ctx,
                                                       pv_ctx, flags)
                       : E_UNEXPECTED;
  }
  HRESULT STDMETHODCALLTYPE UnmarshalInterface(IStream* stm, REFIID riid,
                                               void** ppv) override {
    return marshaller_ ? marshaller_->UnmarshalInterface(stm, riid, ppv)
                       : E_UNEXPECTED;
  }
  HRESULT STDMETHODCALLTYPE ReleaseMarshalData(IStream* stm) override {
    return marshaller_ ? marshaller_->ReleaseMarshalData(stm) : E_UNEXPECTED;
  }
  HRESULT STDMETHODCALLTYPE DisconnectObject(DWORD reserved) override {
    return marshaller_ ? marshaller_->DisconnectObject(reserved)
                       : E_UNEXPECTED;
  }

 protected:
  ComPtr<IMarshal> marshaller_;
};

template <typename... I>
class Implements;
template <typename... I>
class RuntimeClass;

namespace Details {

template <typename T>
struct IsFlags : std::false_type {};
template <unsigned int F>
struct IsFlags<RuntimeClassFlags<F>> : std::true_type {};

template <typename T>
struct IsImplements : std::false_type {};
template <typename... X>
struct IsImplements<Implements<X...>> : std::true_type {};

// Per-item QI / GetIids behaviour. S is any class having the item as a
// unique base.
template <typename I>
struct ItemTraits {
  using Interface = I;
  static constexpr bool kInspectable = std::is_base_of<IInspectable, I>::value;
  static constexpr bool kHasFtm = false;
  static constexpr unsigned long kIidCount = 1;
  static void FillIids(IID*& out) { *out++ = __uuidof(I); }
  template <typename S>
  static void* Cast(S* s, REFIID riid) {
    return IsEqualGUID(riid, __uuidof(I)) ? static_cast<I*>(s) : nullptr;
  }
  template <typename S>
  static IUnknown* Unknown(S* s) {
    return static_cast<I*>(s);
  }
  template <typename S>
  static IInspectable* Inspectable(S* s) {
    if constexpr (kInspectable) {
      return static_cast<I*>(s);
    } else {
      return nullptr;
    }
  }
};

template <typename I, typename... B>
struct ItemTraits<ChainInterfaces<I, B...>> {
  using C = ChainInterfaces<I, B...>;
  static_assert((std::is_base_of<B, I>::value && ...),
                "ChainInterfaces bases must be bases of the first interface");
  using Interface = I;
  static constexpr bool kInspectable = std::is_base_of<IInspectable, I>::value;
  static constexpr bool kHasFtm = false;
  static constexpr unsigned long kIidCount = 1 + sizeof...(B);
  static void FillIids(IID*& out) {
    *out++ = __uuidof(I);
    ((*out++ = __uuidof(B)), ...);
  }
  template <typename S>
  static void* Cast(S* s, REFIID riid) {
    I* p = static_cast<C*>(s);
    if (IsEqualGUID(riid, __uuidof(I)))
      return p;
    void* r = nullptr;
    ((r = (!r && IsEqualGUID(riid, __uuidof(B))) ? static_cast<B*>(p) : r),
     ...);
    return r;
  }
  template <typename S>
  static IUnknown* Unknown(S* s) {
    return static_cast<I*>(static_cast<C*>(s));
  }
  template <typename S>
  static IInspectable* Inspectable(S* s) {
    if constexpr (kInspectable) {
      return static_cast<I*>(static_cast<C*>(s));
    } else {
      return nullptr;
    }
  }
};

template <typename I>
struct ItemTraits<CloakedIid<I>> {
  using C = CloakedIid<I>;
  using Interface = I;
  static constexpr bool kInspectable = std::is_base_of<IInspectable, I>::value;
  static constexpr bool kHasFtm = false;
  static constexpr unsigned long kIidCount = 0;
  static void FillIids(IID*&) {}
  template <typename S>
  static void* Cast(S* s, REFIID riid) {
    return IsEqualGUID(riid, __uuidof(I)) ? static_cast<I*>(static_cast<C*>(s))
                                          : nullptr;
  }
  template <typename S>
  static IUnknown* Unknown(S* s) {
    return static_cast<I*>(static_cast<C*>(s));
  }
  template <typename S>
  static IInspectable* Inspectable(S* s) {
    if constexpr (kInspectable) {
      return static_cast<I*>(static_cast<C*>(s));
    } else {
      return nullptr;
    }
  }
};

template <>
struct ItemTraits<FtmBase> {
  using Interface = void;
  static constexpr bool kInspectable = false;
  static constexpr bool kHasFtm = true;
  static constexpr unsigned long kIidCount = 0;
  static void FillIids(IID*&) {}
  template <typename S>
  static void* Cast(S* s, REFIID riid) {
    FtmBase* f = static_cast<FtmBase*>(s);
    if (IsEqualGUID(riid, __uuidof(IMarshal)))
      return static_cast<IMarshal*>(f);
    if (IsEqualGUID(riid, __uuidof(IAgileObject)))
      return static_cast<IAgileObject*>(f);
    return nullptr;
  }
  template <typename S>
  static IUnknown* Unknown(S* s) {
    return static_cast<IMarshal*>(static_cast<FtmBase*>(s));
  }
  template <typename S>
  static IInspectable* Inspectable(S*) {
    return nullptr;
  }
};

template <typename... T>
struct FirstNonVoid {
  using type = void;
};
template <typename T, typename... R>
struct FirstNonVoid<T, R...> {
  using type = std::conditional_t<std::is_void<T>::value,
                                  typename FirstNonVoid<R...>::type, T>;
};

// Common base of Implements and RuntimeClass: derives from every item and
// knows how to cast to them. No IUnknown implementation here.
template <typename Flags, typename... Items>
class ImplementsCore : public Items... {
  static_assert(sizeof...(Items) > 0, "at least one interface is required");

 public:
  static constexpr unsigned int kWrlFlags = Flags::value;
  static constexpr bool kWrlInspectable =
      (ItemTraits<Items>::kInspectable || ...);
  static constexpr bool kWrlHasFtm = (ItemTraits<Items>::kHasFtm || ...);
  static constexpr unsigned long kWrlIidCount =
      (0ul + ... + ItemTraits<Items>::kIidCount);
  using WrlFirstInterface =
      typename FirstNonVoid<typename ItemTraits<Items>::Interface...>::type;

  static void WrlFillIids(IID*& out) { (ItemTraits<Items>::FillIids(out), ...); }
  static void* WrlCast(ImplementsCore* self, REFIID riid) {
    void* r = nullptr;
    ((r = r ? r : ItemTraits<Items>::Cast(self, riid)), ...);
    return r;
  }
  static IUnknown* WrlUnknown(ImplementsCore* self) {
    return WrlFirstUnknown<Items...>(self);
  }
  static IInspectable* WrlInspectable(ImplementsCore* self) {
    IInspectable* r = nullptr;
    ((r = r ? r : ItemTraits<Items>::Inspectable(self)), ...);
    return r;
  }

 protected:
  ImplementsCore() = default;
  ImplementsCore(const ImplementsCore&) = delete;
  ImplementsCore& operator=(const ImplementsCore&) = delete;

 private:
  template <typename F, typename... R>
  static IUnknown* WrlFirstUnknown(ImplementsCore* self) {
    return ItemTraits<F>::Unknown(self);
  }
};

template <typename... X>
struct ItemTraits<Implements<X...>> {
  using C = Implements<X...>;
  using Interface = typename C::WrlFirstInterface;
  static constexpr bool kInspectable = C::kWrlInspectable;
  static constexpr bool kHasFtm = C::kWrlHasFtm;
  static constexpr unsigned long kIidCount = C::kWrlIidCount;
  static void FillIids(IID*& out) { C::WrlFillIids(out); }
  template <typename S>
  static void* Cast(S* s, REFIID riid) {
    return C::WrlCast(static_cast<C*>(s), riid);
  }
  template <typename S>
  static IUnknown* Unknown(S* s) {
    return C::WrlUnknown(static_cast<C*>(s));
  }
  template <typename S>
  static IInspectable* Inspectable(S* s) {
    return C::WrlInspectable(static_cast<C*>(s));
  }
};

template <typename Flags, typename... Items>
struct NeedsFtm {
  static constexpr bool value =
      (Flags::value & WinRt) && !(Flags::value & InhibitFtmBase) &&
      !(ItemTraits<Items>::kHasFtm || ...);
};

template <bool kAddFtm, typename Flags, typename... Items>
struct SelectCore {
  using type = ImplementsCore<Flags, Items...>;
};
template <typename Flags, typename... Items>
struct SelectCore<true, Flags, Items...> {
  using type = ImplementsCore<Flags, Items..., FtmBase>;
};

// IUnknown over Core, with atomic refcount and `delete this`.
template <typename Core, bool kInsp>
class UnknownImpl : public Core {
 public:
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
    if (!ppv)
      return E_POINTER;
    void* r;
    if (IsEqualGUID(riid, __uuidof(IUnknown))) {
      r = CastToUnknown();
    } else if (kInsp && IsEqualGUID(riid, __uuidof(IInspectable))) {
      r = Core::WrlInspectable(this);
    } else {
      r = Core::WrlCast(this, riid);
    }
    if (!r) {
      *ppv = nullptr;
      return E_NOINTERFACE;
    }
    InternalAddRef();
    *ppv = r;
    return S_OK;
  }
  template <typename Q>
  HRESULT STDMETHODCALLTYPE QueryInterface(Q** pp) {
    return QueryInterface(__uuidof(Q), reinterpret_cast<void**>(pp));
  }
  ULONG STDMETHODCALLTYPE AddRef() override { return InternalAddRef(); }
  ULONG STDMETHODCALLTYPE Release() override {
    ULONG r = InternalRelease();
    if (r == 0)
      delete this;
    return r;
  }

  IUnknown* CastToUnknown() { return Core::WrlUnknown(this); }

 protected:
  UnknownImpl() = default;
  virtual ~UnknownImpl() = default;

  ULONG InternalAddRef() {
    return static_cast<ULONG>(::InterlockedIncrement(&wrl_ref_count_));
  }
  ULONG InternalRelease() {
    return static_cast<ULONG>(::InterlockedDecrement(&wrl_ref_count_));
  }
  unsigned long GetRefCount() const {
    return static_cast<unsigned long>(wrl_ref_count_);
  }

 private:
  volatile LONG wrl_ref_count_ = 1;
};

// Adds IInspectable. Methods are virtual so InspectableClass can replace
// them even when no listed interface derives from IInspectable.
template <typename Core>
class InspectableImpl : public UnknownImpl<Core, true> {
 public:
  virtual HRESULT STDMETHODCALLTYPE GetIids(ULONG* count, IID** iids) {
    if (!count || !iids)
      return E_POINTER;
    *count = 0;
    *iids = nullptr;
    constexpr unsigned long n = Core::kWrlIidCount;
    if (n == 0)
      return S_OK;
    IID* p = static_cast<IID*>(::CoTaskMemAlloc(sizeof(IID) * n));
    if (!p)
      return E_OUTOFMEMORY;
    IID* out = p;
    Core::WrlFillIids(out);
    *count = n;
    *iids = p;
    return S_OK;
  }
  virtual HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* name) {
    if (!name)
      return E_POINTER;
    *name = nullptr;
    return E_NOTIMPL;
  }
  virtual HRESULT STDMETHODCALLTYPE GetTrustLevel(TrustLevel* level) {
    if (!level)
      return E_POINTER;
    *level = BaseTrust;
    return S_OK;
  }

 protected:
  InspectableImpl() = default;
};

template <typename Flags, typename... Items>
struct RuntimeClassBase {
  using CoreT =
      typename SelectCore<NeedsFtm<Flags, Items...>::value, Flags,
                          Items...>::type;
  static constexpr bool kInsp =
      (Flags::value & WinRt) || CoreT::kWrlInspectable;
  using type = std::conditional_t<kInsp, InspectableImpl<CoreT>,
                                  UnknownImpl<CoreT, false>>;
};

}  // namespace Details

// Mixin: derives from the listed items; IUnknown is left to RuntimeClass.
template <typename... I>
class Implements
    : public Details::ImplementsCore<RuntimeClassFlags<WinRt>, I...> {};
template <unsigned int F, typename... I>
class Implements<RuntimeClassFlags<F>, I...>
    : public Details::ImplementsCore<RuntimeClassFlags<F>, I...> {};

template <typename... I>
class RuntimeClass
    : public Details::RuntimeClassBase<RuntimeClassFlags<WinRt>, I...>::type {
 protected:
  RuntimeClass() = default;
};
template <unsigned int F, typename... I>
class RuntimeClass<RuntimeClassFlags<F>, I...>
    : public Details::RuntimeClassBase<RuntimeClassFlags<F>, I...>::type {
 protected:
  RuntimeClass() = default;
};

template <typename T, typename... Args>
ComPtr<T> Make(Args&&... args) {
  ComPtr<T> p;
  p.Attach(new (std::nothrow) T(std::forward<Args>(args)...));
  return p;
}

template <typename T, typename I, typename... Args>
HRESULT MakeAndInitialize(I** result, Args&&... args) {
  if (!result)
    return E_POINTER;
  *result = nullptr;
  ComPtr<T> p = Make<T>();
  if (!p)
    return E_OUTOFMEMORY;
  HRESULT hr = p->RuntimeClassInitialize(std::forward<Args>(args)...);
  if (FAILED(hr))
    return hr;
  if constexpr (std::is_convertible<T*, I*>::value) {
    *result = p.Detach();
    return S_OK;
  } else if constexpr (std::is_same<I, IUnknown>::value) {
    *result = p->CastToUnknown();
    (*result)->AddRef();
    return S_OK;
  } else {
    return p->QueryInterface(__uuidof(I), reinterpret_cast<void**>(result));
  }
}

template <typename T, typename I, typename... Args>
HRESULT MakeAndInitialize(Details::ComPtrRef<ComPtr<I>> result,
                          Args&&... args) {
  return MakeAndInitialize<T>(result.ReleaseAndGetAddressOf(),
                              std::forward<Args>(args)...);
}

template <typename T, typename I, typename... Args>
HRESULT MakeAndInitialize(ComPtr<I>* result, Args&&... args) {
  return MakeAndInitialize<T>(result->ReleaseAndGetAddressOf(),
                              std::forward<Args>(args)...);
}

namespace Details {
using ::Microsoft::WRL::Make;
using ::Microsoft::WRL::MakeAndInitialize;
}  // namespace Details

}  // namespace WRL
}  // namespace Microsoft

// Supplies GetRuntimeClassName/GetTrustLevel for a WinRt RuntimeClass.
#define InspectableClass(runtimeClassName, trustLevel)                      \
 public:                                                                    \
  static const wchar_t* STDMETHODCALLTYPE InternalGetRuntimeClassName() {   \
    return runtimeClassName;                                                \
  }                                                                         \
  static ::TrustLevel STDMETHODCALLTYPE InternalGetTrustLevel() {           \
    return trustLevel;                                                      \
  }                                                                         \
  HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* name) {            \
    if (!name)                                                              \
      return E_POINTER;                                                     \
    const wchar_t* s = InternalGetRuntimeClassName();                       \
    return ::WindowsCreateString(s, static_cast<UINT32>(::wcslen(s)), name); \
  }                                                                         \
  HRESULT STDMETHODCALLTYPE GetTrustLevel(::TrustLevel* level) {            \
    if (!level)                                                             \
      return E_POINTER;                                                     \
    *level = trustLevel;                                                    \
    return S_OK;                                                            \
  }                                                                         \
                                                                            \
 private:

#endif  // _WRL_IMPLEMENTS_H_
