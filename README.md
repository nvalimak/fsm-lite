Frequency-based String Mining (lite)
===

A singe-core implemetation of frequency-based substring mining. This
implementation requires the https://github.com/simongog/sdsl-lite
library (tested using the release `sdsl-lite-2.0.3`).

0. Download and install https://github.com/simongog/sdsl-lite/archive/v2.0.3.tar.gz
1. modify the correct SDSL path into the `Makefile`,
2. turn on preferred compiler optimizations, and
3. run `make depend && make`.

For command-line options, see `./fsm-lite --help`.

TODO
---
1. Optimize the time and space.
2. Multi-threading.
3. Support for gzip compressed input.
