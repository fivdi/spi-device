#include <nan.h>
#include "spidevice.h"

NAN_METHOD(Open) {
  info.GetReturnValue().Set(SpiDevice::Open(info));
}

NAN_METHOD(OpenSync) {
  info.GetReturnValue().Set(SpiDevice::OpenSync(info));
}

NAN_MODULE_INIT(InitAll) {
  SpiDevice::Init(target);

  Nan::Export(target, "open", Open);
  Nan::Export(target, "openSync", OpenSync);
}

NODE_MODULE(spi, InitAll)

