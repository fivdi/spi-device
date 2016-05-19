#include <errno.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <nan.h>
#include "spidevice.h"
#include "getoptions.h"
#include "util.h"


class SpiOptions {
public:
  uint8_t mode;
  uint8_t bitsPerWord;
  uint32_t maxSpeedHz;
};


static int GetOptions(int fd, SpiOptions &spiOptions) {
  if (ioctl(fd, SPI_IOC_RD_MODE, &spiOptions.mode) == -1 ||
      ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &spiOptions.bitsPerWord) == -1 ||
      ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &spiOptions.maxSpeedHz) == -1) {
    return -1;
  }

  return 0;
}


static void ToJsOptions(v8::Local<v8::Object> &jsOptions,
  SpiOptions &spiOptions
) {
  jsOptions->Set(
    Nan::New("mode").ToLocalChecked(),
    Nan::New<v8::Uint32>(spiOptions.mode & (SPI_CPOL | SPI_CPHA))
  );
  jsOptions->Set(
    Nan::New("chipSelectHigh").ToLocalChecked(),
    Nan::New<v8::Boolean>(spiOptions.mode & SPI_CS_HIGH)
  );
  jsOptions->Set(
    Nan::New("lsbFirst").ToLocalChecked(),
    Nan::New<v8::Boolean>(spiOptions.mode & SPI_LSB_FIRST)
  );
  jsOptions->Set(
    Nan::New("threeWire").ToLocalChecked(),
    Nan::New<v8::Boolean>(spiOptions.mode & SPI_3WIRE)
  );
  jsOptions->Set(
    Nan::New("loopback").ToLocalChecked(),
    Nan::New<v8::Boolean>(spiOptions.mode & SPI_LOOP)
  );
  jsOptions->Set(
    Nan::New("noChipSelect").ToLocalChecked(),
    Nan::New<v8::Boolean>(spiOptions.mode & SPI_NO_CS)
  );
  jsOptions->Set(
    Nan::New("ready").ToLocalChecked(),
    Nan::New<v8::Boolean>(spiOptions.mode & SPI_READY)
  );
  jsOptions->Set(
    Nan::New("bitsPerWord").ToLocalChecked(),
    Nan::New<v8::Uint32>(spiOptions.bitsPerWord)
  );
  jsOptions->Set(
    Nan::New("maxSpeedHz").ToLocalChecked(),
    Nan::New<v8::Uint32>(spiOptions.maxSpeedHz)
  );
}


class GetOptionsWorker : public SpiAsyncWorker {
public:
  GetOptionsWorker(
    Nan::Callback *callback,
    int fd
  ) : SpiAsyncWorker(callback), fd_(fd) {
  }

  ~GetOptionsWorker() {}

  void Execute() {
    if (GetOptions(fd_, spiOptions_) == -1) {
      SetErrorNo(errno);
      SetErrorSyscall("getOptions");
    }
  }

  void HandleOKCallback() {
    Nan::HandleScope scope;

    v8::Local<v8::Object> jsOptions = Nan::New<v8::Object>();
    ToJsOptions(jsOptions, spiOptions_);

    v8::Local<v8::Value> argv[] = {
      Nan::Null(),
      jsOptions
    };

    callback->Call(2, argv);
  }

private:
  int fd_;
  SpiOptions spiOptions_;
};


void GetOptions(Nan::NAN_METHOD_ARGS_TYPE info) {
  SpiDevice *device = Nan::ObjectWrap::Unwrap<SpiDevice>(info.This());
  int fd = device->Fd();

  if (fd == -1) {
    return Nan::ThrowError(
      Nan::ErrnoException(
        EPERM, "getOptions", "device closed, operation not permitted"
      )
    );
  }

  if (info.Length() < 1 || !info[0]->IsFunction()) {
    return Nan::ThrowError(
      Nan::ErrnoException(
        EINVAL, "getOptions", "incorrect arguments passed to getOptions(cb)"
      )
    );
  }

  Nan::Callback *callback = new Nan::Callback(info[0].As<v8::Function>());

  Nan::AsyncQueueWorker(new GetOptionsWorker(callback, fd));

  info.GetReturnValue().Set(info.This());
}


void GetOptionsSync(Nan::NAN_METHOD_ARGS_TYPE info) {
  SpiDevice *device = Nan::ObjectWrap::Unwrap<SpiDevice>(info.This());
  int fd = device->Fd();

  if (fd == -1) {
    return Nan::ThrowError(
      Nan::ErrnoException(
        EPERM, "getOptionsSync", "device closed, operation not permitted"
      )
    );
  }

  SpiOptions spiOptions;

  if (GetOptions(fd, spiOptions) == -1) {
    return Nan::ThrowError(Nan::ErrnoException(errno, "getOptionsSync", ""));
  }

  v8::Local<v8::Object> jsOptions = Nan::New<v8::Object>();
  ToJsOptions(jsOptions, spiOptions);

  info.GetReturnValue().Set(jsOptions);
}

