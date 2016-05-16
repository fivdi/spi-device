'use strict';

var spi = require('bindings')('spi');
var device = spi.openSync(0, 0);

console.log(JSON.stringify(device.getOptionsSync()));

