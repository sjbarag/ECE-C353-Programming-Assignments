int NUM_THREADS;
int num_threads;
int number_sleeping = 0;
int all_done = 0;
int spawned = 0;
pthread_t *threads;
//pthread_t threads[NUM_THREADS];

pthread_cond_t wake_up;
pthread_mutex_t num_sleeping;
pthread_mutex_t *sleepers;
pthread_mutex_t done;

/* structure for thread arguments */
typedef struct thread_args
{
	int threadID;

	pthread_mutex_t *mutex_queue;
	pthread_mutex_t *mutex_count;
	queue_t *queue;

	int *num_occurences;

	char **argv;

} THREAD_ARGS;

typedef struct return_type
{
	int count;
	float time;
} RET_TYPE;

// Function prototypes
RET_TYPE* search_for_string_serial(char **);
RET_TYPE* search_for_string_mt(char **);
void *worker_thread( void * );

