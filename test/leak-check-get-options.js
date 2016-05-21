'use strict';

var spi = require('bindings')('spi'),
  device = spi.openSync(0, 0),
  count = 0;

(function next() {
  device.getOptions(function (err, options) {
    if (err) {
      throw err;
    }

    count += 1;

    if (count % 10000 === 0) {
      console.log(count + ' ' + JSON.stringify(options));
    }

    next();
  });
})();

