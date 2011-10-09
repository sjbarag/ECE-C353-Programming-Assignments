#define _REENTRANT // make sure library functions are multi-thread safe
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define NUM_GENERATED  500 // total number of randomly generated floats
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
	float long_sum = 0;

	/* seed random number generator */
	srandom(time(NULL));

	/* generate 5000000 floats and calculate sum */
	printf("Generating random numbers between 0 and 1\n");
	for(int i = 0; i < NUM_GENERATED; i++)
	{
		data[i] = (float)random()/(float)RAND_MAX;
		long_sum += data[i];
	}


	pthread_t main_thread;
	pthread_t s1_thread[NUM_THREADS_S1]; // array of stage one threads
	pthread_t s2_thread[NUM_THREADS_S2]; // array of stage two threads
	THREAD_ARGS *t_args;                 // structure to hold the arguments for
	                                     // each thread
	float t_sum = 0;                     // final sum via threads

	float dbg_sum = 0;

	main_thread = pthread_self();  // get thread ID for calling thread
	printf("Main thread = %u is creating %d worker threads \n", (int)main_thread, NUM_THREADS_S1);


	/* -------------------- stage one -------------------- */
	printf("First 20:\n[ ");
	for(int i = 0; i < 20; i++)
	{
		printf("%f, ", data[i]);
		dbg_sum += data[i];
	}
	printf(" ]\n");
	printf("First 20 sum = %f", dbg_sum);

	printf("----- Creating threads -----\n");
	/* create threads for stage one */
	for(int i = 0; i < NUM_THREADS_S1; i++)
	{

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
	printf("First 20:\n[ ");
	for(int i = 0; i < 20; i++)
		printf("%f, ", data[i]);
	printf(" ]\n");

	printf("----- Creating threads -----\n");
	/* create threads for stage one */
	for(int i = 0; i < NUM_THREADS_S2; i++)
	{

		/* allocate memory for the struct that contains each thread's args */
		t_args = (THREAD_ARGS *)malloc(sizeof(THREAD_ARGS));

		/* fill args */
		t_args->threadID   = i;
		t_args->start_pos  = NUM_TO_SUM_S2 * i;
		t_args->num_to_sum = NUM_TO_SUM_S2;

		/* create thread */
		if( (pthread_create( &s2_thread[i], NULL, partial_sum, (void *)t_args)) != 0 )
		{
			printf("Error: could not create thread!  Exiting.\n");
			exit(0);
		}
	}
	printf("Done creating threads\n");

	/* wait for stage one threads to finish */
	for(int i = 0; i < NUM_THREADS_S2; i++)
		pthread_join( s2_thread[i], NULL );

	printf("Stage two complete!\n");

	/* -------------------- stage three -------------------- */
	for(int i = 0; i < 5; i++)
		t_sum += data[i];

	printf("First 20:\n[ ");
	for(int i = 0; i < 20; i++)
		printf("%f, ", data[i]);
	printf(" ]\n");

	printf(" ------------------------------------\n");
	printf("Sum via bruteforce = \t%f\n", long_sum);
	printf("Sum via threading  = \t%f\n", t_sum);

	pthread_exit( (void *)main_thread );
}

void *partial_sum(void *in_args)
{
	float temp_sum = 0.0;
	THREAD_ARGS *l_args = (THREAD_ARGS *)in_args;

	/* print args */
	//printf("Thread %i is using:\n  ID =  \t%i \n  start =\t%i \n  num =  \t%i\n",
	//	l_args->threadID, l_args->threadID, l_args->start_pos, l_args->num_to_sum);

	for(int i = l_args->start_pos; i < (l_args->start_pos + l_args->num_to_sum); i++)
		temp_sum = (float)temp_sum + (float)data[i];
	printf("ThreadID: %i\ntemp_sum = %f\nnum_summed = %i\n\n", l_args->threadID, temp_sum, l_args->num_to_sum);
	data[l_args->threadID] = (float)temp_sum;
}
