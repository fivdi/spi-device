'use strict';

var spi = require('bindings')('spi'),
  assert = require('assert'),
  temperature;

var channels = [
  {number: 0, expectedValue: 0, transferCount: 0, totalTransferCount: 0, errors: 0},
  {number: 1, expectedValue: 1023, transferCount: 0, totalTransferCount: 0, errors: 0},
  {number: 4, expectedValue: 511, transferCount: 0, totalTransferCount: 0, errors: 0}
];

channels.forEach(function (channel) {
  var mcp3008 = spi.openSync(0, 0);
  var channelByte = 0x80 + (channel.number << 4);
  var message = [{
    sendBuffer: new Buffer([0x01, channelByte, 0x00]),
    receiveBuffer: new Buffer(3),
    byteLength: 3,
    speedHz: 2000000
  }];

  (function next() {
    mcp3008.transfer(message, function (err, message) {
      var rawValue;

      assert(!err, 'can\'t transfer with mcp3008');

      channel.transferCount += 1;
      channel.totalTransferCount += 1;

      rawValue = ((message[0].receiveBuffer[1] & 0x03) << 8) +
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
  var mcp3008 = spi.openSync(0, 0),
    rawValue,
    voltage;

  var message = [{
    sendBuffer: new Buffer([0x01, 0xd0, 0x00]),
    receiveBuffer: new Buffer(3),
    byteLength: 3,
    speedHz: 20000
  }];

  mcp3008.transferSync(message);

  rawValue = ((message[0].receiveBuffer[1] & 0x03) << 8) +
    message[0].receiveBuffer[2];
  voltage = rawValue * 3.3 / 1023;
  temperature = (voltage - 0.5) * 100;
}, 1000);

setInterval(function () {
  console.log('temperature: ' + temperature);

  channels.forEach(function (channel) {
    console.log(
      'channel: ' + channel.number +
      ', errors: ' + channel.errors +
      ', transfers: ' + channel.transferCount +
      ', total transfers: ' + channel.totalTransferCount
    );

    channel.transferCount = 0;
  });
}, 10000);

