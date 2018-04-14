'use strict';

const spi = require('bindings')('spi');
const mcp3008 = spi.openSync(0, 0, {mode: spi.MODE0});

const message = [{
  sendBuffer: Buffer.from([0x01, 0xd0, 0x00]),
  receiveBuffer: Buffer.alloc(3),
  byteLength: 3,
  speedHz: 20000
}];

for (let count = 0; ; count += 1) {
  mcp3008.transferSync(message);

  if (count % 1000 === 0) {
    const rawValue = ((message[0].receiveBuffer[1] & 0x03) << 8) +
      message[0].receiveBuffer[2];
    const voltage = rawValue * 3.3 / 1023;
    const celcius = (voltage - 0.5) * 100;

    console.log(count + ' ' + rawValue + ' '  + celcius);
  }
}

