'use strict';

var spi = require('bindings')('spi'),
  assert = require('assert');

function createMessage(transferCount) {
  var message = [],
    count;

  for (count = 0; count != transferCount; count += 1) {
    message.push({
      sendBuffer: new Buffer([0x01, 0xc0, 0x00]),
      receiveBuffer: new Buffer(3),
      byteLength: 3,
      speedHz: 1350000,
      chipSelectChange: true
    });
  }

  return message;
}

(function transferNothing() {
  var mcp3008 = spi.openSync(0, 0);
  mcp3008.transferSync([]);
  mcp3008.closeSync();
})();

(function transferAsManyAsPossible() {
  var mcp3008 = spi.openSync(0, 0);
  mcp3008.transferSync(createMessage(511)); // works
  mcp3008.closeSync();
})();

(function transferToMany() {
  var mcp3008 = spi.openSync(0, 0);

  try {
    mcp3008.transferSync(createMessage(512)); // doesn't work
    assert.ok(false, 'expected exception for 512 transfers');
  } catch (e) {
    assert.strictEqual(e.code, 'EINVAL', 'expected EINVAL error');
  }

  mcp3008.closeSync();
})();

(function transferMessageThatsTooLong() {
  var mcp3008 = spi.openSync(0, 0),
    message = [{
      sendBuffer: new Buffer(10000),
      receiveBuffer: new Buffer(10000),
      byteLength: 10000,
    }];

  try {
    mcp3008.transferSync(message); // doesn't work
    assert.ok(false, 'expected exception for long message');
  } catch (e) {
    assert.strictEqual(e.code, 'EMSGSIZE', 'expected EMSGSIZE error');
  }

  mcp3008.closeSync();
})();

