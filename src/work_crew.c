/* Reference code that searches files for a user-specified string.
 * Author: Naga Kandasamy Shackleford
 * Date: 10/13/2011
 *
 * Compile the code as follows: gcc -o work_crew work_crew.c queue_utils.c -std=c99
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <pthread.h>
#include "queue.h"


#include "work_crew.h"


int main(int argc, char** argv)
{
	if(argc < 3)
	{
		printf("Usage: %s <search string> <path> \n", argv[0]);
		exit(0);
	}

	// Perform a serial search of the file system
	RET_TYPE *a = search_for_string_serial(argv);

	// Perform a multi-threaded search of the file system
	RET_TYPE *b = search_for_string_mt(argv);

	printf("\n\n\n");
	printf("                  \tSingle\t\tMulti\t\tMatch\n");
	printf("Scriptable output:\t%f\t%f\t%s\n", a->time, b->time, (a->count == b->count) ? "true" : "false");
	exit(0);
}

/* Given a search string, the function performs a single-threaded or serial
 * search of the file system starting from the specified path name. */
RET_TYPE* search_for_string_serial(char **argv)
{
		int num_occurences = 0;
		queue_element_t *element, *new_element;
		struct stat file_stats;
		int status;
		DIR *directory = NULL;
		struct dirent *result = NULL;
		struct dirent *entry = (struct dirent *)malloc(sizeof(struct dirent) + MAX_LENGTH); // Allocate memory for the directory structure

		/* Create and initialize the queue data structure. */
		queue_t *queue = create_queue();
		element = (queue_element_t *)malloc(sizeof(queue_element_t));
		if(element == NULL)
		{
			printf("Error allocating memory. Exiting. \n");
			exit(-1);
		}
		strcpy(element->path_name, argv[2]);
		element->next = NULL;
		insert_in_queue(queue, element); // Insert the initial path name into the queue

		/* Start the timer here. */
		struct timeval start;
		gettimeofday(&start, NULL);

		/* While there is work in the queue, process it. */
		while(queue->head != NULL)
		{
			queue_element_t *element = remove_from_queue(queue);
			status = lstat(element->path_name, &file_stats); // Obtain information about the file pointed to by path_name
			if(status == -1){
				printf("Error obtaining stats for %s \n", element->path_name);
				free((void *)element);
				continue;
			}
			if(S_ISLNK(file_stats.st_mode)){ // Check if the file is a symbolic link; if so ignore it
			}
			else if(S_ISDIR(file_stats.st_mode))
			{ // Check if file is a directory; if so descend into it and post work to the queue
				printf("%s is a directory. \n", element->path_name);
				directory = opendir(element->path_name);
				if(directory == NULL)
				{
					printf("Unable to open directory %s \n", element->path_name);
					continue;
				}
				while(1)
				{
					status = readdir_r(directory, entry, &result); // Store the directory item in the entry data structure; if result == NULL, we have reached the end of the directory
					if(status != 0)
					{
					  		 printf("Unable to read directory %s \n", element->path_name);
					  		 break;
					}
					if(result == NULL)
					  		 break; // End of directory
					/* Ignore the "." and ".." entries. */
					if(strcmp(entry->d_name, ".") == 0)
					  		 continue;
					if(strcmp(entry->d_name, "..") == 0)
					  		 continue;

					/* Insert this directory entry in the queue. */
					new_element = (queue_element_t *)malloc(sizeof(queue_element_t));
					if(new_element == NULL)
					{
						printf("Error allocating memory. Exiting. \n");
						exit(-1);
					}
					/* Construct the full path name for the directory item stored in entry. */
					strcpy(new_element->path_name, element->path_name);
					strcat(new_element->path_name, "/");
					strcat(new_element->path_name, entry->d_name);
					insert_in_queue(queue, new_element);
				}
				closedir(directory);
			}
			else if(S_ISREG(file_stats.st_mode))// Check if file is a regular file
			{
			 	printf("%s is a regular file. \n", element->path_name);
			 	FILE *file_to_search;
			 	char buffer[MAX_LENGTH];
			 	char *bufptr, *searchptr;

			 	/* Search the file for the search string provided as the command-line argument. */
			 	file_to_search = fopen(element->path_name, "r");
			 	if(file_to_search == NULL)
				{
			 		printf("Unable to open file %s \n", element->path_name);
			 		continue;
			 	}
			 	else
				{
			 		while(1)
					{
			 			bufptr = fgets(buffer, sizeof(buffer), file_to_search);
			 			if(bufptr == NULL)
						{
			 				if(feof(file_to_search)) break;
			 				if(ferror(file_to_search))
							{
			 					printf("Error reading file %s \n", element->path_name);
			 					break;
			 				}
			 			}
			 			searchptr = strstr(buffer, argv[1]);
			 			if(searchptr != NULL)
						{
			 				printf("Found string %s within file %s. \n", argv[1], element->path_name);
			 				num_occurences ++;
			 				// getchar();
			 			}
			 		}
			 	}
			 	fclose(file_to_search);
			}
			else
			{
				printf("%s is of type other. \n", element->path_name);
			}
			free((void *)element);
		} // while

		/* Stop timer here and determine the elapsed time. */
		struct timeval stop;
		gettimeofday(&stop, NULL);
		printf("\n \n \n");
		printf("Overall execution time = %fs. \n", (float)(stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec)/(float)1000000));
		printf("The string %s was found %d times within the file system. \n", argv[1], num_occurences);

		printf("====================\n\n");


		RET_TYPE *out = (RET_TYPE *)malloc(sizeof(RET_TYPE));
		out->time = (float)(stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec)/(float)1000000);
		out->count = num_occurences;

		return out;
}

