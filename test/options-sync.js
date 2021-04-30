'use strict';

const spi = require('bindings')('spi');
const assert = require('assert');

const device = spi.openSync(0, 0);

const originalOptions = device.getOptionsSync();

const options = {
  mode: spi.MODE2,
  // The semantics of chipSelectHigh have changed in kernel 5 and higher
  //chipSelectHigh: false,
  lsbFirst: false, // pi only supports msb first
  threeWire: false,
  loopback: false, // pi doesn't support loopback
  noChipSelect: false,
  ready: false, // pi doesn't support ready
  bitsPerWord: 8, // pi only supports 8 bit words
  maxSpeedHz: originalOptions.maxSpeedHz + 1e6
};

device.setOptionsSync(options);

const newOptions = device.getOptionsSync();

assert.strictEqual(
  options.mode,
  newOptions.mode,
  'can\'t set mode'
);

// The semantics of chipSelectHigh have changed in kernel 5 and higher
//assert.strictEqual(
//  options.chipSelectHigh,
//  newOptions.chipSelectHigh,
//  'can\'t set chipSelectHigh'
//);

assert.strictEqual(
  options.lsbFirst,
  newOptions.lsbFirst,
  'can\'t set lsbFirst'
);

assert.strictEqual(
  options.threeWire,
  newOptions.threeWire,
  'can\'t set threeWire'
);

assert.strictEqual(
  options.loopback,
  newOptions.loopback,
  'can\'t set loopback'
);

assert.strictEqual(
  options.noChipSelect,
  newOptions.noChipSelect,
  'can\'t set noChipSelect'
);

assert.strictEqual(
  options.ready,
  newOptions.ready,
  'can\'t set ready'
);

assert.strictEqual(
  options.maxSpeedHz,
  newOptions.maxSpeedHz,
  'can\'t set maxSpeedHz'
);

assert.strictEqual(
  options.bitsPerWord,
  newOptions.bitsPerWord,
  'can\'t set bitsPerWord'
);

device.setOptionsSync(originalOptions);

