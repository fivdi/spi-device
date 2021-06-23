3.1.2 / Jun 23 2021
===================

  * fix compile error occuring with gcc 10 (fixes [#21](https://github.com/fivdi/spi-device/issues/21))

3.1.1 / Apr 30 2021
===================

  * drop support for node.js 8 and 13, add support for node.js 15 and 16
  * update dependencies
  * fix chipSelectHigh documentation and tests (fixes [#16](https://github.com/fivdi/spi-device/issues/16) and [#18](https://github.com/fivdi/spi-device/issues/18))

3.1.0 / Apr 25 2020
===================

  * update dependencies (nan v2.14.1, jshint v2.11.0, @types/node v13.13.2)
  * drop support for node.js 6, add support for node.js 14


3.0.1 / Apr 08 2020
===================

  * fix TypeScript type definition of SpiMessage

3.0.0 / Sep 22 2019
===================

  * drop support for node.js v4

2.0.9 / Sep 07 2019
===================

  * add type definitions for TypeScript (thank you [@huming2207](https://github.com/huming2207))
  * suppress cast-function-type warnings

2.0.8 / Jun 16 2019
===================

  * remove node 11 from build
  * update dependencies
  * update npm keywords

2.0.7 / Mar 15 2019
===================

  * update dependencies (nan v2.13.1, bindings v1.5.0)
  * document node 12 support
  * lint with jshint
  * only compile c++ code on linux

2.0.6 / Dec 19 2018
===================

  * fix deprecation warnings on node.js v12 nightly
  * update dependencies (nan v2.12.1, bindings 1.3.1)

2.0.5 / Oct 13 2018
===================

  * fix deprecation warnings on node.js v10.12 (see https://github.com/nodejs/nan/pull/811)

2.0.4 / Sep 29 2018
===================

  * update dependencies (nan v2.11.1)
  * adapt to V8 7.0: replace v8Value->Uint32Value() with Nan::To<uint32_t>(v8Value).FromJust()
  * adapt to V8 7.0: replace v8Value->BooleanValue() with Nan::To<bool>(v8Value).FromJust()

2.0.3 / Jul 28 2018
===================

  * code style

2.0.2 / Apr 14 2018
===================

  * code style

2.0.1 / Apr 07 2018
===================

  * update dependencies (nan v2.10.0)

2.0.0 / Feb 25 2018
===================

  * update dependencies (nan v2.9.2)
  * drop support for node.js v0.10, v0.12, v5 and v7

1.0.2 / Dec 28 2017
===================

  * don't suppress deprecated-declaration warnings
  * update dependencies (nan v2.8.0)

1.0.1 / Nov 04 2017
===================

  * fix typos in history
  * suppress deprecated-declaration warnings
  * document node 9 support

1.0.0 / Oct 14 2017
===================

  * update dependencies (bindings v1.3.0, nan v2.7.0)
  * document default values for microSecondDelay and chipSelectChange
  * document supported node versions

0.2.6 / Jan 09 2017
===================

  * documentation

0.2.5 / Dec 28 2016
===================

  * use maybe instances to fix compiler warnings
  * upgrade to nan v2.5.0

0.2.4 / Oct 02 2016
===================

  * upgraded to nan v2.4.0

0.2.3 / May 31 2016
===================

  * documentation

0.2.2 / May 30 2016
===================

  * set default configuration options when open is called

0.2.1 / May 28 2016
===================

  * pi-mcp3008-tmp36 Fritzing
  * mask out unused bits in raw value from mcp3008

0.2.0 / May 28 2016
===================

  * transfer option speed renamed to speedHz

0.1.1 / May 26 2016
===================

  * critical section for option modification
  * tests improved
  * check to see if requested transfer takes place

0.1.0 / May 25 2016
===================

  * documentation
  * device configuration options
  * transfer options

0.0.0 / May 16 2016
===================

  * initial release

