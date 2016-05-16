#include <errno.h>
#include <nan.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "spidevice.h"
#include "transfer.h"
#include "util.h"


// TODO
// - Make sure it works when the messages array is empty


static int Transfer(
  int fd,
  spi_ioc_transfer *messages,
  uint32_t messageCount) {
  return ioctl(fd, SPI_IOC_MESSAGE(messageCount), messages);
}


class TransferWorker : public SpiAsyncWorker {
public:
  TransferWorker(
    Nan::Callback *callback,
    int fd,
    v8::Local<v8::Value> messages,
    spi_ioc_transfer *spiMessages,
    uint32_t messageCount
  ) : SpiAsyncWorker(callback),
    fd_(fd),
    spiMessages_(spiMessages),
    messageCount_(messageCount) {
    SaveToPersistent("messages", messages);
  }

  ~TransferWorker() {
  }

  void Execute() {
    int ret = Transfer(fd_, spiMessages_, messageCount_);
    free(spiMessages_);
    if (ret == -1) {
      SetErrorNo(errno);
      SetErrorSyscall("transfer");
    }
  }

  void HandleOKCallback() {
    Nan::HandleScope scope;

    v8::Local<v8::Value> messages = GetFromPersistent("messages");

    v8::Local<v8::Value> argv[] = {
      Nan::Null(),
      messages
    };

    callback->Call(2, argv);
  }

private:
  int fd_;
  spi_ioc_transfer *spiMessages_;
  uint32_t messageCount_;
};


static int32_t ToSpiMessages(
  v8::Local<v8::Array> messages,
  spi_ioc_transfer *spiMessages
) {
  for (unsigned i = 0; i < messages->Length(); ++i) {
    // Message

    v8::Local<v8::Value> message = messages->Get(i);

    if (!message->IsObject()) {
      Nan::ThrowError(Nan::ErrnoException(EINVAL, "toSpiMessages",
        "each message being transfered should be an object"));
      return -1;
    }

    v8::Local<v8::Object> msg = v8::Local<v8::Object>::Cast(message);

    // byteLength

    v8::Local<v8::Value> byteLength = Nan::Get(msg,
      Nan::New<v8::String>("byteLength").ToLocalChecked()).ToLocalChecked();

    if (byteLength->IsUndefined()) {
      Nan::ThrowError(Nan::ErrnoException(EINVAL, "toSpiMessages",
        "message byteLength not specified"));
      return -1;
    } else if (!byteLength->IsUint32()) {
      Nan::ThrowError(Nan::ErrnoException(EINVAL, "toSpiMessages",
        "message byteLength should be an unsigned integer"));
      return -1;
    }

    uint32_t length = byteLength->Uint32Value();
    spiMessages[i].len = length;

    // sendBuffer

    v8::Local<v8::Value> sendBuffer = Nan::Get(msg,
      Nan::New<v8::String>("sendBuffer").ToLocalChecked()).ToLocalChecked();

    if (sendBuffer->IsNull() || sendBuffer->IsUndefined()) {
      // No sendBuffer so tx_buf should be NULL. This is already the case.
    } else if (node::Buffer::HasInstance(sendBuffer)) {
      if (node::Buffer::Length(sendBuffer) < length) {
        Nan::ThrowError(Nan::ErrnoException(EINVAL, "toSpiMessages",
          "message sendBuffer contains less than byteLength bytes"));
        return -1;
      }
      spiMessages[i].tx_buf = (__u64) node::Buffer::Data(sendBuffer);
    } else {
      Nan::ThrowError(Nan::ErrnoException(EINVAL, "toSpiMessages",
        "message sendBuffer should be null, undefined, or a Buffer object"));
      return -1;
    }

    // receiveBuffer

    v8::Local<v8::Value> receiveBuffer = Nan::Get(msg,
      Nan::New<v8::String>("receiveBuffer").ToLocalChecked()).ToLocalChecked();

    if (receiveBuffer->IsNull() || receiveBuffer->IsUndefined()) {
      // No receiveBuffer so rx_buf should be NULL. This is already the case.
    } else if (node::Buffer::HasInstance(receiveBuffer)) {
      if (node::Buffer::Length(receiveBuffer) < length) {
        Nan::ThrowError(Nan::ErrnoException(EINVAL, "toSpiMessages",
          "message receiveBuffer contains less than byteLength bytes"));
        return -1;
      }
      spiMessages[i].rx_buf = (__u64) node::Buffer::Data(receiveBuffer);
    } else {
      Nan::ThrowError(Nan::ErrnoException(EINVAL, "toSpiMessages",
        "message receiveBuffer should be null, undefined, or a Buffer object"));
      return -1;
    }

    // sendBuffer and receiveBuffer

    if ((sendBuffer->IsNull() || sendBuffer->IsUndefined()) &&
        (receiveBuffer->IsNull() || receiveBuffer->IsUndefined())) {
      Nan::ThrowError(Nan::ErrnoException(EINVAL, "toSpiMessages",
        "message contains neither a sendBuffer nor a receiveBuffer"));
      return -1;
    }

    // speed

    v8::Local<v8::Value> speed = Nan::Get(msg,
      Nan::New<v8::String>("speed").ToLocalChecked()).ToLocalChecked();

    if (speed->IsUndefined()) {
      // No speed defined, nothing to do.
    } else if (!speed->IsUint32()) {
      Nan::ThrowError(Nan::ErrnoException(EINVAL, "toSpiMessages",
        "message speed should be an unsigned integer"));
      return -1;
    } else {
      spiMessages[i].speed_hz = speed->Uint32Value();
    }

    // chipSelectChange

    v8::Local<v8::Value> chipSelectChange = Nan::Get(msg,
      Nan::New<v8::String>("chipSelectChange").ToLocalChecked()).
      ToLocalChecked();

    if (chipSelectChange->IsUndefined()) {
      // No chipSelectChange defined, nothing to do.
    } else if (!chipSelectChange->IsBoolean()) {
      Nan::ThrowError(Nan::ErrnoException(EINVAL, "toSpiMessages",
        "message chipSelectChange should be a boolean"));
      return -1;
    } else {
      spiMessages[i].cs_change = chipSelectChange->BooleanValue() ? 1 : 0;
    }
  }

  return 0;
}


