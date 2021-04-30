[![Build Status](https://travis-ci.org/fivdi/spi-device.svg?branch=master)](https://travis-ci.org/fivdi/spi-device)
[![npm Version](http://img.shields.io/npm/v/spi-device.svg)](https://www.npmjs.com/package/spi-device)
[![Downloads Per Month](http://img.shields.io/npm/dm/spi-device.svg)](https://www.npmjs.com/package/spi-device)
[![Mentioned in Awesome Node.js](https://awesome.re/mentioned-badge.svg)](https://github.com/sindresorhus/awesome-nodejs)

# spi-device

SPI serial bus access with **Node.js** on Linux boards like the Raspberry
Pi or BeagleBone. All methods have asynchronous and synchronous forms.

spi-device supports Node.js versions 10, 12, 14, 15 and 16.

## Contents

 * [Installation](#installation)
 * [Usage](#usage)
 * [API Documentation](#api-documentation)

## Installation

```
npm install spi-device
```

## Usage

Determine the temperature using a TMP36 analog temperature sensor wired to
channel 5 on an MCP3008 SPI A/D converter.

<img src="https://raw.githubusercontent.com/fivdi/spi-device/master/example/pi-mcp3008-tmp36.png">

```js
const spi = require('spi-device');

// The MCP3008 is on bus 0 and it's device 0
const mcp3008 = spi.open(0, 0, err => {
  // An SPI message is an array of one or more read+write transfers
  const message = [{
    sendBuffer: Buffer.from([0x01, 0xd0, 0x00]), // Sent to read channel 5
    receiveBuffer: Buffer.alloc(3),              // Raw data read from channel 5
    byteLength: 3,
    speedHz: 20000 // Use a low bus speed to get a good reading from the TMP36
  }];

  if (err) throw err;

  mcp3008.transfer(message, (err, message) => {
    if (err) throw err;

    // Convert raw value from sensor to celcius and log to console
    const rawValue = ((message[0].receiveBuffer[1] & 0x03) << 8) +
      message[0].receiveBuffer[2];
    const voltage = rawValue * 3.3 / 1023;
    const celcius = (voltage - 0.5) * 100;

    console.log(celcius);
  });
});
```

spi-device enables low-level access to SPI devices. Often, high-level access
is required. When this is the case, high-level packages can be built using
spi-device. An example of such a package is
[mcp-spi-adc](https://github.com/fivdi/mcp-spi-adc) which provides a high-level
API for accessing an MCP3008 SPI A/D converter and will generally be more
useful than the low-level demonstration code shown above.

## API Documentation

All methods have asynchronous and synchronous forms.

The asynchronous form always take a completion callback as its last argument.
The arguments passed to the completion callback depend on the method, but the
first argument is always reserved for an exception. If the operation was
completed successfully, then the first argument will be null or undefined.

When using the synchronous form any exceptions are immediately thrown. You can
use try/catch to handle exceptions or allow them to bubble up. 

### Functions

- [open(busNumber, deviceNumber[, options], cb)](#openbusnumber-devicenumber-options-cb)
- [openSync(busNumber, deviceNumber[, options])](#opensyncbusnumber-devicenumber-options)

### Class SpiDevice

- [device.transfer(message, cb)](#devicetransfermessage-cb)
- [device.transferSync(message)](#devicetransfersyncmessage)
- [device.getOptions(cb)](#devicegetoptionscb)
- [device.getOptionsSync()](#devicegetoptionssync)
- [device.setOptions(options, cb)](#devicesetoptionsoptions-cb)
- [device.setOptionsSync(options)](#devicesetoptionssyncoptions)
- [device.close(cb)](#deviceclosecb)
- [device.closeSync()](#deviceclosesync)

### Constants

- [MODE0](#mode0)
- [MODE1](#mode1)
- [MODE2](#mode2)
- [MODE3](#mode3)

### open(busNumber, deviceNumber[, options], cb)
- busNumber - the number of the SPI bus to open, 0 for `/dev/spidev0.n`, 1 for `/dev/spidev1.n`, ...
- deviceNumber - the number of the SPI device to open, 0 for `/dev/spidevn.0`, 1 for `/dev/spidevn.1`, ...
- options - an optional object specifying device
[configuration options](#configuration-options)
- cb - completion callback

Asynchronous open. Returns a new SpiDevice object. The completion callback gets
one argument (err). The SpiDevice object returned should not be used before the
completion callback is called.

### openSync(busNumber, deviceNumber[, options])
- busNumber - the number of the SPI bus to open, 0 for `/dev/spidev0.n`, 1 for `/dev/spidev1.n`, ...
- deviceNumber - the number of the SPI device to open, 0 for `/dev/spidevn.0`, 1 for `/dev/spidevn.1`, ...
- options - an optional object specifying device
[configuration options](#configuration-options)

Synchronous open. Returns a new SpiDevice object.

### device.transfer(message, cb)
- message - an array of one or more read+write transfers
- cb - completion callback

Asynchronous message transfer. An SPI
[message](#message) is an array of one or
more read+write transfers. The completion callback gets two arguments (err,
message). Returns this.

### device.transferSync(message)
- message - an array of one or more read+write transfers

Synchronous message transfer. An SPI
[message](#message) is an array of one or
more read+write transfers. Returns this.

### device.getOptions(cb)
- cb - completion callback

Asynchronously read device
[configuration options](#configuration-options).
The completion callback gets two arguments (err, options) where options is an
object describing the device configuration options. Returns this.

### device.getOptionsSync()

Synchronously read device
[configuration options](#configuration-options).
Returns an object describing the device configuration options.

### device.setOptions(options, cb)
- options - an object specifying device
[configuration options](#configuration-options)
- cb - completion callback

Asynchronously write device
[configuration options](#configuration-options).
The completion callback gets one argument (err). Returns this.

### device.setOptionsSync(options)
- options - an object specifying device
[configuration options](#configuration-options)

Synchronously write device
[configuration options](#configuration-options).
Returns this.

### device.close(cb)
- cb - completion callback

Asynchronous close. Frees system resources used by this instance. The
completion callback gets one argument (err). Returns null.

### device.closeSync()

Synchronous close. Frees system resources used by this instance. Returns null.

### MODE0

SPI mode number 0.

### MODE1

SPI mode number 1.

### MODE2

SPI mode number 2.

### MODE3

SPI mode number 3.

### Message

An SPI message is an array of one or more read+write transfers. A transfer
is an object with the properties listed below. Most of the properties are
optional. Note that although both sendBuffer and receiveBuffer are optional,
at least one one of them must be specified.

- byteLength - number, 32-bit, the number of bytes to transfer
- sendBuffer - optional Buffer, transmit data
- receiveBuffer - optional Buffer, receive data
- speedHz - optional number, 32-bit, override of the device's clock frequency
in Hertz
- microSecondDelay - optional number, 16-bit, delay after the last bit transfer
before optionally deselecting the device before the next transfer, default 0
- bitsPerWord - optional number, 8-bit, override of the device's wordsize
- chipSelectChange - optional boolean, true to deselect device before starting
the next transfer, default false

### Configuration Options

Device configuration options can be optionally specified when a device is
opened with the `open` or `openSync` methods. They can also be specified at a
later point with the `setOptions` or `setOptionsSync` methods. When calling
these methods, only the options that need to be set need to be specified in the
options object passed to those methods. All options are optional and the
appropriate defaults will be used for options that are not specified.

The options supported varies from system to system and will depend on the
device drivers used on those systems.

Configurations options can be read with the `getOptions` and `getOptionsSync`
methods.

**IMPORTANT** The semantics of **chipSelectHigh** have changed with Linux
kernel 5. To the best of my knowledge, the chipSelectHigh option no longer
serves any purpose when used from user space with Linux kernel 5 and should
not be used. With Linux kernel 5, the chip select is assumed to be active low.
With Linux kernel 5, if an SPI device has has active high chip select, it's
chip select must be controlled manually with a GPIO using a module such as
[onoff](https://github.com/fivdi/onoff). The chipSelectHigh option has been
crossed out below but it's still available for usage on older kernels.

- mode - number, 2-bit, MODE0, MODE1, MODE2, or MODE3, default MODE0
- ~~chipSelectHigh - boolean, true for active high chip select, default false~~
- lsbFirst - boolean, true for least significant bit first transfer, default
false
- threeWire - boolean, true for shared MISO/MOSI signals, default false
- loopback - boolean, true for loopback mode, default false
- noChipSelect - boolean, true for 1 device per bus, no chip select, default
false
- ready - boolean, true if device pulls low to pause, default false
- bitsPerWord - number, 8-bit, device word size, default 8
- maxSpeedHz - number, 32-bit, device clock frequency in Hertz, default system
specific

