'use strict';

const spi = require('bindings')('spi');
const assert = require('assert');

const createMessage = transferCount => {
  let message = [];

  for (let count = 0; count !== transferCount; count += 1) {
    message.push({
      sendBuffer: Buffer.from([0x01, 0xc0, 0x00]),
      receiveBuffer: Buffer.alloc(3),
      byteLength: 3,
      speedHz: 1350000,
      chipSelectChange: true
    });
  }

  return message;
};

const transferNothing = _ => {
  const mcp3008 = spi.openSync(0, 0);
  mcp3008.transferSync([]);
  mcp3008.closeSync();
};

const transferAsManyAsPossible = _ => {
  const mcp3008 = spi.openSync(0, 0);
  mcp3008.transferSync(createMessage(511)); // works
  mcp3008.closeSync();
};

const transferToMany = _ => {
  const mcp3008 = spi.openSync(0, 0);

  try {
    mcp3008.transferSync(createMessage(512)); // doesn't work
    assert.ok(false, 'expected exception for 512 transfers');
  } catch (e) {
    assert.strictEqual(e.code, 'EINVAL', 'expected EINVAL error');
  }

  mcp3008.closeSync();
};

const transferMessageThatsTooLong = _ => {
  const mcp3008 = spi.openSync(0, 0),
    message = [{
      sendBuffer: Buffer.alloc(10000),
      receiveBuffer: Buffer.alloc(10000),
      byteLength: 10000,
    }];

  try {
    mcp3008.transferSync(message); // doesn't work
    assert.ok(false, 'expected exception for long message');
  } catch (e) {
    assert.strictEqual(e.code, 'EMSGSIZE', 'expected EMSGSIZE error');
  }

  mcp3008.closeSync();
};

transferNothing();
transferAsManyAsPossible();
transferToMany();
transferMessageThatsTooLong();

