'use strict';

var spi = require('bindings')('spi');
var device = spi.openSync(0, 0);

device.getOptions(function (err, options) {
  console.log('err: ' + err);
  console.log(JSON.stringify(options));
});

