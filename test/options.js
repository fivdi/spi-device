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
      mode: spi.MODE3,
      chipSelectHigh: true,
      lsbFirst: false,
      threeWire: true,
      loopback: false,
      noChipSelect: true,
      ready: false,
      bitsPerWord: 8,
      maxSpeedHz: originalOptions.maxSpeedHz + 1e4
    }

    device.setOptions(options, function (err) {
      assert(!err, 'can\'t set options');

      device.getOptions(function (err, newOptions) {
        assert(!err, 'can\'t get new options');

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
          options.bitsPerWord,
          newOptions.bitsPerWord,
          'can\'t set bitsPerWord'
        );

        assert.strictEqual(
          options.maxSpeedHz,
          newOptions.maxSpeedHz,
          'can\'t set maxSpeedHz'
        );

        device.setOptions(originalOptions, function (err) {
          assert(!err, 'can\'t set options');
        })
      });
    });
  });
});

