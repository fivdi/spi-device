'use strict';

var spi = require('bindings')('spi'),
  assert = require('assert'),
  device;

device = spi.open(0, 0, function (err) {
  assert(!err, 'can\'t open device');

  device.getOptions(function (err, originalOptions) {
    var options;

    assert(!err, 'can\'t get original options');

    options = {
      maxSpeedHz: originalOptions.maxSpeedHz + 1e6
    }

    device.setOptions(options, function (err) {
      assert(!err, 'can\'t set options');

      device.getOptions(function (err, newOptions) {
        assert(!err, 'can\'t get new options');

        assert.strictEqual(
          options.maxSpeedHz,
          newOptions.maxSpeedHz,
          'can\'t set maxSpeedHz'
        );
      });
    });
  });
});

