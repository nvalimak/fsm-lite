Frequent String Mining (lite)
===

A singe-core implemetation of frequency-based substring
mining. Requires the [https://github.com/simongog/sdsl-lite] library
(tested using the release `sdsl-lite-2.0.3`).

1) Modify the correct SDSL path into the `Makefile`,
2) turn on preferred compiler optimizations, and
3) run `make`.

For command-line options, see `./fsm-lite --help`.

TODO
---
1) Optimize the time and space.
2) Multi-threading.
3) Support for gzip compressed input.
