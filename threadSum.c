#define _REENTRANT
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_GENERATED 5000000
#define NUM_THREADS_S1     25
#define NUM_THREADS_S2      5

#define NUM_TO_SUM_S1  200000
#define NUM_TO_SUM_S2       5


void *partial_sum(void *);

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
	pthread_t main_thread;
	pthread_t s1_thread[NUM_THREADS_S1];
	pthread_t s2_thread[NUM_THREADS_S2];

	double *data_s0 = (double *)malloc(NUM_GENERATED * sizeof(double));
	double *data_s1 = (double *)malloc(NUM_THREADS_S1 * sizeof(double));
	double *data_s2 = (double *)malloc(NUM_THREADS_S2 * sizeof(double));

	THREAD_ARGS *t_args;

	double t_sum = 0;
	double it_sum = 0;

	srandom(time(NULL));


	/* stage 0 */
	printf("Generating random numbers between 0 and 1...\n\n");
	for(int i = 0; i < NUM_GENERATED; i++)
		data_s0[i] = (double)random() / RAND_MAX;

	main_thread = pthread_self();

	for(int i = 0; i < NUM_GENERATED; i++)
		it_sum += data_s0[i];


	/* stage 1 */
	for(int i = 0; i < NUM_THREADS_S1; i++)
	{
		t_args = (THREAD_ARGS *)malloc(sizeof(THREAD_ARGS));

		t_args->threadID = i;
		t_args->start = NUM_TO_SUM_S1 * i;
		t_args->count = NUM_TO_SUM_S1;
		t_args->src   = data_s0;
		t_args->dst   = data_s1;

		if( (pthread_create( &s1_thread[i], NULL, partial_sum, (void *)t_args)) != 0)
		{
			printf("Error: could not create thread! Exiting.\n");
			exit(-1);
		}
	}

	/* wait */
	for(int i = 0; i < NUM_THREADS_S1; i++)
		pthread_join( s1_thread[i], NULL );


	/* stage 2 */
	for(int i = 0; i < NUM_THREADS_S2; i++)
	{
		t_args = (THREAD_ARGS *)malloc(sizeof(THREAD_ARGS));

		t_args->threadID = i;
		t_args->start = NUM_TO_SUM_S2 * i;
		t_args->count = NUM_TO_SUM_S2;
		t_args->src   = data_s1;
		t_args->dst   = data_s2;

		if( (pthread_create( &s2_thread[i], NULL, partial_sum, (void *)t_args)) != 0)
		{
			printf("Error: could not create thread! Exiting.\n");
			exit(-1);
		}
	}

	/* wait */
	for(int i = 0; i < NUM_THREADS_S2; i++)
		pthread_join( s2_thread[i], NULL );


	/* stage 3 */
	for(int i = 0; i < NUM_THREADS_S2; i++)
		t_sum += data_s2[i];

	printf("Iterated sum = \t%f\n", it_sum);
	printf("Final sum =    \t%f\n", t_sum);


	pthread_exit( (void *)main_thread);;
}

void *partial_sum(void *in_args)
{
	THREAD_ARGS *l_args = (THREAD_ARGS *)in_args;
	double val = 0;

	for(int i = l_args->start; i < (l_args->count + l_args->start); i++)
		val = val + l_args->src[i];
	l_args->dst[ l_args->threadID ] = val;
	pthread_exit(0);
}
