#define _REENTRANT // make sure library functions are multi-thread safe
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_GENERATED  5000000 // total number of randomly generated floats
#define NUM_THREADS_S1      25 // number of threads in stage one
#define NUM_THREADS_S2       5 // number of threads in stage two

float data[NUM_GENERATED];  // array of floats to sum
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
	//int waiting_for;  // thread to wait for.  Probably not needed.
} THREAD_ARGS;

int main()
{
	pthread_t main_thread;
	pthread_t s1_thread[NUM_THREADS_S1]; // array of stage one threads
	pthread_t s2_thread[NUM_THREADS_S2]; // array of stage two threads
	THREAD_ARGS *t_args;

	main_thread = pthread_self();  // get thread ID for calling thread
	printf("Main thread = %u is creating %d worker threads \n", (int)main_thread, NUM_THREADS_S1);

	printf("NUM_TO_SUM_S1 = %i\n", NUM_TO_SUM_S1);

	/* -------------------- stage one -------------------- */

	printf("----- Creating threads -----\n");
	/* create threads for stage one */
	for(int i = 0; i < NUM_THREADS_S1; i++)
	{

		printf("i = %i\n", i);
		/* allocate memory for the struct that contains each thread's args */
		t_args = (THREAD_ARGS *)malloc(sizeof(THREAD_ARGS));

		/* fill args */
		t_args->threadID   = i;
		t_args->start_pos  = NUM_TO_SUM_S1 * i;
		t_args->num_to_sum = NUM_TO_SUM_S1;

		/* create thread */
		if( (pthread_create( &s1_thread[i], NULL, partial_sum, (void *)t_args)) != 0 )
		{
			printf("Error: could not create thread!  Exiting.\n");
			exit(0);
		}
	}
	printf("Done creating threads\n");

	/* wait for stage one threads to finish */
	for(int i = 0; i < NUM_THREADS_S1; i++)
		pthread_join( s1_thread[i], NULL );

	printf("Stage one complete!\n");

	/* -------------------- stage two -------------------- */

	pthread_exit( (void *)main_thread );
}

void *partial_sum(void *in_args)
{
	THREAD_ARGS *l_args = (THREAD_ARGS *)in_args;

	/* print args */
	printf("Thread %i is using:\n  ID =  \t%i \n  start =\t%i \n  num =  \t%i\n",
		l_args->threadID, l_args->start_pos, l_args->num_to_sum);
}
