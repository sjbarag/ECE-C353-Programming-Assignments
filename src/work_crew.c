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
#include "queue.h"

// Function prototypes
void search_for_string_serial(char **);
void search_for_string_mt(char **);


int main(int argc, char** argv)
{
	if(argc < 3)
	{
		printf("Usage: %s <search string> <path> \n", argv[0]);
		exit(0);
	}

	// Perform a serial search of the file system
	search_for_string_serial(argv);

	// Perform a multi-threaded search of the file system
	search_for_string_mt(argv);

	exit(0);
}

/* Given a search string, the function performs a single-threaded or serial search of the file system starting from the specified path name. */
void search_for_string_serial(char **argv)
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
}

/* Given a search string, the function performs a multi-threaded search of the file system starting from the specified path name. */
void search_for_string_mt(char **argv)
{
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
}

