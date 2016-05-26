#include <errno.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <nan.h>
#include "spidevice.h"
#include "setoptions.h"
#include "util.h"


int SetOptions(int fd, SpiOptions &spiOptions) {
  if (spiOptions.setMode) {
    SpiDevice::LockOptionAccess();

    uint8_t currentMode;
    int ret = ioctl(fd, SPI_IOC_RD_MODE, &currentMode);
    if (ret != -1) {
      uint8_t nextMode = (currentMode & spiOptions.modeMask) | spiOptions.mode;
      ret = ioctl(fd, SPI_IOC_WR_MODE, &nextMode);
    }

    SpiDevice::UnlockOptionAccess();

    if (ret == -1) {
      return -1;
    }
  }

  if (spiOptions.setBitsPerWord) {
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spiOptions.bitsPerWord) == -1) {
      return -1;
    }
  }

  if (spiOptions.setMaxSpeedHz) {
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spiOptions.maxSpeedHz) == -1) {
      return -1;
    }
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


int32_t ToSpiOptions(
  v8::Local<v8::Object> &jsOptions,
  SpiOptions &spiOptions
) {
  // mode

  v8::Local<v8::Value> mode = Nan::Get(jsOptions,
    Nan::New<v8::String>("mode").ToLocalChecked()).ToLocalChecked();

  if (mode->IsUndefined()) {
    // No mode defined, nothing to do.
  } else if (!mode->IsUint32()) {
    Nan::ThrowError(
      Nan::ErrnoException(
        EINVAL,
        "toSpiOptions",
        "option mode must be an unsigned integer"
      )
    );
    return -1;
  } else {
    uint32_t spiMode = mode->Uint32Value();

    if (spiMode > SPI_MODE_3) {
      Nan::ThrowError(
        Nan::ErrnoException(
          EINVAL,
          "toSpiOptions",
          "option mode is not a valid MODE"
        )
      );
      return -1;
    }

    spiOptions.mode |= spiMode;
    spiOptions.modeMask &= ~SPI_MODE_3;
    spiOptions.setMode = true;
  }

  // chipSelectHigh

  v8::Local<v8::Value> chipSelectHigh = Nan::Get(jsOptions,
    Nan::New<v8::String>("chipSelectHigh").ToLocalChecked()).ToLocalChecked();

  if (chipSelectHigh->IsUndefined()) {
    // No chipSelectHigh defined, nothing to do.
  } else if (!chipSelectHigh->IsBoolean()) {
    Nan::ThrowError(
      Nan::ErrnoException(
        EINVAL,
        "toSpiOptions",
        "option chipSelectHigh must be a boolean"
      )
    );
    return -1;
  } else {
    uint8_t spiChipSelectHigh =
      chipSelectHigh->BooleanValue() ? SPI_CS_HIGH : 0;

    spiOptions.mode |= spiChipSelectHigh;
    spiOptions.modeMask &= ~SPI_CS_HIGH;
    spiOptions.setMode = true;
  }

  // lsbFirst

  v8::Local<v8::Value> lsbFirst = Nan::Get(jsOptions,
    Nan::New<v8::String>("lsbFirst").ToLocalChecked()).ToLocalChecked();

  if (lsbFirst->IsUndefined()) {
    // No lsbFirst defined, nothing to do.
  } else if (!lsbFirst->IsBoolean()) {
    Nan::ThrowError(
      Nan::ErrnoException(
        EINVAL,
        "toSpiOptions",
        "option lsbFirst must be a boolean"
      )
    );
    return -1;
  } else {
    uint8_t spiLsbFirst = lsbFirst->BooleanValue() ? SPI_LSB_FIRST : 0;

    spiOptions.mode |= spiLsbFirst;
    spiOptions.modeMask &= ~SPI_LSB_FIRST;
    spiOptions.setMode = true;
  }

  // threeWire

  v8::Local<v8::Value> threeWire = Nan::Get(jsOptions,
    Nan::New<v8::String>("threeWire").ToLocalChecked()).ToLocalChecked();

  if (threeWire->IsUndefined()) {
    // No threeWire defined, nothing to do.
  } else if (!threeWire->IsBoolean()) {
    Nan::ThrowError(
      Nan::ErrnoException(
        EINVAL,
        "toSpiOptions",
        "option threeWire must be a boolean"
      )
    );
    return -1;
  } else {
    uint8_t spiThreeWire = threeWire->BooleanValue() ? SPI_3WIRE : 0;

    spiOptions.mode |= spiThreeWire;
    spiOptions.modeMask &= ~SPI_3WIRE;
    spiOptions.setMode = true;
  }

  // loopback

  v8::Local<v8::Value> loopback = Nan::Get(jsOptions,
    Nan::New<v8::String>("loopback").ToLocalChecked()).ToLocalChecked();

  if (loopback->IsUndefined()) {
    // No loopback defined, nothing to do.
  } else if (!loopback->IsBoolean()) {
    Nan::ThrowError(
      Nan::ErrnoException(
        EINVAL,
        "toSpiOptions",
        "option loopback must be a boolean"
      )
    );
    return -1;
  } else {
    uint8_t spiLoopback = loopback->BooleanValue() ? SPI_LOOP : 0;

    spiOptions.mode |= spiLoopback;
    spiOptions.modeMask &= ~SPI_LOOP;
    spiOptions.setMode = true;
  }

  // noChipSelect

  v8::Local<v8::Value> noChipSelect = Nan::Get(jsOptions,
    Nan::New<v8::String>("noChipSelect").ToLocalChecked()).ToLocalChecked();

  if (noChipSelect->IsUndefined()) {
    // No noChipSelect defined, nothing to do.
  } else if (!noChipSelect->IsBoolean()) {
    Nan::ThrowError(
      Nan::ErrnoException(
        EINVAL,
        "toSpiOptions",
        "option noChipSelect must be a boolean"
      )
    );
    return -1;
  } else {
    uint8_t spiNoChipSelect = noChipSelect->BooleanValue() ? SPI_NO_CS : 0;

    spiOptions.mode |= spiNoChipSelect;
    spiOptions.modeMask &= ~SPI_NO_CS;
    spiOptions.setMode = true;
  }

  // ready

  v8::Local<v8::Value> ready = Nan::Get(jsOptions,
    Nan::New<v8::String>("ready").ToLocalChecked()).ToLocalChecked();

  if (ready->IsUndefined()) {
    // No ready defined, nothing to do.
  } else if (!ready->IsBoolean()) {
    Nan::ThrowError(
      Nan::ErrnoException(
        EINVAL,
        "toSpiOptions",
        "option ready must be a boolean"
      )
    );
    return -1;
  } else {
    uint8_t spiReady = ready->BooleanValue() ? SPI_READY : 0;

    spiOptions.mode |= spiReady;
    spiOptions.modeMask &= ~SPI_READY;
    spiOptions.setMode = true;
  }

  // bitsPerWord

  v8::Local<v8::Value> bitsPerWord = Nan::Get(jsOptions,
    Nan::New<v8::String>("bitsPerWord").ToLocalChecked()).ToLocalChecked();

  if (bitsPerWord->IsUndefined()) {
    // No bitsPerWord defined, nothing to do.
  } else if (!bitsPerWord->IsUint32()) {
    Nan::ThrowError(
      Nan::ErrnoException(
        EINVAL,
        "toSpiOptions",
        "option bitsPerWord must be an unsigned integer"
      )
    );
    return -1;
  } else {
    uint32_t bits = bitsPerWord->Uint32Value();

    if (bits > 255) {
      Nan::ThrowError(
        Nan::ErrnoException(
          EINVAL,
          "toSpiOptions",
          "option bitsPerWord must be less than 256"
        )
      );
      return -1;
    }

    spiOptions.bitsPerWord = bits;
    spiOptions.setBitsPerWord = true;
  }

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
        "option maxSpeedHz must be an unsigned integer"
      )
    );
    return -1;
  } else {
    spiOptions.maxSpeedHz = maxSpeedHz->Uint32Value();
    spiOptions.setMaxSpeedHz = true;
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

