#ifndef UTIL_H
#define UTIL_H

class SpiAsyncWorker : public Nan::AsyncWorker {
public:
  explicit SpiAsyncWorker(Nan::Callback *callback)
    : Nan::AsyncWorker(callback), errorNo_(0), errorSyscall_(0) {}

  virtual ~SpiAsyncWorker() {
    if (errorSyscall_) {
      delete[] errorSyscall_;
    }
  }

  virtual void WorkComplete() {
    Nan::HandleScope scope;

    if (errorNo_ == 0) {
      HandleOKCallback();
    } else {
      HandleErrorCallback();
    }

    delete callback;
    callback = 0;
  }

protected:
  virtual void HandleErrorCallback() {
    Nan::HandleScope scope;

    v8::Local<v8::Value> argv[1] = {
      Nan::ErrnoException(ErrorNo(), ErrorSyscall(), "")
    };

    callback->Call(1, argv);
  }

  void SetErrorNo(int errorNo) {
    errorNo_ = errorNo;
  }

  int ErrorNo() {
    return errorNo_;
  }

  void SetErrorSyscall(const char *errorSyscall) {
    if (errorSyscall_) {
      delete[] errorSyscall_;
    }

    size_t size = strlen(errorSyscall) + 1;
    errorSyscall_ = new char[size];
    memcpy(errorSyscall_, errorSyscall, size);
  }

  const char *ErrorSyscall() {
    return errorSyscall_;
  }

private:
  int errorNo_;
  char *errorSyscall_;
};

#endif // UTIL_H

