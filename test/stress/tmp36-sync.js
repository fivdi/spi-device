'use strict';

var spi = require('bindings')('spi'),
  mcp3008 = spi.openSync(0, 0, {mode: spi.MODE0}),
  rawValue,
  voltage,
  celcius,
  count = 0;

var message = [{
  sendBuffer: new Buffer([0x01, 0xd0, 0x00]),
  receiveBuffer: new Buffer(3),
  byteLength: 3,
  speedHz: 20000
}];

while (true) {
  mcp3008.transferSync(message);
  count += 1;

  if (count % 1000 === 0) {
    rawValue = ((message[0].receiveBuffer[1] & 0x03) << 8) +
      message[0].receiveBuffer[2];
    voltage = rawValue * 3.3 / 1023;
    celcius = (voltage - 0.5) * 100;

    console.log(count + ' ' + rawValue + ' '  + celcius);
  }
}

