http://www.complang.tuwien.ac.at/anton/lvas/effizienz-aufgabe24/#given

The dataset can be downloaded from the URL above.

Call the reference implementation as follows:

```bash
./scripts/myjoin a.csv b.csv c.csv d.csv
```
Get performance stats like this:

```bash
LC_NUMERIC=en_US
perf stat -e cycles ./scripts/myjoin a.csv b.csv c.csv d.csv |cat > /dev/null
```
Why cat?
So you dont cheat i guess. Didnt understand prof completely.

How to check if the result is working correctly (ertl):

```bash
./scripts/myjoin a.csv b.csv c.csv d.csv |sort|diff - output.csv
```
I guess calculating a checksum should also work:

```bash
./scripts/myjoin a.csv b.csv c.csv d.csv |sha256sum
```
