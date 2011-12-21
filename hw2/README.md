#### Compiling
The code for each part can be compiled using the included Makefile, assuming
you have GNU `make` installed on your system.  The default target builds each
assigned problem and places the corresponding binary in the `out/` directory.

Should GNU `make` not work for you, the following commands should be sufficient:

* For question one (multi-threaded sum of 5000000 floats)

    `gcc -std=c99 -o out/threadSum threadSum.c -pthread`

* For question two (Multipy two NxN matrices with threads):

    `gcc -std=c99 -o out/matrixMult matrixMult.c -pthread`

#### Running
There is no Makefile target to run the executables; these must be called by
hand using the following commands:

* For question one (multi-threaded sum of 5000000 floats)

    `./out/threadSum`

* For question two (Multipy two NxN matrices with threads):

	`./out/matrixMult`

#### Example Output
* For question one (multi-threaded sum of 5000000 floats):

```text
$ ./out/threadSum
Generating random numbers between 0 and 1...

Iterated sum =  2500423.277011
Final sum =     2500423.277011
```

* For question two (Multipy two NxN matrices with threads):

```text
$ ./out/matrixMult
Product:
38      29      49
31      26      35
81      81      111

```

#### Other
The source code for this project (including this README) is available on
[GitHub](github.com/sjbarag/ECE-C353-Programming-Assignment-2).

