'use strict';

const spi = require('bindings')('spi');
const assert = require('assert');

const checkDefaultOptions = options => {
  assert.strictEqual(options.mode, spi.MODE0, 'default mode incorrect');
  // The semantics of chipSelectHigh have changed in kernel 5 and higher
  //assert.strictEqual(options.chipSelectHigh, false, 'default chipSelectHigh incorrect');
  assert.strictEqual(options.lsbFirst, false, 'default lsbFirst incorrect');
  assert.strictEqual(options.threeWire, false, 'default threeWire incorrect');
  assert.strictEqual(options.loopback, false, 'default loopback incorrect');
  assert.strictEqual(options.noChipSelect, false, 'default noChipSelect incorrect');
  assert.strictEqual(options.ready, false, 'default ready incorrect');
  assert.strictEqual(options.bitsPerWord, 8, 'default bitsPerWord incorrect');
};

const checkOptions = _ => {
  let device = spi.openSync(0, 0);
  checkDefaultOptions(device.getOptionsSync());

  device = spi.open(0, 0, err => {
    assert(!err, 'can\'t open device');
    device.getOptions((err, options) => {
      assert(!err, 'can\'t get options');
      checkDefaultOptions(options);
    });
  });
};

checkOptions();

