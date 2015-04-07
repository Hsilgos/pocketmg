#pragma once

#include <Windows.h>

template<class Handle>
class ScopedHandle {
  template<class Handle>
  struct ScopedStructBase {
    Handle handle_;
    ScopedStructBase(Handle handle) : handle_(handle) {}
    virtual ~ScopedStructBase() {}
  };

  template<class Handle, class Deleter>
  struct ScopedStruct : public ScopedStructBase<Handle> {
    Deleter deleter_;
    ScopedStruct(Handle handle, Deleter deleter) : ScopedStructBase<Handle>(handle), deleter_(deleter) {}
    ~ScopedStruct() {
      if (handle_)
        deleter_(handle_);
    }
  };

public:
  ScopedHandle() {}

  template<class Deleter> ScopedHandle(Handle handle, Deleter deleter) : scoped_(new ScopedStruct<Handle, Deleter>(handle, deleter)) {}

  template<class Deleter>
  void reset(Handle handle, Deleter deleter) {
    scoped_.reset(new ScopedStruct<Handle, Deleter>(handle, deleter));
  }

  Handle handle() {
    return scoped_ ? scoped_->handle_ : nullptr;
  }

private:
  std::shared_ptr<ScopedStructBase<Handle> > scoped_;
};

class ScopedSelectObject {
public:
  ScopedSelectObject(HDC hdc, HGDIOBJ obj) : hdc_(hdc), previous_(::SelectObject(hdc, obj)) {}
  ~ScopedSelectObject() {
    SelectObject(hdc_, previous_);
  }

private:
  ScopedSelectObject& operator=(const ScopedSelectObject&);
  const HDC hdc_;
  const HGDIOBJ previous_;
};
