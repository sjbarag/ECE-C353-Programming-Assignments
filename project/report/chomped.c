THREAD_ARGS *l_args = (THREAD_ARGS *) args;

queue_element_t *element, *new_element;
struct stat file_stats;
int status;
DIR *directory = NULL;
struct dirent *result = NULL;
/* allocate memory for the directory structure */
struct dirent *entry = (struct dirent *)malloc(sizeof(struct dirent) + MAX_LENGTH);


/* initialize queue if on thread zero */
if( l_args->threadID == 0 )
{
	/* Create and initialize the queue data structure. */
	l_args->queue = create_queue();
	element = (queue_element_t *)malloc(sizeof(queue_element_t));
	if(element == NULL)
	{
		printf("E: Error allocating memory. Exiting. \n");
		exit(-1);
	}
	strcpy(element->path_name, l_args->argv[2]);
	element->next = NULL;
	/* Insert the initial path name into the queue */
	insert_in_queue(l_args->queue, element);
}


while( 1 )
{
	pthread_mutex_lock( l_args->mutex_queue );
	/* While there is work in the queue, process it. */
	if(l_args->queue->head != NULL)
	{
		queue_element_t *element = remove_from_queue(l_args->queue);
		pthread_mutex_unlock( l_args->mutex_queue );


if(result == NULL)
{
	if( l_args->threadID == 0 && !spawned )
	{
		spawned = 1;
		THREAD_ARGS *t_args;
		for( int i = 1; i < MAX_THREADS; i++ )
		{
			t_args = (THREAD_ARGS *)malloc(sizeof(THREAD_ARGS));
			t_args->threadID = i;
			t_args->queue = l_args->queue;
			t_args->mutex_queue = l_args->mutex_queue;
			t_args->mutex_count = l_args->mutex_count;
			t_args->num_occurrences = l_args->num_occurrences;
			/* NULL argv implies it's a worker thread */
			t_args->argv = l_args->argv;

			if( pthread_create( &threads[i], NULL, worker_thread, (void *)t_args ) != 0 )
			{
				printf("E: could not create thread %d! Exiting\n");
				exit(-1);
			}
		}
	}

  	break; // End of directory
}


/* Construct the full path name for the directory item stored in entry. */
strcpy(new_element->path_name, element->path_name);
strcat(new_element->path_name, "/");
strcat(new_element->path_name, entry->d_name);

/* safely add to queue */
pthread_mutex_lock( l_args->mutex_queue );
insert_in_queue(l_args->queue, new_element);
pthread_mutex_unlock( l_args->mutex_queue );

/* wake up others
 * man, pthread_broadcast never works for me */
for( int i = 0; i < MAX_THREADS-1; i++ )
	pthread_cond_signal( &wake_up );


else
{
	pthread_mutex_unlock( l_args->mutex_queue );
	pthread_mutex_lock( &num_sleeping );
	pthread_mutex_lock( &done );

	if( all_done == 1 )
	{
		pthread_mutex_unlock( &done );
		pthread_mutex_unlock( &num_sleeping );
		pthread_exit(0);
	}


else if( number_sleeping == (MAX_THREADS - 1) )
{
	all_done = 1;
	pthread_mutex_unlock( &done );
	pthread_mutex_unlock( &num_sleeping );

	/* wake everyone up
	 * man, pthread_broadcast never works for me */
	for( int i = 0; i < MAX_THREADS-1; i++ )
		pthread_cond_signal( &wake_up );
	pthread_exit( 0 );
}


else
{
	pthread_mutex_unlock( &done );
	/* add to number of sleeping */
	number_sleeping ++;
	pthread_mutex_unlock( &num_sleeping );

	/* sleep */
	pthread_mutex_lock( &sleepers[l_args->threadID] );
	pthread_mutex_lock( &done ); // check done status just before sleeping
	if( all_done == 1 )
	{
		pthread_mutex_unlock( &done );
		pthread_mutex_unlock( &sleepers[l_args->threadID] );
		pthread_exit(0);
	}
	pthread_mutex_unlock( &done );
	pthread_cond_wait( &wake_up, &sleepers[l_args->threadID] );
	pthread_mutex_unlock( &sleepers[l_args->threadID] );
	if( all_done == 1 )
		pthread_exit(0);
	else
	{
		pthread_mutex_lock( &num_sleeping);
		number_sleeping --;
		pthread_mutex_unlock( &num_sleeping );
		continue;
	}
}
