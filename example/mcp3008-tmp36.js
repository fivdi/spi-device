'use strict';

var spi = require('bindings')('spi'),
  mcp3008;

// Determine the temperature using a TMP36 analog temperature sensor wired to
// channel 5 on an MCP3008 SPI A/D converter

// The MCP3008 is on bus 0 and it's device 0
mcp3008 = spi.open(0, 0, function (err) {
  // An SPI message is an array of one or more read+write transfers
  var message = [{
    sendBuffer: new Buffer([0x01, 0xd0, 0x00]), // Sent to read channel 5
    receiveBuffer: new Buffer(3),               // Raw data read from channel 5
    byteLength: 3,
    speedHz: 20000 // Use a low bus speed to get a good reading from the TMP36
  }];

  if (err) throw err;

  mcp3008.transfer(message, function (err, message) {
    var rawValue,
      voltage,
      celcius;

    if (err) throw err;

    // Convert raw value from sensor to celcius and log to console
    rawValue = ((message[0].receiveBuffer[1] & 0x03) << 8) +
      message[0].receiveBuffer[2];
    voltage = rawValue * 3.3 / 1023;
    celcius = (voltage - 0.5) * 100;

    console.log(celcius);
  });
});

