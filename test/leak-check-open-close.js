'use strict';

var spi = require('bindings')('spi');
var count = 0;

(function next() {
  var device = spi.open(0, 0, function (err) {
    if (err) {
      throw err;
    }

    device.close(function () {
      if (err) {
        throw err;
      }

      count += 1;
      if (count % 10000 === 0) {
        console.log(count);
      }
      next();
    });
  });
})();

