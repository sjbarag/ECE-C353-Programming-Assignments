#### Compiling
The code for each part can be compiled using the included Makefile, assuming
you have GNU `make` installed on your system.  The default target builds each
assigned problem and places the corresponding binary in the `out/` directory.

Should GNU `make` not work for you, the following commands should be sufficient:

* For question one (sleeping barber with pthread)

    `gcc -std=c99 -o out/sleeping_barber sleeping_barber.c -pthread -lm -lrt

* For question two (multi-threaded histogram):

    `gcc -std=c99 -o out/histogram histogram.c -pthread -lm`

#### Running
There is no Makefile target to run the executables; these must be called by
hand using the following commands:

* For question one (sleeping barber with pthread)

    `./out/sleeping_barber <customers> <chairs>`

* For question two (multi-threaded histogram):

	`./out/histogram <num elements> [num threads]`

#### Example Output
* For question one (sleeping barber with pthread):

```text
$ out/sleeping_barber 10 3
Customer 6: Leaving for the barber shop
Customer 7: Leaving for the barber shop
Customer 8: Leaving for the barber shop
Customer 9: Leaving for the barber shop
Customer 5: Leaving for the barber shop
Customer 4: Leaving for the barber shop
Customer 3: Leaving for the barber shop
Customer 2: Leaving for the barber shop
Customer 1: Leaving for the barber shop
Barber: Sleeping
Customer 0: Leaving for the barber shop
Customer 6: Arrived at the barber shop
Customer 6: Entering waiting room
Customer 6: Getting cut
Customer 3: Arrived at the barber shop
Customer 3: Entering waiting room
Customer 1: Arrived at the barber shop
Customer 1: Entering waiting room
Barber: Cutting hair
Customer 9: Arrived at the barber shop
Customer 9: Entering waiting room
Customer 5: Arrived at the barber shop
Customer 4: Arrived at the barber shop
Customer 2: Arrived at the barber shop
Customer 0: Arrived at the barber shop
Customer 7: Arrived at the barber shop
Customer 8: Arrived at the barber shop
Barber: Done cut
Barber: Sleeping
Customer 6: Going home
Customer 3: Getting cut
Customer 5: Entering waiting room
Barber: Cutting hair
Barber: Done cut
Barber: Sleeping
Customer 3: Going home
Customer 1: Getting cut
Customer 4: Entering waiting room
Barber: Cutting hair
Barber: Done cut
Barber: Sleeping
Customer 1: Going home
Customer 9: Getting cut
Customer 2: Entering waiting room
Barber: Cutting hair
Barber: Done cut
Barber: Sleeping
Customer 9: Going home
Customer 5: Getting cut
Barber: Cutting hair
Customer 0: Entering waiting room
Barber: Done cut
Barber: Sleeping
Customer 5: Going home
Customer 4: Getting cut
Customer 7: Entering waiting room
Barber: Cutting hair
Barber: Done cut
Barber: Sleeping
Customer 4: Going home
Customer 2: Getting cut
Customer 8: Entering waiting room
Barber: Cutting hair
Barber: Done cut
Barber: Sleeping
Customer 2: Going home
Customer 0: Getting cut
Barber: Cutting hair
Barber: Done cut
Barber: Sleeping
Customer 7: Getting cut
Barber: Cutting hair
Customer 0: Going home
Barber: Done cut
Barber: Sleeping
Customer 7: Going home
Customer 8: Getting cut
Barber: Cutting hair
Barber: Done cut
Barber: Sleeping
Customer 8: Going home
Barber: Done for the day. Going home
```

* For question two (multi-threaded histogram):

```text
$ out/histogram 100000000
Creating the reference histogram.
CPU run time (reference) = 0.60 s.

Creating histogram using 8 pthreads.
CPU run time (thread)    = 0.32 s.

Difference between the reference and pthread results: 0.000000.
Scriptable output:       100000000       0.60    0.32
```
