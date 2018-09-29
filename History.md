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

