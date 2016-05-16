#include <errno.h>
#include <nan.h>
#include "spidevice.h"
#include "open.h"
#include "util.h"


static int Open(uint32_t busNumber, uint32_t deviceNumber) {
  char file[100];
  snprintf(file, sizeof(file), "/dev/spidev%u.%u", busNumber, deviceNumber);
  return open(file, O_RDWR);
}


class OpenWorker : public SpiAsyncWorker {
public:
  OpenWorker(
    Nan::Callback *callback,
    SpiDevice *device,
    uint32_t busNumber,
    uint32_t deviceNumber
  ) : SpiAsyncWorker(callback),
    device_(device),
    busNumber_(busNumber),
    deviceNumber_(deviceNumber) {
  }

  ~OpenWorker() {}

  void Execute() {
    fd_ = Open(busNumber_, deviceNumber_);
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
};


void Open(SpiDevice *device, Nan::NAN_METHOD_ARGS_TYPE info) {
  if (info.Length() < 3 ||
      !info[0]->IsUint32() ||
      !info[1]->IsUint32() ||
      !info[2]->IsFunction()) {
    return Nan::ThrowError(
      Nan::ErrnoException(
        EINVAL,
        "open",
        "incorrect arguments passed to open(busNumber, deviceNumber, cb)"
      )
    );
  }

  uint32_t busNumber = info[0]->Uint32Value();
  uint32_t deviceNumber = info[1]->Uint32Value();
  Nan::Callback *callback = new Nan::Callback(info[2].As<v8::Function>());

  Nan::AsyncQueueWorker(
    new OpenWorker(callback, device, busNumber, deviceNumber)
  );
}


void OpenSync(SpiDevice *device, Nan::NAN_METHOD_ARGS_TYPE info) {
  if (info.Length() < 2 ||
      !info[0]->IsUint32() ||
      !info[1]->IsUint32()) {
    return Nan::ThrowError(
      Nan::ErrnoException(
        EINVAL,
        "openSync",
        "incorrect arguments passed to openSync(busNumber, deviceNumber)"
      )
    );
  }

  uint32_t busNumber = info[0]->Uint32Value();
  uint32_t deviceNumber = info[1]->Uint32Value();

  int fd = Open(busNumber, deviceNumber);

  if (fd == -1) {
    return Nan::ThrowError(Nan::ErrnoException(errno, "openSync", ""));
  }

  device->SetFd(fd);
}

