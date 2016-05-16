'use strict';

var spi = require('bindings')('spi');
var device = spi.openSync(0, 0);
var count = 0;
var options;

while (true) {
  count += 1;
  options = device.getOptionsSync();
  if (count % 10000 === 0) {
    console.log(count + ' ' + JSON.stringify(options));
  }
}

