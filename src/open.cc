#include <errno.h>
#include <nan.h>
#include "spidevice.h"
#include "setoptions.h"
#include "open.h"
#include "util.h"


static int Open(
  uint32_t busNumber,
  uint32_t deviceNumber,
  SpiOptions &spiOptions
) {
  char file[100];

  snprintf(file, sizeof(file), "/dev/spidev%u.%u", busNumber, deviceNumber);

  int fd = open(file, O_RDWR);
  if (fd == -1) {
    return -1;
  }

  if (SetOptions(fd, spiOptions) == -1) {
    return -1;
  }

  return fd;
}


static void setDefaultOpenOptions(SpiOptions &spiOptions) {
  spiOptions.setMode = true;
  spiOptions.modeMask = 0;
  spiOptions.mode = 0;

  spiOptions.setBitsPerWord = true;
  spiOptions.bitsPerWord = 8;
}


class OpenWorker : public SpiAsyncWorker {
public:
  OpenWorker(
    Nan::Callback *callback,
    SpiDevice *device,
    uint32_t busNumber,
    uint32_t deviceNumber,
    SpiOptions spiOptions
  ) : SpiAsyncWorker(callback),
    device_(device),
    busNumber_(busNumber),
    deviceNumber_(deviceNumber),
    spiOptions_(spiOptions) {
  }

  ~OpenWorker() {}

  void Execute() {
    fd_ = Open(busNumber_, deviceNumber_, spiOptions_);
    if (fd_ == -1) {
      SetErrorNo(errno);
      SetErrorSyscall("open");
    }
  }

  void HandleOKCallback() {
    Nan::HandleScope scope;

    v8::Local<v8::Value> argv[] = {
      Nan::Null()
    };

    device_->SetFd(fd_);

    callback->Call(1, argv);
  }

private:
  int fd_;
  SpiDevice *device_;
  uint32_t busNumber_;
  uint32_t deviceNumber_;
  SpiOptions spiOptions_;
};


void Open(SpiDevice *device, Nan::NAN_METHOD_ARGS_TYPE info) {
  if (info.Length() < 3 ||
      !info[0]->IsUint32() ||
      !info[1]->IsUint32() ||
      (info.Length() == 3 && !info[2]->IsFunction()) ||
      (info.Length() > 3 && !info[2]->IsObject() && !info[3]->IsFunction())) {
    return Nan::ThrowError(
      Nan::ErrnoException(
        EINVAL,
        "open",
        "incorrect arguments passed to "
        "open(busNumber, deviceNumber[, options], cb)"
      )
    );
  }

  uint32_t busNumber = info[0]->Uint32Value();
  uint32_t deviceNumber = info[1]->Uint32Value();
  Nan::Callback *callback;
  SpiOptions spiOptions;

  setDefaultOpenOptions(spiOptions);

  callback = new Nan::Callback(
    info[info.Length() == 3 ? 2 : 3].As<v8::Function>()
  );

  if (info.Length() > 3) {
    v8::Local<v8::Object> jsOptions = info[2].As<v8::Object>();
    if (ToSpiOptions(jsOptions, spiOptions) == -1) {
      return;
    }
  }

  Nan::AsyncQueueWorker(
    new OpenWorker(callback, device, busNumber, deviceNumber, spiOptions)
  );
}


void OpenSync(SpiDevice *device, Nan::NAN_METHOD_ARGS_TYPE info) {
  if (info.Length() < 2 ||
      !info[0]->IsUint32() ||
      !info[1]->IsUint32() ||
      (info.Length() > 2 && !info[2]->IsObject())) {
    return Nan::ThrowError(
      Nan::ErrnoException(
        EINVAL,
        "openSync",
        "incorrect arguments passed to "
        "openSync(busNumber, deviceNumber[, options])"
      )
    );
  }

  uint32_t busNumber = info[0]->Uint32Value();
  uint32_t deviceNumber = info[1]->Uint32Value();
  SpiOptions spiOptions;

  setDefaultOpenOptions(spiOptions);

  if (info.Length() > 2) {
    v8::Local<v8::Object> jsOptions = info[2].As<v8::Object>();
    if (ToSpiOptions(jsOptions, spiOptions) == -1) {
      return;
    }
  }

  int fd = Open(busNumber, deviceNumber, spiOptions);

  if (fd == -1) {
    return Nan::ThrowError(Nan::ErrnoException(errno, "openSync", ""));
  }

  device->SetFd(fd);
}

