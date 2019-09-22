'use strict';

const spi = require('bindings')('spi');
const assert = require('assert');

const mcp3008 = spi.open(0, 0, {mode: spi.MODE0}, err => {
  let rawValue;
  let celcius;
  let count = 0;
  let total = 0;

  assert(!err, 'can\'t open device');

  const message = [{
    sendBuffer: Buffer.from([0x01, 0xd0, 0x00]),
    receiveBuffer: Buffer.alloc(3),
    byteLength: 3,
    speedHz: 20000
  }];

  const next = _ => {
    mcp3008.transfer(message, (err, message) => {
      assert(!err, 'can\'t transfer message');

      count += 1;

      rawValue = ((message[0].receiveBuffer[1] & 0x03) << 8) +
        message[0].receiveBuffer[2];
      const voltage = rawValue * 3.3 / 1023;
      celcius = (voltage - 0.5) * 100;

      next();
    });
  };

  next();

  setInterval(_ => {
    total += count;
    console.log(count + ' ' + total + ' ' + rawValue + ' '  + celcius);
    count = 0;
  }, 1000);
});


