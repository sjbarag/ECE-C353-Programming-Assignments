#define _REENTRANT // make sure library functions are multi-thread safe
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define NUM_GENERATED  5000000  // total number of randomly generated floats
#define NUM_THREADS_S1      25  // number of threads in stage one
#define NUM_THREADS_S2       5  // number of threads in stage two

float raw_data[NUM_GENERATED];  // raw array of floats to sum
float data_s1[NUM_THREADS_S1];  // array of partial sums from s1
float data_s2[NUM_THREADS_S2];  // array of partial sums from s2
int NUM_TO_SUM_S1 = NUM_GENERATED / NUM_THREADS_S1;  // number of elements for
													 // each thread to sum in
													 // stage one
int NUM_TO_SUM_S2 = NUM_THREADS_S1 / NUM_THREADS_S2; // number of elements for
													 // each thread to sum in
													 // stage two

/* prototype for the thread routines */
void *partial_sum(void *);

typedef struct thread_args
{
	int threadID;     // thread ID of the current thread
	int start_pos;    // position at which the thread should start adding
	                  // values
	int num_to_sum;   // number of values for this thread to sum
	float *src;       // pointer to array of data to sum
	float *dst;       // pointer to array to place sum
} THREAD_ARGS;

int main()
{
	pthread_t main_thread;
	pthread_t s1_thread[NUM_THREADS_S1]; // array of stage one threads
	pthread_t s2_thread[NUM_THREADS_S2]; // array of stage two threads
	THREAD_ARGS *t_args;                 // structure to hold the arguments for
	                                     // each thread
	float t_sum = 0;                     // final sum via threads
	float long_sum = 0;                  // bruteforce sum


	/* seed random number generator */
	srandom(time(NULL));

	/* generate 5000000 floats and calculate sum */
	printf("Generating random numbers between 0 and 1...\n\n");
	for(int i = 0; i < NUM_GENERATED; i++)
	{
		raw_data[i] = (float)random()/(float)RAND_MAX;
		long_sum += raw_data[i];
	}


	main_thread = pthread_self();  // get thread ID for calling thread
	/* -------------------- stage one -------------------- */

	printf("Main thread = %u is creating %d worker threads for stage 1.\n", (int)main_thread, NUM_THREADS_S1);

	/* build args and create threads for stage one */
	for(int i = 0; i < NUM_THREADS_S1; i++)
	{
		/* allocate memory for the struct that contains each thread's args */
		t_args = (THREAD_ARGS *)malloc(sizeof(THREAD_ARGS));

		/* fill args */
		t_args->threadID   = i;
		t_args->start_pos  = NUM_TO_SUM_S1 * i;
		t_args->num_to_sum = NUM_TO_SUM_S1;
		t_args->src        = raw_data;
		t_args->dst        = data_s1;

		/* create thread */
		if( (pthread_create( &s1_thread[i], NULL, partial_sum, (void *)t_args)) != 0 )
		{
			printf("Error: could not create thread!  Exiting.\n");
			exit(0);
		}
	}

	/* wait for stage one threads to finish */
	for(int i = 0; i < NUM_THREADS_S1; i++)
		pthread_join( s1_thread[i], NULL );


	/* -------------------- stage two -------------------- */

	printf("Main thread = %u is creating %d worker threads for stage 2.\n", (int)main_thread, NUM_THREADS_S2);

	/* build args and create threads for stage one */
	for(int i = 0; i < NUM_THREADS_S2; i++)
	{
		/* allocate memory for the struct that contains each thread's args */
		t_args = (THREAD_ARGS *)malloc(sizeof(THREAD_ARGS));

		/* fill args */
		t_args->threadID   = i;
		t_args->start_pos  = NUM_TO_SUM_S2 * i;
		t_args->num_to_sum = NUM_TO_SUM_S2;
		t_args->src        = data_s1;
		t_args->dst        = data_s2;

		/* create thread */
		if( (pthread_create( &s2_thread[i], NULL, partial_sum, (void *)t_args)) != 0 )
		{
			printf("Error: could not create thread!  Exiting.\n");
			exit(0);
		}
	}

	/* wait for stage two threads to finish */
	for(int i = 0; i < NUM_THREADS_S2; i++)
		pthread_join( s2_thread[i], NULL );


	/* -------------------- stage three -------------------- */

	/* calculate final sum */
	for(int i = 0; i < NUM_THREADS_S2; i++)
		t_sum += data_s2[i];


	/* print results */
	printf("------------------------------------\n");
	printf("Sum via bruteforce = \t%f\n", long_sum);
	printf("Sum via threading  = \t%f\n", t_sum);


	/* exit main.  we're done! */
	pthread_exit( (void *)main_thread );
}

void *partial_sum(void *in_args)
{
	THREAD_ARGS *l_args = (THREAD_ARGS *)in_args;

	for(int i = l_args->start_pos; i < (l_args->start_pos + l_args->num_to_sum); i++)
		l_args->dst[ l_args->threadID ] += l_args->src[ i ];
}
