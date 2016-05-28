'use strict';

var spi = require('bindings')('spi'),
  assert = require('assert'),
  count = 0,
  totalCount = 0,
  result;

function createMessage() {
  var message = [],
    channelByte;

  [0, 1, 4, 5].forEach(function (channel) {
    channelByte = 0x80 + (channel << 4);

    message.push({
      sendBuffer: new Buffer([0x01, channelByte, 0x00]),
      receiveBuffer: new Buffer(3),
      byteLength: 3,
      speedHz: 1350000,
      chipSelectChange: true
    });
  });

  return message;
}

function saveResult(message) {
  result = '';

  message.forEach(function (transfer) {
    result += (((transfer.receiveBuffer[1] & 0x03) << 8) +
      transfer.receiveBuffer[2]) + ' ';
  });
}

function sync() {
  var mcp3008 = spi.openSync(0, 0),
    message = createMessage();

  mcp3008.setOptionsSync(mcp3008.getOptionsSync());
  mcp3008.transferSync(message);
  saveResult(message);
  mcp3008.closeSync();

  count += 1;
  totalCount += 1;

  async();
}

function transfer(mcp3008, cb) {
  mcp3008.transfer(createMessage(), function (err, message) {
    assert(!err, 'can\'t transfer with mcp3008');
    saveResult(message);
    cb();
  })
}

function accessOptions(mcp3008, cb) {
  mcp3008.getOptions(function (err, options) {
    assert(!err, 'can\'t get options from mcp3008');
    mcp3008.setOptions(options, function (err) {
      assert(!err, 'can\'t set mcp3008 options');
      transfer(mcp3008, cb);
    });
  });
}

function async() {
  var mcp3008 = spi.open(0, 0, function (err) {
    assert(!err, 'can\'t open mcp3008');

    accessOptions(mcp3008, function () {
      mcp3008.close(function (err) {
        assert(!err, 'can\'t close mcp3008');
        sync();
      });
    });
  });
}

setInterval(function () {
  console.log(totalCount + ' ' + count + ' - ' + result);
  count = 0;
}, 1000);

async();

