'use strict';

var spi = require('bindings')('spi'),
  assert = require('assert'),
  device,
  originalOptions,
  options,
  newOptions;

device = spi.openSync(0, 0);

originalOptions = device.getOptionsSync();

options = {
  maxSpeedHz: originalOptions.maxSpeedHz + 1e6
};
device.setOptionsSync(options);

newOptions = device.getOptionsSync();
assert.strictEqual(
  options.maxSpeedHz,
  newOptions.maxSpeedHz,
  'can\'t set maxSpeedHz'
);

