'use strict';

const spi = require('bindings')('spi');
const assert = require('assert');

let count = 0;
let totalCount = 0;
let result;

const createMessage = _ => {
  let message = [];
  let channelByte;

  [0, 1, 4, 5].forEach(channel => {
    channelByte = 0x80 + (channel << 4);

    message.push({
      sendBuffer: Buffer.from([0x01, channelByte, 0x00]),
      receiveBuffer: Buffer.alloc(3),
      byteLength: 3,
      speedHz: 1350000,
      chipSelectChange: true
    });
  });

  return message;
};

const saveResult = message => {
  result = '';

  message.forEach(transfer => {
    result += (((transfer.receiveBuffer[1] & 0x03) << 8) +
      transfer.receiveBuffer[2]) + ' ';
  });
};

const sync = _ => {
  const mcp3008 = spi.openSync(0, 0);
  const message = createMessage();

  mcp3008.setOptionsSync(mcp3008.getOptionsSync());
  mcp3008.transferSync(message);
  saveResult(message);
  mcp3008.closeSync();

  count += 1;
  totalCount += 1;

  async();
};

const transfer = (mcp3008, cb) => {
  mcp3008.transfer(createMessage(), (err, message) => {
    assert(!err, 'can\'t transfer with mcp3008');
    saveResult(message);
    cb();
  });
};

const accessOptions = (mcp3008, cb) => {
  mcp3008.getOptions((err, options) => {
    assert(!err, 'can\'t get options from mcp3008');
    mcp3008.setOptions(options, (err) => {
      assert(!err, 'can\'t set mcp3008 options');
      transfer(mcp3008, cb);
    });
  });
};

const async = _ => {
  const mcp3008 = spi.open(0, 0, err => {
    assert(!err, 'can\'t open mcp3008');

    accessOptions(mcp3008, _ => {
      mcp3008.close(err => {
        assert(!err, 'can\'t close mcp3008');
        sync();
      });
    });
  });
};

setInterval(_ => {
  console.log(totalCount + ' ' + count + ' - ' + result);
  count = 0;
}, 1000);

async();

