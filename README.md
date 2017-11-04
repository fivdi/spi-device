## spi-device

SPI serial bus access with **Node.js** on Linux boards like the Raspberry
Pi or BeagleBone. All methods have asynchronous and synchronous forms.

spi-device supports Node.js versions 0.10, 0.12, 4, 5, 6, 7, 8 and 9.

## Contents

 * [Installation](https://github.com/fivdi/spi-device#installation)
 * [Usage](https://github.com/fivdi/spi-device#usage)
 * [API documentation](https://github.com/fivdi/spi-device#api-documentation)

## Installation

```
npm install spi-device
```

## Usage

Determine the temperature using a TMP36 analog temperature sensor wired to
channel 5 on an MCP3008 SPI A/D converter.

<img src="https://raw.githubusercontent.com/fivdi/spi-device/master/example/pi-mcp3008-tmp36.png">

```js
var spi = require('spi-device'),
  mcp3008;

// The MCP3008 is on bus 0 and it's device 0
mcp3008 = spi.open(0, 0, function (err) {
  // An SPI message is an array of one or more read+write transfers
  var message = [{
    sendBuffer: new Buffer([0x01, 0xd0, 0x00]), // Sent to read channel 5
    receiveBuffer: new Buffer(3),               // Raw data read from channel 5
    byteLength: 3,
    speedHz: 20000 // Use a low bus speed to get a good reading from the TMP36
  }];

  if (err) throw err;

  mcp3008.transfer(message, function (err, message) {
    var rawValue,
      voltage,
      celcius;

    if (err) throw err;

    // Convert raw value from sensor to celcius and log to console
    rawValue = ((message[0].receiveBuffer[1] & 0x03) << 8) +
      message[0].receiveBuffer[2];
    voltage = rawValue * 3.3 / 1023;
    celcius = (voltage - 0.5) * 100;

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

## API documentation

All methods have asynchronous and synchronous forms.

The asynchronous form always take a completion callback as its last argument.
The arguments passed to the completion callback depend on the method, but the
first argument is always reserved for an exception. If the operation was
completed successfully, then the first argument will be null or undefined.

When using the synchronous form any exceptions are immediately thrown. You can
use try/catch to handle exceptions or allow them to bubble up. 

### Functions

- [open(busNumber, deviceNumber[, options], cb)](https://github.com/fivdi/spi-device#openbusnumber-devicenumber-options-cb)
- [openSync(busNumber, deviceNumber[, options])](https://github.com/fivdi/spi-device#opensyncbusnumber-devicenumber-options)

### Class SpiDevice

- [device.transfer(message, cb)](https://github.com/fivdi/spi-device#devicetransfermessage-cb)
- [device.transferSync(message)](https://github.com/fivdi/spi-device#devicetransfersyncmessage)
- [device.getOptions(cb)](https://github.com/fivdi/spi-device#devicegetoptionscb)
- [device.getOptionsSync()](https://github.com/fivdi/spi-device#devicegetoptionssync)
- [device.setOptions(options, cb)](https://github.com/fivdi/spi-device#devicesetoptionsoptions-cb)
- [device.setOptionsSync(options)](https://github.com/fivdi/spi-device#devicesetoptionssyncoptions)
- [device.close(cb)](https://github.com/fivdi/spi-device#deviceclosecb)
- [device.closeSync()](https://github.com/fivdi/spi-device#deviceclosesync)

### Constants

- [MODE0](https://github.com/fivdi/spi-device#mode0)
- [MODE1](https://github.com/fivdi/spi-device#mode1)
- [MODE2](https://github.com/fivdi/spi-device#mode2)
- [MODE3](https://github.com/fivdi/spi-device#mode3)

### open(busNumber, deviceNumber[, options], cb)
- busNumber - the number of the SPI bus to open, 0 for `/dev/spidev0.n`, 1 for `/dev/spidev1.n`, ...
- deviceNumber - the number of the SPI device to open, 0 for `/dev/spidevn.0`, 1 for `/dev/spidevn.1`, ...
- options - an optional object specifying device
[configuration options](https://github.com/fivdi/spi-device#configuration-options)
- cb - completion callback

Asynchronous open. Returns a new SpiDevice object. The completion callback gets
one argument (err). The SpiDevice object returned should not be used before the
completion callback is called.

### openSync(busNumber, deviceNumber[, options])
- busNumber - the number of the SPI bus to open, 0 for `/dev/spidev0.n`, 1 for `/dev/spidev1.n`, ...
- deviceNumber - the number of the SPI device to open, 0 for `/dev/spidevn.0`, 1 for `/dev/spidevn.1`, ...
- options - an optional object specifying device
[configuration options](https://github.com/fivdi/spi-device#configuration-options)

Synchronous open. Returns a new SpiDevice object.

### device.transfer(message, cb)
- message - an array of one or more read+write transfers
- cb - completion callback

Asynchronous message transfer. An SPI
[message](https://github.com/fivdi/spi-device#message) is an array of one or
more read+write transfers. The completion callback gets two arguments (err,
message). Returns this.

### device.transferSync(message)
- message - an array of one or more read+write transfers

Synchronous message transfer. An SPI
[message](https://github.com/fivdi/spi-device#message) is an array of one or
more read+write transfers. Returns this.

### device.getOptions(cb)
- cb - completion callback

Asynchronously read device
[configuration options](https://github.com/fivdi/spi-device#configuration-options).
The completion callback gets two arguments (err, options) where options is an
object describing the device configuration options. Returns this.

### device.getOptionsSync()

Synchronously read device
[configuration options](https://github.com/fivdi/spi-device#configuration-options).
Returns an object describing the device configuration options.

### device.setOptions(options, cb)
- options - an object specifying device
[configuration options](https://github.com/fivdi/spi-device#configuration-options)
- cb - completion callback

Asynchronously write device
[configuration options](https://github.com/fivdi/spi-device#configuration-options).
The completion callback gets one argument (err). Returns this.

### device.setOptionsSync(options)
- options - an object specifying device
[configuration options](https://github.com/fivdi/spi-device#configuration-options)

Synchronously write device
[configuration options](https://github.com/fivdi/spi-device#configuration-options).
Returns this.

### device.close(cb)
- cb - completion callback

Asynchronous close. The completion callback gets one argument (err). Returns
null.

### device.closeSync()

Synchronous close. Returns null.

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

### Configuration options

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

- mode - number, 2-bit, MODE0, MODE1, MODE2, or MODE3, default MODE0
- chipSelectHigh - boolean, true for active high chip select, default false
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

