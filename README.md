Frequency-based String Mining (lite)
===

A singe-core implemetation of frequency-based substring mining. This
implementation requires the https://github.com/simongog/sdsl-lite
library (tested using the release `sdsl-lite-2.0.3`).

1. Download and install https://github.com/simongog/sdsl-lite/archive/v2.0.3.tar.gz
2. modify the correct SDSL path into the `Makefile`,
3. turn on preferred compiler optimizations, and
4. run `make depend && make`.

For command-line options, see `./fsm-lite --help`.

Usage example
---

Input files are given as a list of `<data-identifier>` `<data-filename>` pairs. The `<data-identifier>`'s are assumed to be unique. Here's an example how to construct such a list out of all `*.fasta` files in the current directory:

  `ls *.fasta | awk '{split($1,a,"."); print a[1],$1}' > input.list` 

The files can then be processed by 

  `./fsm-lite -l input.list -t tmp | gzip - > output.txt.gz`

where `tmp` is a prefix filename for storing temporary index files.

TODO
---
1. Optimize the time and space usage.
2. Multi-threading.
3. Support for gzip compressed input.
