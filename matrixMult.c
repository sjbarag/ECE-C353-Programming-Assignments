#include <stdio.h>
#include <string.h>
#include <unistd.h>
// for threads
#include <pthread.h>
// for file manipulation
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// matrix subscript definitions
#define _A_ 0
#define _B_ 1


int fd;
fd = open('input_data.txt', O_RDONLY);
/* figure out how to read line by line */
read(line, 8, ) // 8 chars: 'ROWS __' + 1?
numRows = strtok(line, "ROWS "); // again, not threadsafe
numCols = numRows;
read() // don't need columns - they're square matrices :)
int data[2][numRows][numCols];
int *array = data;

int outData[numRows][numCols];
int *out = outData;

int threadPool[numRows][numCols];

typedef struct input_args
{
	int i;
	int j;
	int *src;
	int *dst;
} INPUT_ARGS;

void parseFile()
{
	for(int k=0; k < 2; k++) // do it once for A and B.  Too lazy to copy paste this, so I'll carry arround a 3-dimensional array...
	{
		for(int row=0; row < numRows; row++)
		{
			line = read()
			for(int col=0; col < numCols; col++)
			{
				if( col == 1)
					/* strtok() is not threadsafe. BUT, this is before threads are spawned, so I don't think I care */
					array[k][row][col] = strtok(line, " ");
				else
	  				array[k][row][col] = strtok(NULL, " ");
			}
		}
		if( k == 0 )
			read() // handle the linebreak
	}
}

for(int i = 0;  i < numCols; i++)
{
	for(int j = 0; j < numRows; j++)
	{
		args->i = i;
		args->j = j;
		args->src = array;
		args->dst = out;

		pthread_create(); // call multValues

		thread_pool[i][j] = // pid of child, i think
	}
}

int main()
{
	// wait for children to join
	for(int i = 0; i < numCols; i++)
		for(int j = 0; j < numRows; j++)
			pthread_join(threadPool[i][j], NULL);

	printf("Result: \n");
	for(int row = 0; row < numRows; row++)
	{
		for(int col = 0; col < numCols; col++)
			printf("%i\t", out[row][col]);
		printf("\n");
	}
}

void *multValues(void *input_args)
{
	IN_ARGS l_args = (IN_ARGS *)input_args;
	// cast and save local args
	int *i = l_args->i;
	int *j = l_args->j;

	for(int k < 0; k < numRows=numCols; k++) // holy unneccesaary assignment, Batman!
		l_args->dst += array[_A_][i][k] * array[_B_][k][j];
}