/* Given a search string, the function performs a multi-threaded search of the
 * file system starting from the specified path name. */
RET_TYPE* search_for_string_mt(char **argv)
{
	pthread_mutex_t queue_mutex, count_mutex;
	pthread_mutex_init( &queue_mutex, NULL);
	pthread_mutex_init( &count_mutex, NULL);

	pthread_cond_init( &wake_up, NULL );
	pthread_mutex_init( &num_sleeping, NULL );

	pthread_mutex_init( &done, NULL );

	sleepers = (pthread_mutex_t *)malloc( sizeof(pthread_mutex_t)*NUM_THREADS );
	for( int i = 0; i < NUM_THREADS; i ++ )
		pthread_mutex_init( &sleepers[i], NULL );

	int count = 0;
	int *p_count = &count;

	THREAD_ARGS *t_args;

	t_args = (THREAD_ARGS *)malloc(sizeof(THREAD_ARGS));

	t_args->threadID = 0;
	t_args->mutex_queue = &queue_mutex;
	t_args->mutex_count = &count_mutex;
	t_args->num_occurences = p_count;
	t_args->argv = argv;


	num_threads = 0;

	struct timeval start;
	gettimeofday(&start, NULL);

	if( pthread_create( &threads[0], NULL, worker_thread, (void *)t_args ) == 0 )
	{
		pthread_mutex_lock( &queue_mutex );
		num_threads++;
		pthread_mutex_unlock( &queue_mutex );
	}
	else
	{
		printf("E: could not create thread! Exiting\n");
		exit(-1);
	}

	for( int i = 0; i < NUM_THREADS; i++ )
		pthread_join( threads[i], NULL );

	struct timeval stop;
	gettimeofday(&stop, NULL);


	printf("\n \n \n");
	printf("Overall execution time = %fs. \n", (float)(stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec)/(float)1000000));
	printf("The string %s was found %d times within the file system. \n", argv[1], count);
	printf("====================\n\n");

	RET_TYPE *out = (RET_TYPE *)malloc(sizeof(RET_TYPE));
	out->time = (float)(stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec)/(float)1000000);
	out->count = count;

	return out;
}

