#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
// for threads
#include <pthread.h>
// for file manipulation
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* structure for thread arguments */
typedef struct thread_args
{
	int i;
	int j;
	int **srcA;
	int **srcB;
	int **dst;
} THREAD_ARGS;

void *multValues(void *);  /* process to run for each thread */
int g_numRows;             /* global version of number of rows */
int *outArray;             /* pointer to where the final values will go */

int main()
{
	FILE *input;           /* file descriptor for the input file */
	char lineData[64];     /* each line gets stored here */
	char *line = lineData;
	int numRows;           /* number of rows in the matrix, read from 'input' */
	int numCols;           /* number of columns.  Equal to numCols, not read */

	input = fopen("input_data.txt", "r");  /* open the input file */
	fgets(line, 24, input);                /* get the first line */
	line = strtok(line, "\n");             /* strip the newline */
	line = strtok(line, "ROWS ");          /* isolate just the value */
	numRows = atoi(line);                  /* convert to int, yo */
	g_numRows = numRows;                   /* there's a global version too */

	numCols = numRows;                     /* NxN matrix, so make them equal */
	fgets(line, 64, input);                /* read the line for columns, but ignore it */


	/* allocate two-dimensional array for matrix A */
	int **A = (int **)malloc(numRows * sizeof(int *));
	for(int i = 0; i < numRows; i ++)
		A[i] = (int *)malloc(numCols * sizeof(int));

	/* allocate two-dimensional array for matrix B */
	int **B = (int **)malloc(numRows * sizeof(int *));
	for(int i = 0; i < numRows; i ++)
		B[i] = (int *)malloc(numCols * sizeof(int));

	/* allocate two-dimensional array for output matrix */
	int **out = (int **)malloc(numRows * sizeof(int *));
	for(int i = 0; i < numRows; i ++)
		out[i] = (int *)malloc(numCols * sizeof(int));


	fgets(line, 64, input); // handle blankline

	pthread_t thread_pool[numRows][numCols];

	THREAD_ARGS *t_args;

	/* read data for matrix A elements */
	for(int row=0; row < numRows; row++)
	{
		fgets(line, numRows*2+1, input);
		for(int col=0; col < numCols; col++)
		{
			if( col == 0)
				A[row][col] = atoi(strtok(line, " "));
			else
				A[row][col] = atoi(strtok(NULL, " "));
		}
	}

	/* ignore blank line between matrices */
	fgets(line, numRows*2+1, input);

	/* read data for matrix B elements */
	for(int row=0; row < numRows; row++)
	{
		fgets(line, numRows*2+1, input);
		for(int col=0; col < numCols; col++)
		{
			if( col == 0)
				B[row][col] = atoi(strtok(line, " "));
			else
				B[row][col] = atoi(strtok(NULL, " "));
		}
	}


	/* spawn threads */
	for(int i = 0; i < numRows; i++)
	{
		for(int j = 0; j < numCols; j++)
		{
			/* allocate new args structure */
			t_args = (THREAD_ARGS *)malloc(sizeof(THREAD_ARGS));

			/* fill args */
			t_args->i = i;
			t_args->j = j;
			t_args->srcA = A;
			t_args->srcB = B;
			t_args->dst = out;

			/* create thread */
			if( pthread_create( &thread_pool[i][j], NULL, multValues, (void *)t_args) != 0)
			{
				printf("Error: could not create thread! Exiting.\n");
				exit(0);
			}
		}
	}

	/* wait for all threads to return */
	for( int i = 0; i < numRows; i++)
		for( int j = 0; j < numCols; j++)
			pthread_join( thread_pool[i][j], NULL );

	/* print final result */
	printf("Product:\n");
	for(int row = 0; row < numRows; row++)
	{
		for(int col = 0; col < numCols; col++)
			printf("%i\t", out[row][col]);
		printf("\n");
	}
}

void *multValues(void *input_args)
{
	/* cast to usable datatype */
	THREAD_ARGS *l_args = (THREAD_ARGS *)input_args;

	/* these is just shorter to write */
	int i = l_args->i;
	int j = l_args->j;

	/* calculate values */
	for(int k = 0; k < g_numRows; k++)
		l_args->dst[i][j] += l_args->srcA[i][k] * l_args->srcB[k][j];

	/* peace out */
	pthread_exit(0);
}
