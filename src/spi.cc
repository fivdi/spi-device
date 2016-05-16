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

// Trick to speed up compilation.
// Normally cc files are listed in bindings.gyp and compiled individually.
// Including them here rather than compiling them individually reduces compile
// time.
/*#include "close.cc"
#include "open.cc"
#include "spidevice.cc"
#include "transfer.cc"*/

