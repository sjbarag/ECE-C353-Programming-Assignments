#### Compiling
The code for each part can be compiled using the included Makefile, assuming
you have GNU `make` installed on your system.  The default target builds each
assigned problem and places the corresponding binary in the `out/` directory.

Should GNU `make` not work for you, the following command should be sufficient:

    `gcc -std=c99 -lpthead -o out/work_crew src/work_crew.c src/queue_utils.c`

#### Running
The program can be executed with `make run` after changing the values of `DIR`
and `STRING` to appropriate values.  If GNU `make` is not available or you just
want to run the application directly, you can simply run:

    `out/work_crew string directory`

#### Example Output
Because the output for this application is mostly system dependant, it is not
included here in its entirety.  However, the application should terminally
output a table of the following form:

                         Threads    Single    Multi    Match
    Scriptable Output:        8    3.292550  3.16328   true

#### Other
The source code for this project (including this README) is available on
[GitHub](github.com/sjbarag/ECE-C353-Programming-Assignment-4).

