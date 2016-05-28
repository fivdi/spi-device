'use strict';

var spi = require('bindings')('spi'),
  assert = require('assert');

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

function sync() {
  var mcp3008 = spi.openSync(0, 0),
    message = createMessage();

  mcp3008.setOptionsSync(mcp3008.getOptionsSync());
  mcp3008.transferSync(message);
  mcp3008.closeSync();
}

function transfer(mcp3008, cb) {
  mcp3008.transfer(createMessage(), function (err, message) {
    assert(!err, 'can\'t transfer with mcp3008');
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

async();

