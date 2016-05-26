#ifndef SPIDEVICE_H
#define SPIDEVICE_H

class SpiDevice : public Nan::ObjectWrap {
 public:
  static NAN_MODULE_INIT(Init);

  static v8::Local<v8::Object> Open(Nan::NAN_METHOD_ARGS_TYPE info);
  static v8::Local<v8::Object> OpenSync(Nan::NAN_METHOD_ARGS_TYPE info);
  static void LockOptionAccess();
  static void UnlockOptionAccess();

  void SetFd(int fd) { fd_ = fd; }
  int Fd() const { return fd_; }

 private:
  SpiDevice();
  ~SpiDevice();

  static NAN_METHOD(New);
  static NAN_METHOD(Close);
  static NAN_METHOD(CloseSync);
  static NAN_METHOD(Transfer);
  static NAN_METHOD(TransferSync);
  static NAN_METHOD(GetOptions);
  static NAN_METHOD(GetOptionsSync);
  static NAN_METHOD(SetOptions);
  static NAN_METHOD(SetOptionsSync);

  static uv_mutex_t optionAccessLock;
  static Nan::Persistent<v8::Function> constructor;

  int fd_;
};

#endif

