'use strict';

var spi = require('bindings')('spi'),
  assert = require('assert');

var channels = [
  {number: 0, expectedValue: 0, transferCount: 0, errors: 0},
  {number: 1, expectedValue: 1023, transferCount: 0, errors: 0},
  {number: 4, expectedValue: 511, transferCount: 0, errors: 0}
];

channels.forEach(function (channel) {
  var mcp3008 = spi.openSync(0, 0);
  var channelByte = 0x80 + (channel.number << 4);
  var message = [{
    sendBuffer: new Buffer([0x01, channelByte, 0x00]),
    receiveBuffer: new Buffer(3),
    byteLength: 3,
    speed: 1350000
  }];

  (function next() {
    mcp3008.transfer(message, function (err, message) {
      var rawValue;

      assert(!err, 'can\'t transfer with mcp3008');

      channel.transferCount += 1;

      rawValue = (message[0].receiveBuffer[1] << 8) +
        message[0].receiveBuffer[2];
      if (rawValue < channel.expectedValue - 1 ||
          rawValue > channel.expectedValue + 1) {
        console.log(
          '*** channel: ' + channel.number +
          ', expected: ' + channel.expectedValue +
          ', got: ' + rawValue
        );
        channel.errors += 1;
      }

      next();
    });
  })();
});

setInterval(function () {
  channels.forEach(function (channel) {
    console.log(
      'channel: ' + channel.number +
      ', errors: ' + channel.errors +
      ', transfers: ' + channel.transferCount
    );
  });
}, 10000);

