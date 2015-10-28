Frequency-based String Mining (lite)
===

A singe-core implemetation of frequency-based substring mining. This
implementation requires the https://github.com/simongog/sdsl-lite
library (tested using the release `sdsl-lite-2.0.3`).

1. Download and extract https://github.com/simongog/sdsl-lite/archive/v2.0.3.tar.gz
2. install SDSL by running `./install.sh /install/path/sdsl-lite-2.0.3`, where `/install/path` need to be specified,
3. update the correct SDSL installation path into the `fsm-lite/Makefile`,
4. turn on preferred compiler optimization in `fsm-lite/Makefile`, and
5. run `make depend && make` under the directory `fsm-lite`.

For command-line options, see `./fsm-lite --help`.

Usage example
---

Input files are given as a list of `<data-identifier>` `<data-filename>` pairs. The `<data-identifier>`'s are assumed to be unique. Here's an example how to construct such a list out of all `/input/dir/*.fasta` files:

  `for f in /input/dir/*.fasta; do id=$(basename "$f" .fasta); echo $id $f; done > input.list` 

The files can then be processed by 

  `./fsm-lite -l input.list -t tmp | gzip - > output.txt.gz`

where `tmp` is a prefix filename for storing temporary index files.

TODO
---
1. Optimize the time and space usage.
2. Multi-threading.
3. Support for gzip compressed input.
