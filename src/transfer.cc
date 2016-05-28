#include <errno.h>
#include <nan.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "spidevice.h"
#include "transfer.h"
#include "util.h"


static int Transfer(
  int fd,
  spi_ioc_transfer *spiTransfers,
  uint32_t transferCount
) {
  int totalLen = 0;

  for (uint32_t i = 0; i != transferCount; ++i) {
    totalLen += spiTransfers[i].len;
  }

  int ret = ioctl(fd, SPI_IOC_MESSAGE(transferCount), spiTransfers);

  if (ret != -1 && ret != totalLen) {
    errno = EINVAL;
    ret = -1;
  }

  return ret;
}


class TransferWorker : public SpiAsyncWorker {
public:
  TransferWorker(
    Nan::Callback *callback,
    int fd,
    v8::Local<v8::Array> &message,
    spi_ioc_transfer *spiTransfers,
    uint32_t transferCount
  ) : SpiAsyncWorker(callback),
    fd_(fd),
    spiTransfers_(spiTransfers),
    transferCount_(transferCount) {
    SaveToPersistent("message", message);
  }

  ~TransferWorker() {
  }

  void Execute() {
    int ret = Transfer(fd_, spiTransfers_, transferCount_);
    free(spiTransfers_);
    if (ret == -1) {
      SetErrorNo(errno);
      SetErrorSyscall("transfer");
    }
  }

  void HandleOKCallback() {
    Nan::HandleScope scope;

    v8::Local<v8::Value> message = GetFromPersistent("message");

    v8::Local<v8::Value> argv[] = {
      Nan::Null(),
      message
    };

    callback->Call(2, argv);
  }

private:
  int fd_;
  spi_ioc_transfer *spiTransfers_;
  uint32_t transferCount_;
};


static int32_t ToSpiTransfers(
  v8::Local<v8::Array> &message,
  spi_ioc_transfer *spiTransfers
) {
  for (unsigned i = 0; i < message->Length(); ++i) {
    // Transfer

    v8::Local<v8::Value> transfer = message->Get(i);

    if (!transfer->IsObject()) {
      Nan::ThrowError(
        Nan::ErrnoException(
          EINVAL, "toSpiTransfers", "a transfer must be an object"
        )
      );
      return -1;
    }

    v8::Local<v8::Object> msg = v8::Local<v8::Object>::Cast(transfer);

    // byteLength

    v8::Local<v8::Value> byteLength =
      Nan::Get(msg, Nan::New<v8::String>("byteLength").ToLocalChecked()).
      ToLocalChecked();

    if (byteLength->IsUndefined()) {
      Nan::ThrowError(
        Nan::ErrnoException(
          EINVAL, "toSpiTransfers", "transfer byteLength not specified"
        )
      );
      return -1;
    } else if (!byteLength->IsUint32()) {
      Nan::ThrowError(
        Nan::ErrnoException(
          EINVAL,
          "toSpiTransfers",
          "transfer byteLength must be an unsigned integer"
        )
      );
      return -1;
    }

    uint32_t length = byteLength->Uint32Value();
    spiTransfers[i].len = length;

    // sendBuffer

    v8::Local<v8::Value> sendBuffer =
      Nan::Get(msg, Nan::New<v8::String>("sendBuffer").ToLocalChecked()).
      ToLocalChecked();

    if (sendBuffer->IsNull() || sendBuffer->IsUndefined()) {
      // No sendBuffer so tx_buf must be NULL. This is already the case.
    } else if (node::Buffer::HasInstance(sendBuffer)) {
      if (node::Buffer::Length(sendBuffer) < length) {
        Nan::ThrowError(
          Nan::ErrnoException(
            EINVAL,
            "toSpiTransfers",
            "transfer sendBuffer contains less than byteLength bytes"
          )
        );
        return -1;
      }
      spiTransfers[i].tx_buf = (__u64) node::Buffer::Data(sendBuffer);
    } else {
      Nan::ThrowError(
        Nan::ErrnoException(
          EINVAL,
          "toSpiTransfers",
          "transfer sendBuffer must be null, undefined, or a Buffer object"
        )
      );
      return -1;
    }

    // receiveBuffer

    v8::Local<v8::Value> receiveBuffer =
      Nan::Get(msg, Nan::New<v8::String>("receiveBuffer").ToLocalChecked()).
      ToLocalChecked();

    if (receiveBuffer->IsNull() || receiveBuffer->IsUndefined()) {
      // No receiveBuffer so rx_buf must be NULL. This is already the case.
    } else if (node::Buffer::HasInstance(receiveBuffer)) {
      if (node::Buffer::Length(receiveBuffer) < length) {
        Nan::ThrowError(
          Nan::ErrnoException(
            EINVAL,
            "toSpiTransfers",
            "transfer receiveBuffer contains less than byteLength bytes"
          )
        );
        return -1;
      }
      spiTransfers[i].rx_buf = (__u64) node::Buffer::Data(receiveBuffer);
    } else {
      Nan::ThrowError(
        Nan::ErrnoException(
          EINVAL,
          "toSpiTransfers",
          "transfer receiveBuffer must be null, undefined, or a Buffer object"
        )
      );
      return -1;
    }

    // sendBuffer and receiveBuffer

    if ((sendBuffer->IsNull() || sendBuffer->IsUndefined()) &&
        (receiveBuffer->IsNull() || receiveBuffer->IsUndefined())) {
      Nan::ThrowError(
        Nan::ErrnoException(
          EINVAL,
          "toSpiTransfers",
          "transfer contains neither a sendBuffer nor a receiveBuffer"
        )
      );
      return -1;
    }

    // speedHz

    v8::Local<v8::Value> speedHz = Nan::Get(msg,
      Nan::New<v8::String>("speedHz").ToLocalChecked()).ToLocalChecked();

    if (speedHz->IsUndefined()) {
      // No speedHz defined, nothing to do.
    } else if (!speedHz->IsUint32()) {
      Nan::ThrowError(
        Nan::ErrnoException(
          EINVAL,
          "toSpiTransfers",
          "transfer speedHz must be an unsigned integer"
        )
      );
      return -1;
    } else {
      spiTransfers[i].speed_hz = speedHz->Uint32Value();
    }

    // microSecondDelay

    v8::Local<v8::Value> microSecondDelay = Nan::Get(msg,
      Nan::New<v8::String>("microSecondDelay").ToLocalChecked()).ToLocalChecked();

    if (microSecondDelay->IsUndefined()) {
      // No microSecondDelay defined, nothing to do.
    } else if (!microSecondDelay->IsUint32()) {
      Nan::ThrowError(
        Nan::ErrnoException(
          EINVAL,
          "toSpiTransfers",
          "transfer microSecondDelay must be an unsigned integer"
        )
      );
      return -1;
    } else {
      uint32_t delay = microSecondDelay->Uint32Value();

      if (delay >= 65536) {
        Nan::ThrowError(
          Nan::ErrnoException(
            EINVAL,
            "toSpiTransfers",
            "transfer microSecondDelay must be less than 65536"
          )
        );
        return -1;
      }

      spiTransfers[i].delay_usecs = delay;
    }

    // bitsPerWord

    v8::Local<v8::Value> bitsPerWord = Nan::Get(msg,
      Nan::New<v8::String>("bitsPerWord").ToLocalChecked()).ToLocalChecked();

    if (bitsPerWord->IsUndefined()) {
      // No bitsPerWord defined, nothing to do.
    } else if (!bitsPerWord->IsUint32()) {
      Nan::ThrowError(
        Nan::ErrnoException(
          EINVAL,
          "toSpiTransfers",
          "transfer bitsPerWord must be an unsigned integer"
        )
      );
      return -1;
    } else {
      uint32_t bits = bitsPerWord->Uint32Value();

      if (bits >= 256) {
        Nan::ThrowError(
          Nan::ErrnoException(
            EINVAL,
            "toSpiTransfers",
            "transfer bitsPerWord must be less than 256"
          )
        );
        return -1;
      }

      spiTransfers[i].bits_per_word = bits;
    }

    // chipSelectChange

    v8::Local<v8::Value> chipSelectChange =
      Nan::Get(msg, Nan::New<v8::String>("chipSelectChange").ToLocalChecked()).
      ToLocalChecked();

    if (chipSelectChange->IsUndefined()) {
      // No chipSelectChange defined, nothing to do.
    } else if (!chipSelectChange->IsBoolean()) {
      Nan::ThrowError(
        Nan::ErrnoException(
          EINVAL,
          "toSpiTransfers",
          "transfer chipSelectChange must be a boolean"
        )
      );
      return -1;
    } else {
      spiTransfers[i].cs_change = chipSelectChange->BooleanValue() ? 1 : 0;
    }
  }

  return 0;
}


