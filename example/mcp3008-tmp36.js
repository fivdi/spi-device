'use strict';

var spi = require('bindings')('spi');

var message = [{
  sendBuffer: new Buffer([0x01, 0xd0, 0x00]),
  receiveBuffer: new Buffer(3),
  byteLength: 3,
  speed: 20000
}];

var mcp3008 = spi.open(0, 0, function (err) {
  if (err) {
    throw err;
  }

  mcp3008.transfer(message, function (err, message) {
    var rawValue;
    var voltage;
    var celcius;

    if (err) {
      throw err;
    }

    rawValue = (message[0].receiveBuffer[1] << 8) +
      message[0].receiveBuffer[2];
    voltage = rawValue * 3.3 / 1024;
    celcius = (voltage - 0.5) * 100;

    console.log(celcius);
  });
});

