http://www.complang.tuwien.ac.at/anton/lvas/effizienz-aufgabe24/#given

The dataset can be downloaded from the URL above.

Call the reference implementation as follows:

```bash	
clone this repository to the server
cd efficient-programs-WS2024
```

Everything is configured in the makefile. To make the reference implementation, run:

```bash
make VERSION=xy
```

To run the the test on small data, run:

```bash
make testsmall VERSION=xy
```

To run the the test on large data, run:

```bash
make testbig VERSION=xy

```

To run a sanity test with the reference implementation, run:

```bash
make sanity VERSION=xy
```

To get performance data, run:

```bash
make perf VERSION=xy
```

To create a new version, simply create a new folder within src with v{xy} and then execute the respective make command.

Furthermore, we created a shell script to fetch the data from our repo. This makes it easy to just develop on your pc, push it to Git and then execute the shell script (update_from_git.sh) to delete the current avaible version on the server and ƒetch the current version from the repository. Just move the update_from_git.sh file to your home directory. Be careful, it contains an rm -rf for the efficient programs file, so everything that you would implement on the complang machine will be deleted.

Once:
```bash
cd efficient-programs-WS2024
mv update_from_git.sh ../
```

Then just simply:
```bash
/bin/bash update_from_git.sh
```