void Transfer(Nan::NAN_METHOD_ARGS_TYPE info) {
  SpiDevice *device = Nan::ObjectWrap::Unwrap<SpiDevice>(info.This());
  int fd = device->Fd();

  if (fd == -1) {
    return Nan::ThrowError(
      Nan::ErrnoException(
        EPERM, "transfer", "device closed, operation not permitted"
      )
    );
  }

  if (info.Length() < 2 ||
      !info[0]->IsArray() ||
      !info[1]->IsFunction()) {
    return Nan::ThrowError(
      Nan::ErrnoException(
        EINVAL,
        "transfer",
        "incorrect arguments passed to transfer(message, cb)"
      )
    );
  }

  v8::Local<v8::Array> message = info[0].As<v8::Array>();
  Nan::Callback *callback = new Nan::Callback(info[1].As<v8::Function>());

  spi_ioc_transfer *spiTransfers = (spi_ioc_transfer *)
    malloc(message->Length() * sizeof(spi_ioc_transfer));
  memset(spiTransfers, 0, message->Length() * sizeof(spi_ioc_transfer));

  if (ToSpiTransfers(message, spiTransfers) == -1) {
    free(spiTransfers);
    return;
  }

  Nan::AsyncQueueWorker(new TransferWorker(
    callback,
    fd,
    message,
    spiTransfers,
    message->Length()
  ));

  info.GetReturnValue().Set(info.This());
}


void TransferSync(Nan::NAN_METHOD_ARGS_TYPE info) {
  SpiDevice *device = Nan::ObjectWrap::Unwrap<SpiDevice>(info.This());
  int fd = device->Fd();

  if (fd == -1) {
    return Nan::ThrowError(
      Nan::ErrnoException(
        EPERM, "transferSync", "device closed, operation not permitted"
      )
    );
  }

  if (info.Length() < 1 || !info[0]->IsArray()) {
    return Nan::ThrowError(
      Nan::ErrnoException(
        EINVAL,
        "transfer",
        "incorrect arguments passed to transferSync(message)"
      )
    );
  }

  v8::Local<v8::Array> message = info[0].As<v8::Array>();

  spi_ioc_transfer *spiTransfers = (spi_ioc_transfer *)
    malloc(message->Length() * sizeof(spi_ioc_transfer));
  memset(spiTransfers, 0, message->Length() * sizeof(spi_ioc_transfer));

  if (ToSpiTransfers(message, spiTransfers) == 0) {
    if (Transfer(fd, spiTransfers, message->Length()) == -1) {
      Nan::ThrowError(Nan::ErrnoException(errno, "transferSync", ""));
    }
  }

  free(spiTransfers);

  info.GetReturnValue().Set(info.This());
}

