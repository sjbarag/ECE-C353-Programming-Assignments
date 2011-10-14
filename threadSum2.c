#define _REENTRANT
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_GENERATED 5000000
#define NUM_THREADS_S1     25
#define NUM_THREADS_S2      5


void *partial_sum(void *);

typedef struct thread_args
{
	int threadID;
	int start;
	int count;

	float *src;
	float *dst;
} THREAD_ARGS;

int main()
{
	pthread_t main_thread;
	pthread_t s1_thread[NUM_THREADS_S1];
	pthread_t s2_thread[NUM_THREADS_S2];

	float *data_s0 = (float *)malloc(NUM_GENERATED * sizeof(float));
	float *data_s1 = (float *)malloc(NUM_THREADS_S1 * sizeof(float));
	float *data_s2 = (float *)malloc(NUM_THREADS_S2 * sizeof(float));

	THREAD_ARGS *t_args;

	float t_sum = 0;
	float it_sum = 0;

	srandom(time(NULL));


	/* stage 0 */
	printf("Generatinig random numbers between 0 and 1...\n\n");
	for(int i = 0; i < NUM_GENERATED; i++)
		data_s0[i] = (float)i;

	main_thread = pthread_self();

	for(int i = 0; i < NUM_GENERATED; i++)
		it_sum += data_s0[i];

	printf("Iterated sum = %.1f\n", it_sum);

	exit(0);

	/* stage 1 */
}