void Transfer(Nan::NAN_METHOD_ARGS_TYPE info) {
  SpiDevice *device = Nan::ObjectWrap::Unwrap<SpiDevice>(info.This());
  int fd = device->Fd();

  if (fd == -1) {
    return Nan::ThrowError(Nan::ErrnoException(EPERM, "transfer",
      "device closed, operation not permitted"));
  }

  if (info.Length() < 2 ||
      !info[0]->IsArray() ||
      !info[1]->IsFunction()) {
    return Nan::ThrowError(Nan::ErrnoException(EINVAL, "transfer",
      "incorrect arguments passed to transfer(messages, cb)"));
  }

  v8::Local<v8::Array> messages = info[0].As<v8::Array>();
  Nan::Callback *callback = new Nan::Callback(info[1].As<v8::Function>());

  spi_ioc_transfer *spiMessages = (spi_ioc_transfer *)
    malloc(messages->Length() * sizeof(spi_ioc_transfer));
  memset(spiMessages, 0, messages->Length() * sizeof(spi_ioc_transfer));

  if (ToSpiMessages(messages, spiMessages) == -1) {
    free(spiMessages);
    return;
  }

  Nan::AsyncQueueWorker(new TransferWorker(
    callback,
    fd,
    messages,
    spiMessages,
    messages->Length()
  ));

  info.GetReturnValue().Set(info.This());
}


void TransferSync(Nan::NAN_METHOD_ARGS_TYPE info) {
  SpiDevice *device = Nan::ObjectWrap::Unwrap<SpiDevice>(info.This());
  int fd = device->Fd();

  if (fd == -1) {
    return Nan::ThrowError(Nan::ErrnoException(EPERM, "transferSync",
      "device closed, operation not permitted"));
  }

  if (info.Length() < 1 || !info[0]->IsArray()) {
    return Nan::ThrowError(Nan::ErrnoException(EINVAL, "transfer",
      "incorrect arguments passed to transferSync(messages)"));
  }

  v8::Local<v8::Array> messages = info[0].As<v8::Array>();

  spi_ioc_transfer *spiMessages = (spi_ioc_transfer *)
    malloc(messages->Length() * sizeof(spi_ioc_transfer));
  memset(spiMessages, 0, messages->Length() * sizeof(spi_ioc_transfer));

  if (ToSpiMessages(messages, spiMessages) == 0) {
    if (Transfer(fd, spiMessages, messages->Length()) == -1) {
      Nan::ThrowError(Nan::ErrnoException(errno, "transferSync", ""));
    }
  }

  free(spiMessages);

  info.GetReturnValue().Set(info.This());
}

