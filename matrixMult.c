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


typedef struct thread_args
{
	int i;
	int j;
	int **srcA;
	int **srcB;
	int **dst;
} THREAD_ARGS;

void *multValues(void *);
int g_numRows;
int *outArray;

int main()
{

	FILE *input;
	char lineData[64];
	char *line = lineData;
	int numRows;
	int numCols;
//	char *line = (char *)malloc(sizeof(char)*64);
	input = fopen("input_data.txt", "r");
	fgets(line, 24, input);
	line = strtok(line, "\n"); // strip newline
	line = strtok(line, "ROWS ");
	numRows = atoi(line);
	g_numRows = numRows;

	//fscanf(input, "ROWS %i", &numRows);
	numCols = numRows;
	printf("numRows: %i\n", numRows);
	fgets(line, 64, input); // columns


	int **A = (int **)malloc(numRows * sizeof(int *));
	for(int i = 0; i < numRows; i ++)
		A[i] = (int *)malloc(numCols * sizeof(int));

	int **B = (int **)malloc(numRows * sizeof(int *));
	for(int i = 0; i < numRows; i ++)
		B[i] = (int *)malloc(numCols * sizeof(int));

	int **out = (int **)malloc(numRows * sizeof(int *));
	for(int i = 0; i < numRows; i ++)
		out[i] = (int *)malloc(numCols * sizeof(int));

	//int *p_data = data;
	char *val;
	fgets(line, 64, input); // handle blankline

	pthread_t thread_pool[numRows][numCols];

	THREAD_ARGS *t_args;

	// do it once for A and B.  Too lazy to copy paste this, so I'll carry arround a 3-dimensional array...
	for(int row=0; row < numRows; row++)
	{
		fgets(line, numRows*2+1, input);
		for(int col=0; col < numCols; col++) {
			if( col == 0)
			{
				val = strtok(line, " ");
				A[row][col] = atoi(val);
			}
			else
			{
				val = strtok(NULL, " ");
				A[row][col] = atoi(val);
			}
		}
	}

	fgets(line, numRows*2+1, input); // handle blankline

	for(int row=0; row < numRows; row++)
	{
		fgets(line, numRows*2+1, input);
		for(int col=0; col < numCols; col++)
		{
			if( col == 0)
			{
				val = strtok(line, " ");
				B[row][col] = atoi(val);
			}
			else
			{
				val = strtok(NULL, " ");
				B[row][col] = atoi(val);
			}
		}
	}




	/* print input */
	printf("Read Data \n");
	for(int row = 0; row < numRows; row++)
	{
		for(int col = 0; col < numCols; col++)
			printf("%i\t", A[row][col]);
		printf("\n");
	}

	for(int row = 0; row < numRows; row++)
	{
		for(int col = 0; col < numCols; col++)
			printf("%i\t", B[row][col]);
		printf("\n");
	}


	/* spawn threads */
	for(int i = 0; i < numRows; i++)
	{
		for(int j = 0; j < numCols; j++)
		{
			t_args = (THREAD_ARGS *)malloc(sizeof(THREAD_ARGS));

			t_args->i = i;
			t_args->j = j;
			t_args->srcA = A;
			t_args->srcB = B;
			t_args->dst = out;

			if( pthread_create( &thread_pool[i][j], NULL, multValues, (void *)t_args) != 0)
			{
				printf("Error: could not create thread! Exiting.\n");
				exit(0);
			}
		}
	}

	for( int i = 0; i < numRows; i++)
		for( int j = 0; j < numCols; j++)
			pthread_join( thread_pool[i][j], NULL );

	printf("Output:\n");
	for(int row = 0; row < numRows; row++)
	{
		for(int col = 0; col < numCols; col++)
			printf("%i\t", out[row][col]);
		printf("\n");
	}
}

void *multValues(void *input_args)
{
	THREAD_ARGS *l_args = (THREAD_ARGS *)input_args;

	printf("Running\n");
	// cast and save local args
	int i = l_args->i;
	int j = l_args->j;

	for(int k = 0; k < g_numRows; k++)
		l_args->dst[i][j] += l_args->srcA[i][k] * l_args->srcB[k][j];
}
