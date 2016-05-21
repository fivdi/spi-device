#include <linux/spi/spidev.h>
#include <nan.h>
#include "spidevice.h"

NAN_METHOD(Open) {
  info.GetReturnValue().Set(SpiDevice::Open(info));
}

NAN_METHOD(OpenSync) {
  info.GetReturnValue().Set(SpiDevice::OpenSync(info));
}

static void ExportInt(
  Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target,
  const char* name,
  int value
) {
  Nan::Set(target,
    Nan::New<v8::String>(name).ToLocalChecked(),
    Nan::New<v8::Integer>(value)
  );
}

NAN_MODULE_INIT(InitAll) {
  SpiDevice::Init(target);

  Nan::Export(target, "open", Open);
  Nan::Export(target, "openSync", OpenSync);

  ExportInt(target, "MODE0", SPI_MODE_0);
  ExportInt(target, "MODE1", SPI_MODE_1);
  ExportInt(target, "MODE2", SPI_MODE_2);
  ExportInt(target, "MODE3", SPI_MODE_3);
}

NODE_MODULE(spi, InitAll)