void *worker_thread( void *args )
{
	THREAD_ARGS *l_args = (THREAD_ARGS *) args;

	queue_element_t *element, *new_element;
	struct stat file_stats;
	int status;
	DIR *directory = NULL;
	struct dirent *result = NULL;
	struct dirent *entry = (struct dirent *)malloc(sizeof(struct dirent) + MAX_LENGTH); // Allocate memory for the directory structure

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
		insert_in_queue(l_args->queue, element); // Insert the initial path name into the queue
	}


	while( 1 )
	{
		pthread_mutex_lock( l_args->mutex_queue );
		/* While there is work in the queue, process it. */
		if(l_args->queue->head != NULL)
		{
			queue_element_t *element = remove_from_queue(l_args->queue);
			pthread_mutex_unlock( l_args->mutex_queue );
			status = lstat(element->path_name, &file_stats); // Obtain information about the file pointed to by path_name
			if(status == -1)
			{
				printf("E/%d: Error obtaining stats for %s \n", l_args->threadID, element->path_name);
				free((void *)element);
				continue;
			}
			if(S_ISLNK(file_stats.st_mode)){ // Check if the file is a symbolic link; if so ignore it
			}
			// Check if file is a directory; if so descend into it and post
			//     work to the queue
			else if(S_ISDIR(file_stats.st_mode))
			{
				printf("I/%d: %s is a directory. \n", l_args->threadID, element->path_name);
				directory = opendir(element->path_name);
				if(directory == NULL)
				{
					printf("W/%d: Unable to open directory %s \n", l_args->threadID, element->path_name);
					continue;
				}
				while(1)
				{
					// Store the directory item in the entry data structure; if
					//     result == NULL, we have reached the end of the directory
					status = readdir_r(directory, entry, &result);
					if(status != 0)
					{
					  	printf("W/%d: Unable to read directory %s \n", l_args->threadID, element->path_name);
					  	break;
					}
					if(result == NULL)
					{
						if( l_args->threadID == 0 && !spawned )
						{
							spawned = 1;
							THREAD_ARGS *t_args;
							for( int i = 1; i < NUM_THREADS; i++ )
							{
								t_args = (THREAD_ARGS *)malloc(sizeof(THREAD_ARGS));
								t_args->threadID = i;
								t_args->queue = l_args->queue;
								t_args->mutex_queue = l_args->mutex_queue;
								t_args->mutex_count = l_args->mutex_count;
								t_args->num_occurences = l_args->num_occurences;
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

					/* Ignore the "." and ".." entries. */
					if(strcmp(entry->d_name, ".") == 0)
					  		 continue;
					if(strcmp(entry->d_name, "..") == 0)
					  		 continue;

					/* Insert this directory entry in the queue. */
					new_element = (queue_element_t *)malloc(sizeof(queue_element_t));
					if(new_element == NULL)
					{
						printf("E/%d: Error allocating memory. Exiting. \n", l_args->threadID);
						exit(-1);
					}
					/* Construct the full path name for the directory item stored in entry. */
					strcpy(new_element->path_name, element->path_name);
					strcat(new_element->path_name, "/");
					strcat(new_element->path_name, entry->d_name);

					/* safely add to queue */
					pthread_mutex_lock( l_args->mutex_queue );
					insert_in_queue(l_args->queue, new_element);
					pthread_mutex_unlock( l_args->mutex_queue );

					/* wake up others */
					for( int i = 0; i < NUM_THREADS; i++ )
						pthread_cond_signal( &wake_up ); // man, pthread_broadcast never works for me
				}
				closedir(directory);
			}
			else if(S_ISREG(file_stats.st_mode))// Check if file is a regular file
			{
			 	printf("I/%d: %s is a regular file. \n", l_args->threadID, element->path_name);
			 	FILE *file_to_search;
			 	char buffer[MAX_LENGTH];
			 	char *bufptr, *searchptr;

			 	/* Search the file for the search string provided as the command-line argument. */
			 	file_to_search = fopen(element->path_name, "r");
			 	if(file_to_search == NULL)
				{
			 		printf("I/%d: Unable to open file %s \n", l_args->threadID, element->path_name);
			 		continue;
			 	}
			 	else
				{
			 		while(1)
					{
			 			bufptr = fgets(buffer, sizeof(buffer), file_to_search);
			 			if(bufptr == NULL)
						{
			 				if(feof(file_to_search)) break;
			 				if(ferror(file_to_search))
							{
			 					printf("E/%d: Error reading file %s \n", l_args->threadID, element->path_name);
			 					break;
			 				}
			 			}
			 			searchptr = strstr(buffer, l_args->argv[1]);
			 			if(searchptr != NULL)
						{
			 				printf("I/%d: Found string %s within file %s. \n", l_args->threadID, l_args->argv[1], element->path_name);
							pthread_mutex_lock(l_args->mutex_count);
			 				(*l_args->num_occurences)++;
							pthread_mutex_unlock(l_args->mutex_count);
			 				// getchar();
			 			}
			 		}
			 	}
			 	fclose(file_to_search);
			}
			else
			{
				printf("I/%d: %s is of type other. \n", l_args->threadID, element->path_name);
			}
			free((void *)element);
		} // if
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
			else if( number_sleeping == (NUM_THREADS - 1) )
			{
				all_done = 1;
				pthread_mutex_unlock( &done );
				pthread_mutex_unlock( &num_sleeping );
				// wake everyone up

				printf("D/%d: Waking everyone up.\n", l_args->threadID);
				for( int i = 0; i < NUM_THREADS; i++ )
					pthread_cond_signal( &wake_up ); // man, pthread_broadcast never works for me
				printf("D/%d: Exiting.\n", l_args->threadID);
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
				printf("D/%d: sleeping.\n", l_args->threadID);
				pthread_mutex_lock( &done ); // check done status just before sleeping
				if( all_done == 1 )
				{
					pthread_mutex_unlock( &done );
					pthread_mutex_unlock( &sleepers[l_args->threadID] );
					pthread_exit(0);
				}
				pthread_mutex_unlock( &done );
				pthread_cond_wait( &wake_up, &sleepers[l_args->threadID] );
				printf("D/%d: awake: ", l_args->threadID);
				pthread_mutex_unlock( &sleepers[l_args->threadID] );
				if( all_done == 1 )
				{
					printf("woken to exit.\n");
					pthread_exit(0);
				}
				else
				{
					printf("woken to continue.\n");
					pthread_mutex_lock( &num_sleeping);
					number_sleeping --;
					pthread_mutex_unlock( &num_sleeping );
					continue;
				}
			}
		}
	}
}

