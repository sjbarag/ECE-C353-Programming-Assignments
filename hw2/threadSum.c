#define _REENTRANT    /* compatibility, yo */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_GENERATED 5000000    /* number of random numbers to generate */
#define NUM_THREADS_S1     25    /* number of threads used in stage one  */
#define NUM_THREADS_S2      5    /* number of threads used in stage two  */

#define NUM_TO_SUM_S1  200000    /* number of elements each thread sums in s1 */
#define NUM_TO_SUM_S2       5    /* number of elements each thread sums in s2 */

void *partial_sum(void *);

/* structure for thread arguments */
typedef struct thread_args
{
	int threadID;
	int start;
	int count;

	double *src;
	double *dst;
} THREAD_ARGS;

int main()
{
	pthread_t main_thread;                    /* threadID for main thread */
	pthread_t s1_thread[NUM_THREADS_S1];      /* array of threadIDs for stage one */
	pthread_t s2_thread[NUM_THREADS_S2];      /* array of threadIDs for stage two */

	/* arrays to hold the output of stages 0..2 */
	double *data_s0 = (double *)malloc(NUM_GENERATED * sizeof(double));
	double *data_s1 = (double *)malloc(NUM_THREADS_S1 * sizeof(double));
	double *data_s2 = (double *)malloc(NUM_THREADS_S2 * sizeof(double));

	THREAD_ARGS *t_args;  /* reusable thread arguments structure */

	double t_sum = 0;     /* sum calculated by threads */
	double it_sum = 0;    /* sum calculated by iteration */

	/* seed random generator with current time */
	srandom(time(NULL));


	/* --------------------- stage 0 --------------------- */
	printf("Generating random numbers between 0 and 1...\n\n");
	for(int i = 0; i < NUM_GENERATED; i++)
		data_s0[i] = (double)random() / RAND_MAX;

	main_thread = pthread_self();

	for(int i = 0; i < NUM_GENERATED; i++)
		it_sum += data_s0[i];


	/* --------------------- stage 1 --------------------- */
	for(int i = 0; i < NUM_THREADS_S1; i++)
	{
		/* allocate new args structure */
		t_args = (THREAD_ARGS *)malloc(sizeof(THREAD_ARGS));

		/* fill args */
		t_args->threadID = i;
		t_args->start = NUM_TO_SUM_S1 * i;
		t_args->count = NUM_TO_SUM_S1;
		t_args->src   = data_s0;
		t_args->dst   = data_s1;

		/* create thread */
		if( (pthread_create( &s1_thread[i], NULL, partial_sum, (void *)t_args)) != 0)
		{
			printf("Error: could not create thread! Exiting.\n");
			exit(-1);
		}
	}

	/* wait for all threads to return */
	for(int i = 0; i < NUM_THREADS_S1; i++)
		pthread_join( s1_thread[i], NULL );


	/* --------------------- stage 2 --------------------- */
	for(int i = 0; i < NUM_THREADS_S2; i++)
	{
		/* allocate new args structure */
		t_args = (THREAD_ARGS *)malloc(sizeof(THREAD_ARGS));

		/* fill args */
		t_args->threadID = i;
		t_args->start = NUM_TO_SUM_S2 * i;
		t_args->count = NUM_TO_SUM_S2;
		t_args->src   = data_s1;
		t_args->dst   = data_s2;

		/* create thread */
		if( (pthread_create( &s2_thread[i], NULL, partial_sum, (void *)t_args)) != 0)
		{
			printf("Error: could not create thread! Exiting.\n");
			exit(-1);
		}
	}

	/* wait for all threads to return */
	for(int i = 0; i < NUM_THREADS_S2; i++)
		pthread_join( s2_thread[i], NULL );


	/* --------------------- stage 3 --------------------- */
	/* sum last five partial sums manualls */
	for(int i = 0; i < NUM_THREADS_S2; i++)
		t_sum += data_s2[i];

	/* display output */
	printf("Iterated sum = \t%f\n", it_sum);
	printf("Final sum =    \t%f\n", t_sum);

	/* exit gracefully */
	pthread_exit( (void *)main_thread);;
}

void *partial_sum(void *in_args)
{
	/* cast to usable datatype */
	THREAD_ARGS *l_args = (THREAD_ARGS *)in_args;

	double l_sum = 0; /* local sum */

	/* sum appropriate values */
	for(int i = l_args->start; i < (l_args->count + l_args->start); i++)
		l_sum = l_sum + l_args->src[i];

	/* save sum */
	l_args->dst[ l_args->threadID ] = l_sum;

	/* peace out */
	pthread_exit(0);
}
