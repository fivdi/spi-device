#ifndef SETOPTIONS_H
#define SETOPTIONS_H


class SpiOptions {
public:
  SpiOptions() :
    setMode(false),
    setBitsPerWord(false),
    setMaxSpeedHz(false),
    modeMask(0xff),
    mode(0),
    bitsPerWord(0),
    maxSpeedHz(0) {
  }

  bool setMode;
  bool setBitsPerWord;
  bool setMaxSpeedHz;

  uint8_t modeMask;

  uint8_t mode;
  uint8_t bitsPerWord;
  uint32_t maxSpeedHz;
};


int SetOptions(int fd, SpiOptions &spiOptions);
int32_t ToSpiOptions(
  v8::Local<v8::Object> &jsOptions,
  SpiOptions &spiOptions
);


void SetOptions(Nan::NAN_METHOD_ARGS_TYPE info);
void SetOptionsSync(Nan::NAN_METHOD_ARGS_TYPE info);
#endif

