Frequency-based String Mining (lite)
===

A singe-core implemetation of frequency-based substring mining. This
implementation requires the https://github.com/simongog/sdsl-lite
library (tested using the release `sdsl-lite-2.0.3`).

# Modify the correct SDSL path into the `Makefile`,
# turn on preferred compiler optimizations, and
# run `make`.

For command-line options, see `./fsm-lite --help`.

TODO
---
# Optimize the time and space.
# Multi-threading.
# Support for gzip compressed input.
