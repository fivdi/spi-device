'use strict';

const spi = require('bindings')('spi');
const assert = require('assert');

const channels = [
  {number: 0, expectedValue: 0, transferCount: 0, totalTransferCount: 0, errors: 0},
  {number: 1, expectedValue: 1023, transferCount: 0, totalTransferCount: 0, errors: 0},
  {number: 4, expectedValue: 511, transferCount: 0, totalTransferCount: 0, errors: 0}
];

let temperature;

channels.forEach(channel => {
  const mcp3008 = spi.openSync(0, 0);
  const channelByte = 0x80 + (channel.number << 4);
  const message = [{
    sendBuffer: Buffer.from([0x01, channelByte, 0x00]),
    receiveBuffer: Buffer.alloc(3),
    byteLength: 3,
    speedHz: 2000000
  }];

  const next = _ => {
    mcp3008.transfer(message, (err, message) => {
      assert(!err, 'can\'t transfer with mcp3008');

      channel.transferCount += 1;
      channel.totalTransferCount += 1;

      const rawValue = ((message[0].receiveBuffer[1] & 0x03) << 8) +
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
  };

  next();
});

setInterval(_ => {
  const message = [{
    sendBuffer: Buffer.from([0x01, 0xd0, 0x00]),
    receiveBuffer: Buffer.alloc(3),
    byteLength: 3,
    speedHz: 20000
  }];

  const mcp3008 = spi.openSync(0, 0);
  mcp3008.transferSync(message);

  const rawValue = ((message[0].receiveBuffer[1] & 0x03) << 8) +
    message[0].receiveBuffer[2];
  const voltage = rawValue * 3.3 / 1023;
  temperature = (voltage - 0.5) * 100;
}, 1000);

setInterval(_ => {
  console.log('temperature: ' + temperature);

  channels.forEach(channel => {
    console.log(
      'channel: ' + channel.number +
      ', errors: ' + channel.errors +
      ', transfers: ' + channel.transferCount +
      ', total transfers: ' + channel.totalTransferCount
    );

    channel.transferCount = 0;
  });
}, 10000);

