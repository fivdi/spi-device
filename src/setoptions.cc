#include <errno.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <nan.h>
#include "spidevice.h"
#include "setoptions.h"
#include "util.h"


class SpiOptions {
public:
  SpiOptions() :
    setMode(false),
    setBitsPerWord(false),
    setMaxSpeed(false),
    modeMask(0xff),
    mode(0),
    bitsPerWord(0),
    maxSpeedHz(0) {
  }

  bool setMode;
  bool setBitsPerWord;
  bool setMaxSpeed;

  uint8_t modeMask;

  uint8_t mode;
  uint8_t bitsPerWord;
  uint32_t maxSpeedHz;
};


static int SetOptions(int fd, SpiOptions &spiOptions) {
  if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spiOptions.maxSpeedHz) == -1) {
    return -1;
  }

  return 0;
}


class SetOptionsWorker : public SpiAsyncWorker {
public:
  SetOptionsWorker(
    Nan::Callback *callback,
    int fd,
    SpiOptions spiOptions
  ) : SpiAsyncWorker(callback), fd_(fd), spiOptions_(spiOptions) {
  }

  ~SetOptionsWorker() {}

  void Execute() {
    if (SetOptions(fd_, spiOptions_) == -1) {
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
  SpiOptions spiOptions_;
};


static int32_t ToSpiOptions(
  v8::Local<v8::Object> &jsOptions,
  SpiOptions &spiOptions
) {
  // maxSpeedHz

  v8::Local<v8::Value> maxSpeedHz = Nan::Get(jsOptions,
    Nan::New<v8::String>("maxSpeedHz").ToLocalChecked()).ToLocalChecked();

  if (maxSpeedHz->IsUndefined()) {
    // No maxSpeedHz defined, nothing to do.
  } else if (!maxSpeedHz->IsUint32()) {
    Nan::ThrowError(
      Nan::ErrnoException(
        EINVAL,
        "toSpiOptions",
        "option maxSpeedHz should be an unsigned integer"
      )
    );
    return -1;
  } else {
    spiOptions.maxSpeedHz = maxSpeedHz->Uint32Value();
    spiOptions.setMaxSpeed = true;
  }

  return 0;
}


void SetOptions(Nan::NAN_METHOD_ARGS_TYPE info) {
  SpiDevice *device = Nan::ObjectWrap::Unwrap<SpiDevice>(info.This());
  int fd = device->Fd();

  if (fd == -1) {
    return Nan::ThrowError(
      Nan::ErrnoException(
        EPERM, "setOptions", "device closed, operation not permitted"
      )
    );
  }

  if (info.Length() < 2 || !info[0]->IsObject() || !info[1]->IsFunction()) {
    return Nan::ThrowError(
      Nan::ErrnoException(
        EINVAL,
        "setOptions",
        "incorrect arguments passed to setOptions(options, cb)"
      )
    );
  }

  v8::Local<v8::Object> jsOptions = info[0].As<v8::Object>();
  Nan::Callback *callback = new Nan::Callback(info[1].As<v8::Function>());

  SpiOptions spiOptions;
  if (ToSpiOptions(jsOptions, spiOptions) == -1) {
    return;
  }

  Nan::AsyncQueueWorker(new SetOptionsWorker(callback, fd, spiOptions));

  info.GetReturnValue().Set(info.This());
}


void SetOptionsSync(Nan::NAN_METHOD_ARGS_TYPE info) {
  SpiDevice *device = Nan::ObjectWrap::Unwrap<SpiDevice>(info.This());
  int fd = device->Fd();

  if (fd == -1) {
    return Nan::ThrowError(
      Nan::ErrnoException(
        EPERM, "setOptionsSync", "device closed, operation not permitted"
      )
    );
  }

  if (info.Length() < 1 || !info[0]->IsObject()) {
    return Nan::ThrowError(
      Nan::ErrnoException(
        EINVAL,
        "setOptionsSync",
        "incorrect arguments passed to setOptionsSync(options)"
      )
    );
  }

  v8::Local<v8::Object> jsOptions = info[0].As<v8::Object>();

  SpiOptions spiOptions;
  if (ToSpiOptions(jsOptions, spiOptions) == -1) {
    return;
  }

  if (SetOptions(fd, spiOptions) == -1) {
    return Nan::ThrowError(Nan::ErrnoException(errno, "setOptionsSync", ""));
  }

  info.GetReturnValue().Set(info.This());
}

