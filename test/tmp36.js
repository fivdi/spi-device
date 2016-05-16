'use strict';

var spi = require('bindings')('spi');
var mcp3008 = spi.openSync(0, 0);
var rawValue;
var celcius;
var count = 0;
var total = 0;

var message = [{
  sendBuffer: new Buffer([0x01, 0xd0, 0x00]),
  receiveBuffer: new Buffer([0, 0, 0]),
  byteLength: 3,
  speed: 20000
}];

(function next() {
  mcp3008.transfer(message, function (err, message) {
    var voltage;

    count += 1;

    rawValue = (message[0].receiveBuffer[1] << 8) +
      message[0].receiveBuffer[2];
    voltage = rawValue * 3.3 / 1024;
    celcius = (voltage - 0.5) * 100;

    next();
  });
}());

setInterval(function () {
  total += count;
  console.log(count + ' ' + total + ' ' + rawValue + ' '  + celcius);
  count = 0;
}, 1000);

