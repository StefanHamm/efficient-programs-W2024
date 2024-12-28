http://www.complang.tuwien.ac.at/anton/lvas/effizienz-aufgabe24/#given

The dataset can be downloaded from the URL above.

Call the reference implementation as follows:

```bash	
clone this repository to the server
cd efficient-programs-WS2024
```

Everything is configured in the makefile. To make the reference implementation, run:

```bash
make
```

To run the the test on small data, run:

```bash
make testsmall
```

To run the the test on large data, run:

```bash
make testbig

```

To run a sanity test with the reference implementation, run:

```bash
make sanitytest
```

To get performance data, run:

```bash
make perf
```