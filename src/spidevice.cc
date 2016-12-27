#include <nan.h>
#include "spidevice.h"
#include "open.h"
#include "close.h"
#include "transfer.h"
#include "getoptions.h"
#include "setoptions.h"


uv_mutex_t SpiDevice::optionAccessLock;
Nan::Persistent<v8::Function> SpiDevice::constructor;


SpiDevice::SpiDevice() : fd_(-1) {};
SpiDevice::~SpiDevice() {};


NAN_MODULE_INIT(SpiDevice::Init) {
  Nan::HandleScope scope;

  // Constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("SpiDevice").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "close", Close);
  Nan::SetPrototypeMethod(tpl, "closeSync", CloseSync);
  Nan::SetPrototypeMethod(tpl, "transfer", Transfer);
  Nan::SetPrototypeMethod(tpl, "transferSync", TransferSync);
  Nan::SetPrototypeMethod(tpl, "getOptions", GetOptions);
  Nan::SetPrototypeMethod(tpl, "getOptionsSync", GetOptionsSync);
  Nan::SetPrototypeMethod(tpl, "setOptions", SetOptions);
  Nan::SetPrototypeMethod(tpl, "setOptionsSync", SetOptionsSync);

  constructor.Reset(tpl->GetFunction());

  uv_mutex_init(&optionAccessLock);
}


NAN_METHOD(SpiDevice::New) {
  SpiDevice* obj = new SpiDevice();
  obj->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}


v8::Local<v8::Object> SpiDevice::Open(Nan::NAN_METHOD_ARGS_TYPE info) {
  Nan::EscapableHandleScope scope;

  v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
  Nan::MaybeLocal<v8::Object> maybeInstance = Nan::NewInstance(cons, 0, NULL);
  v8::Local<v8::Object> instance;

  if (maybeInstance.IsEmpty()) {
    Nan::ThrowError("Could not create new SpiDevice instance");
  } else {
    instance = maybeInstance.ToLocalChecked();

    SpiDevice *device = Nan::ObjectWrap::Unwrap<SpiDevice>(instance);

    // TODO what if Open does return Nan::ThrowError?
    // Looks like Open should have a return code that's evalueated here
    ::Open(device, info);
  }

  return scope.Escape(instance);
}


v8::Local<v8::Object> SpiDevice::OpenSync(Nan::NAN_METHOD_ARGS_TYPE info) {
  Nan::EscapableHandleScope scope;

  v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
  Nan::MaybeLocal<v8::Object> maybeInstance = Nan::NewInstance(cons, 0, NULL);
  v8::Local<v8::Object> instance;

  if (maybeInstance.IsEmpty()) {
    Nan::ThrowError("Could not create new SpiDevice instance");
  } else {
    instance = maybeInstance.ToLocalChecked();

    SpiDevice *device = Nan::ObjectWrap::Unwrap<SpiDevice>(instance);

    // TODO what if OpenSync does return Nan::ThrowError?
    // Looks like OpenSync should have a return code that's evalueated here
    ::OpenSync(device, info);
  }

  return scope.Escape(instance);
}


void SpiDevice::LockOptionAccess() {
  uv_mutex_lock(&SpiDevice::optionAccessLock);
}


void SpiDevice::UnlockOptionAccess() {
  uv_mutex_unlock(&SpiDevice::optionAccessLock);
}


NAN_METHOD(SpiDevice::Close) {
  ::Close(info);
}


NAN_METHOD(SpiDevice::CloseSync) {
  ::CloseSync(info);
}


NAN_METHOD(SpiDevice::Transfer) {
  ::Transfer(info);
}


NAN_METHOD(SpiDevice::TransferSync) {
  ::TransferSync(info);
}


NAN_METHOD(SpiDevice::GetOptions) {
  ::GetOptions(info);
}


NAN_METHOD(SpiDevice::GetOptionsSync) {
  ::GetOptionsSync(info);
}


NAN_METHOD(SpiDevice::SetOptions) {
  ::SetOptions(info);
}


NAN_METHOD(SpiDevice::SetOptionsSync) {
  ::SetOptionsSync(info);
}

