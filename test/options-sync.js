'use strict';

var spi = require('bindings')('spi'),
  assert = require('assert'),
  device = spi.openSync(0, 0),
  originalOptions,
  options,
  newOptions;

originalOptions = device.getOptionsSync();

options = {
  mode: spi.MODE2,
  chipSelectHigh: false,
  lsbFirst: false, // pi only supports msb first
  threeWire: false,
  loopback: false, // pi doesn't support loopback
  noChipSelect: false,
  ready: false, // pi doesn't support ready
  bitsPerWord: 8, // pi only supports 8 bit words
  maxSpeedHz: originalOptions.maxSpeedHz + 1e6
};
device.setOptionsSync(options);

newOptions = device.getOptionsSync();

assert.strictEqual(
  options.mode,
  newOptions.mode,
  'can\'t set mode'
);

assert.strictEqual(
  options.chipSelectHigh,
  newOptions.chipSelectHigh,
  'can\'t set chipSelectHigh'
);

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

