'use strict';

var spi = require('bindings')('spi'),
  assert = require('assert');

function checkDefaultOptions(options) {
  assert.strictEqual(options.mode, spi.MODE0, 'default mode incorrect');
  assert.strictEqual(options.chipSelectHigh, false, 'default chipSelectHigh incorrect');
  assert.strictEqual(options.lsbFirst, false, 'default lsbFirst incorrect');
  assert.strictEqual(options.threeWire, false, 'default threeWire incorrect');
  assert.strictEqual(options.loopback, false, 'default loopback incorrect');
  assert.strictEqual(options.noChipSelect, false, 'default noChipSelect incorrect');
  assert.strictEqual(options.ready, false, 'default ready incorrect');
  assert.strictEqual(options.bitsPerWord, 8, 'default bitsPerWord incorrect');
}

(function () {
  var device;

  device = spi.openSync(0, 0);
  checkDefaultOptions(device.getOptionsSync());

  device = spi.open(0, 0, function (err) {
    assert(!err, 'can\'t open device');
    device.getOptions(function (err, options) {
      assert(!err, 'can\'t get options');
      checkDefaultOptions(options);
    });
  });
})();

