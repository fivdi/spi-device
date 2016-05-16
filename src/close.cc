#include <errno.h>
#include <unistd.h>
#include <nan.h>
#include "spidevice.h"
#include "close.h"
#include "util.h"


static int Close(SpiDevice *device) {
  int fd = device->Fd();
  device->SetFd(-1);
  return close(fd);
}


class CloseWorker : public SpiAsyncWorker {
public:
  CloseWorker(
    Nan::Callback *callback,
    SpiDevice *device
  ) : SpiAsyncWorker(callback), device_(device) {
  }

  ~CloseWorker() {}

  void Execute() {
    if (Close(device_) == -1) {
      SetErrorNo(errno);
      SetErrorSyscall("close");
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
  SpiDevice *device_;
};


void Close(Nan::NAN_METHOD_ARGS_TYPE info) {
  SpiDevice *device = Nan::ObjectWrap::Unwrap<SpiDevice>(info.This());

  if (device->Fd() == -1) {
    return Nan::ThrowError(
      Nan::ErrnoException(
        EPERM, "close", "device closed, operation not permitted"
      )
    );
  }

  if (info.Length() < 1 || !info[0]->IsFunction()) {
    return Nan::ThrowError(
      Nan::ErrnoException(
        EINVAL, "close", "incorrect arguments passed to close(cb)"
      )
    );
  }

  Nan::Callback *callback = new Nan::Callback(info[0].As<v8::Function>());

  Nan::AsyncQueueWorker(new CloseWorker(callback, device));

  info.GetReturnValue().SetNull();
}


void CloseSync(Nan::NAN_METHOD_ARGS_TYPE info) {
  SpiDevice *device = Nan::ObjectWrap::Unwrap<SpiDevice>(info.This());

  if (device->Fd() == -1) {
    return Nan::ThrowError(
      Nan::ErrnoException(
        EPERM, "closeSync", "device closed, operation not permitted"
      )
    );
  }

  if (Close(device) == -1) {
    return Nan::ThrowError(Nan::ErrnoException(errno, "closeSync", ""));
  }

  info.GetReturnValue().SetNull();
}

