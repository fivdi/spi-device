'use strict';

var spi = require('bindings')('spi'),
  device = spi.openSync(0, 0),
  count = 0,
  options;

while (true) {
  count += 1;
  options = device.getOptionsSync();
  if (count % 10000 === 0) {
    console.log(count + ' ' + JSON.stringify(options));
  }
}

