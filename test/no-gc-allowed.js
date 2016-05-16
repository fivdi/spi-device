'use strict';

var spi = require('bindings')('spi');
var count = 0;

var message = [{
  sendBuffer: new Buffer([0x01, 0xd0, 0x00]),
  receiveBuffer: new Buffer([0, 0, 0]),
  byteLength: 3,
  speed: 20000
}];

spi.openSync(0, 0).transfer(message, function () {
  console.log('finished');
});

(function next() {
  setImmediate(function () {
    count += 1;
    next();
  });
})();

setInterval(function () {
  console.log(count);
}, 10000);

