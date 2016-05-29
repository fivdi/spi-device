'use strict';

var spi = require('bindings')('spi'),
  assert = require('assert'),
  mcp3008,
  rawValue,
  celcius,
  count = 0,
  total = 0;

mcp3008 = spi.open(0, 0, {mode: spi.MODE0}, function (err) {
  var message = [{
    sendBuffer: new Buffer([0x01, 0xd0, 0x00]),
    receiveBuffer: new Buffer(3),
    byteLength: 3,
    speedHz: 20000
  }];

  assert(!err, 'can\'t open device');

  (function next() {
    mcp3008.transfer(message, function (err, message) {
      var voltage;

      assert(!err, 'can\'t transfer message');

      count += 1;

      rawValue = ((message[0].receiveBuffer[1] & 0x03) << 8) +
        message[0].receiveBuffer[2];
      voltage = rawValue * 3.3 / 1023;
      celcius = (voltage - 0.5) * 100;

      next();
    });
  }());

  setInterval(function () {
    total += count;
    console.log(count + ' ' + total + ' ' + rawValue + ' '  + celcius);
    count = 0;
  }, 1000);
});


