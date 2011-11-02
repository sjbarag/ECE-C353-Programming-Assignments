/*
	 This code illustrates the sleeping barber problem. Compile as
	 follows:
	 gcc -o sleeping_barber -std=c99 sleeping_barber.c -lpthread -lm -lrt

	 Usage:
	 sleeping_barber <customers> <chairs>
	 Example: sleeping_barber 10 5; means: there are 10 customers and 5 waiting chairs in the
	 barber shop

*/

#define _REENTRANT
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

#define TRUE 1
#define FALSE 0
#define MIN_TIME 4
#define MAX_TIME 8

#define MAX_NUM_CUSTOMERS 50

void *customer(void *num); // Prototype for customer thread
void *barber(void *); // Prototype of barber thread
int UD(int, int); // Random number generator
int num_waiting_chairs; // number of empty seats in waiting room
int barber_sleeping;

/* definition of conditions */
pthread_cond_t seats_available; /* seats in the waiting room are available */
pthread_cond_t barber_free;     /* the barber is not currently cutting hair */
pthread_cond_t wake_up;	        /* sent to wake the barber up */
pthread_cond_t done_cut;        /* tells the current customer that the haircut
                                   is finished*/

/* definition of mutexes */
pthread_mutex_t barber_chair;   /* the barber's chair */
pthread_mutex_t number_seats;   /* mutex protecting the number of seats */
pthread_mutex_t bed;            /* the barber's bed.
                                   there's no contention for this, so probably
                                   unneeded. */
pthread_mutex_t finished;       /* finished customer */
pthread_mutex_t barber_state;   /* protects bool of barber's state */

int done_with_all_customers = FALSE; // Flag indicating the barber can go home

int main(int argc, char **argv){

	if(argc < 3){
		printf("Usage: sleeping_barber <customers> <chairs> \n");
		exit(0);
	}

	int num_customers = atoi(argv[1]);  // Number of customers
	num_waiting_chairs = atoi(argv[2]); // Number of waiting chairs in the barber shop

	srand((long)time(NULL)); /* Initialize randomizer */

	if(num_customers > MAX_NUM_CUSTOMERS){
		printf("Number of customers exceeds the maximum capacity of the barber \n");
		printf("Resetting the number of customers to %d \n", (int)MAX_NUM_CUSTOMERS);
		num_customers = MAX_NUM_CUSTOMERS;
	}

	/* initialize the conditions */
	pthread_cond_init( &seats_available, NULL );
	pthread_cond_init( &barber_free, NULL );
	pthread_cond_init( &wake_up, NULL );
	pthread_cond_init( &done_cut, NULL );

	/* initialize the mutexes */
	pthread_mutex_init( &barber_chair, NULL );
	pthread_mutex_init( &number_seats, NULL );
	pthread_mutex_init( &bed, NULL );
	pthread_mutex_init( &finished, NULL );
	pthread_mutex_init( &barber_state, NULL );

	pthread_t btid; // ID for the barber thread
	pthread_t tid[MAX_NUM_CUSTOMERS]; // IDs for customer threads

	/* Create barber thread */
	pthread_create(&btid, 0, barber, 0);

	/* Create customer threads and give each an ID */
	int customer_ID[MAX_NUM_CUSTOMERS]; // Customer IDs
	int i;
	for(i = 0; i < num_customers; i++){
		customer_ID[i] = i;
		pthread_create(&tid[i], 0, customer, &customer_ID[i]);
	}

	for(i = 0; i < num_customers; i++)
		pthread_join(tid[i], 0);

	done_with_all_customers = TRUE;
	pthread_cond_signal( &wake_up ); // wake up barber
	pthread_join(btid, 0);
}

