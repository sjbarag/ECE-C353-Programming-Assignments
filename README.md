#### Compiling
The code for each part can be compiled using the included Makefile, assuming
you have GNU `make` installed on your system.  The default target builds each
assigned problem and places the corresponding binary in the `out/` directory.
Please note the presence of the non-standard "cleanshm" target, which removes
the shared memory object created in question two without removing any binaries.

Should GNU `make` not work for you, the following commands should be sufficient:

* For question one (change the case of a string using pipes):
    ```gcc -std=c99 -o out/changeCaseStr changeCaseStr.c```
* For question two (sum two numbers using shared memory):
    ```gcc -std=c99 -o out/sum sum.c -lrt```

#### Running
There is no Makefile target to run the executables; these must be called by
hand using the following commands:

* For question one (change the case of a string using pipes):
    ```./out/changeCaseStr 'your text here'```
* For question two (sum two numbers using shared memory):
	```./out/sum```

#### Example Output
* For question one (change the case of a string using pipes):
	```./out/changeCaseStr 'Lorem iPsUm doloR SIT amet!'
	lOREM IpSuM DOLOr sit AMET!```
* For question two (sum two numbers using shared memory):
    ```Enter the first number:         1
    Enter the second number:        73
    Sum:                            74

	... (repeats prompt)```

#### Other
The source code for this project (including this README) is available on
[GitHub](github.com/sjbarag/ECE-C353-Programming-Assignment-1).

