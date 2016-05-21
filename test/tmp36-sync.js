'use strict';

var spi = require('bindings')('spi'),
  mcp3008 = spi.openSync(0, 0),
  rawValue,
  voltage,
  celcius,
  count = 0;

var message = [{
  sendBuffer: new Buffer([0x01, 0xd0, 0x00]),
  receiveBuffer: new Buffer([0, 0, 0]),
  byteLength: 3,
  speed: 20000
}];

while (true) {
  mcp3008.transferSync(message);
  count += 1;

  if (count % 1000 === 0) {
    rawValue = (message[0].receiveBuffer[1] << 8) +
      message[0].receiveBuffer[2];
    voltage = rawValue * 3.3 / 1024;
    celcius = (voltage - 0.5) * 100;

    console.log(count + ' ' + rawValue + ' '  + celcius);
  }
}