void *barber(void *arg){
	pthread_mutex_lock( &barber_state);
	barber_sleeping = TRUE;
	pthread_mutex_unlock( &barber_state);


	while(!done_with_all_customers){ // Customers remain to be serviced
	printf("Barber: Sleeping \n");
		pthread_mutex_lock( &barber_state );
		pthread_cond_wait( &wake_up, &barber_state ); // wait for someone to wake him up
		barber_sleeping = FALSE;
		pthread_mutex_unlock( &barber_state );

		if(!done_with_all_customers){
			printf("Barber: Cutting hair \n");
			int waitTime = UD(MIN_TIME, MAX_TIME); // Simulate cutting hair
			sleep(waitTime);
			printf("Barber: Done cut \n");
			pthread_cond_signal( &done_cut ); // tell customer to get up and get out
			pthread_mutex_lock( &barber_state );	 // lock bed again
			barber_sleeping = TRUE;
			pthread_mutex_unlock( &barber_state );
		}
		else{
			printf("Barber: Done for the day. Going home \n");
		}
	}
}

void *customer(void *customerNumber){
	int number = *(int *)customerNumber;
	printf("Customer %d: Leaving for the barber shop \n", number);
	int waitTime = UD(MIN_TIME, MAX_TIME); // Simulate going to the barber shop
	sleep(waitTime);
	printf("Customer %d: Arrived at the barber shop \n", number);



	/* check number of seats */
	pthread_mutex_lock( &number_seats ); // lock number of seats
	if( num_waiting_chairs == 0 ) // if no seats available
		pthread_cond_wait( &seats_available, &number_seats ); // wait for a free seat
	printf("Customer %d: Entering waiting room \n", number);

	/* take one seat */
	num_waiting_chairs--;
	pthread_mutex_unlock( &number_seats ); // unlock mutex so others can check seats

	/* --- customer now has a seat --- */

	/* check barber's state */
	pthread_mutex_lock( &barber_state );
	printf("Customer %d: Checking barber state \n", number);
	if( barber_sleeping )
	{
		printf("Customer %d: He sleepin, yo \n", number);
		/* check number of seats */
		pthread_mutex_lock( &number_seats );
		if( num_waiting_chairs == 0 )
			pthread_cond_broadcast( &seats_available ); // tell outsiders only if they're waiting

		/* free your seat */
		num_waiting_chairs++;
		pthread_mutex_unlock( &number_seats ); // unlock mutex so others can check seats


		printf("Customer %d: Waking barber! \n", number);
		pthread_cond_signal( &wake_up ); // wake him up!
		pthread_mutex_unlock( &barber_state ); // unlock so barber can change his state
		printf("Customer %d: Taking chair \n", number);
		pthread_mutex_lock( &barber_chair ); // take barber chair
		printf("Customer %d: Chair taken \n", number);
	}
	else
	{
		pthread_mutex_unlock( &barber_state );
		pthread_mutex_lock( &barber_chair );
		pthread_cond_wait( &barber_free, &barber_chair ); // wait for free barber
	}

	/* check number of seats */
	printf("Customer %d: giving up seat \n", number);
	pthread_mutex_lock( &number_seats );
	if( num_waiting_chairs == 0 )
	{
		pthread_cond_broadcast( &seats_available ); // tell outsiders only if they're waiting
		printf("Customer %d: told outsiders \n", number);
	}

	/* free your seat */
	num_waiting_chairs++;
	printf("Customer %d: seat relinquished \n", number);
	pthread_mutex_unlock( &number_seats ); // unlock mutex so others can check seats

	/* --- customer now has the barber's chair --- */
	printf("Customer %d: Getting cut \n", number);
	pthread_mutex_lock( &finished );
	pthread_cond_wait( &done_cut, &finished ); // wait for cut to finish
	pthread_mutex_unlock( &finished ); // finished isn't really needed, so dump it
	printf("Customer %d: Going home \n", number);

	pthread_cond_broadcast( &barber_free ); // tell others barber is free
	pthread_mutex_unlock( &barber_chair ); // unlock mutex so someone can get service
}


/* Returns a random number between min and max */
int UD(int min, int max){
	return((int)floor((double)(min + (max - min + 1)*((float)rand()/(float)RAND_MAX))));
}
