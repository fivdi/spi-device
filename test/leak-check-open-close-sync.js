'use strict';

var spi = require('bindings')('spi');
var count = 0;

while (true) {
  spi.openSync(0, 0).closeSync();
  count += 1;
  if (count % 100000 === 0) {
    console.log(count);
  }
}

