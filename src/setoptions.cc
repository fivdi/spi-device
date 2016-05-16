#include <errno.h>
#include <unistd.h>
#include <nan.h>
#include "spidevice.h"
#include "setoptions.h"
#include "util.h"


static int SetOptions(int fd) {
  return 0;
}


class SetOptionsWorker : public SpiAsyncWorker {
public:
  SetOptionsWorker(
    Nan::Callback *callback,
    int fd
  ) : SpiAsyncWorker(callback), fd_(fd) {
  }

  ~SetOptionsWorker() {}

  void Execute() {
    if (SetOptions(fd_) == -1) {
      SetErrorNo(errno);
      SetErrorSyscall("setOptions");
    }
  }

  void HandleOKCallback() {
    Nan::HandleScope scope;

    v8::Local<v8::Value> argv[] = {
      Nan::Null()
    };

    callback->Call(1, argv);
  }

private:
  int fd_;
};


void SetOptions(Nan::NAN_METHOD_ARGS_TYPE info) {
  SpiDevice *device = Nan::ObjectWrap::Unwrap<SpiDevice>(info.This());
  int fd = device->Fd();

  if (fd == -1) {
    return Nan::ThrowError(Nan::ErrnoException(EPERM, "setOptions",
      "device closed, operation not permitted"));
  }

  if (info.Length() < 1 || !info[0]->IsFunction()) {
    return Nan::ThrowError(Nan::ErrnoException(EINVAL, "setOptions",
      "incorrect arguments passed to setOptions(cb)"));
  }

  Nan::Callback *callback = new Nan::Callback(info[0].As<v8::Function>());

  Nan::AsyncQueueWorker(new SetOptionsWorker(callback, fd));

  info.GetReturnValue().Set(info.This());
}


void SetOptionsSync(Nan::NAN_METHOD_ARGS_TYPE info) {
  SpiDevice *device = Nan::ObjectWrap::Unwrap<SpiDevice>(info.This());
  int fd = device->Fd();

  if (fd == -1) {
    return Nan::ThrowError(Nan::ErrnoException(EPERM, "setOptionsSync",
      "device closed, operation not permitted"));
  }

  if (SetOptions(fd) == -1) {
    return Nan::ThrowError(Nan::ErrnoException(errno, "setOptionsSync", ""));
  }

  info.GetReturnValue().Set(info.This());
}

