#include <stdio.h>
#include <sys/mman.h>
//#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
//#include <time.h>

#define SO_PATH "/sjb89_hw1-5"
#define MAX_MESSAGE_LENGTH 50
#define MAX_FLAG_LENGTH 5

struct msg_s
{
	char content[MAX_MESSAGE_LENGTH];
};

int main(int argc, char **argv[])
{
	int fd, pid;
	int shared_seg_size = (sizeof(struct msg_s)); // We want a shared segment
	                                              // capable of storing one
	                                              // message
	struct msg_s *shared_msg;   // shared segment and head of the messages list

	/* pipes */
	int fd_p2c[2], fd_c2p[2];   // parent to child & child to parent file
	                            // descriptors for pipes
	char data[MAX_FLAG_LENGTH];
	char *str = data;
	int n;


	fd = shm_open(SO_PATH, O_CREAT | O_EXCL | O_RDWR, S_IRWXU | S_IRWXG);
	if (fd < 0)
	{
		printf("Error in shm_open()!");
		exit(-1);
	}
	printf("Success in creating shared memory object at /dev/shm%s\n", SO_PATH);

	if( (pipe(fd_p2c) < 0) || (pipe(fd_c2p) < 0) )
	{
		printf("Error creating pipes.  Exiting.\n");
		exit(-1);
	}


	if( (pid = fork()) < 0) /* fork a child! */
	{
		printf("OH NOEZ!  Fork error.\n");
		exit(-1);
	}
	else if(pid == 0)       /* if child */
	{
		/* read from pipe to synchronize processes */
		n = read(fd_p2c[0], str, MAX_FLAG_LENGTH);
		printf("[Child] \tSignaled ready to read from shm.\n");

		/* read from shm */
		// mmap
		// open
		// read
		// close

		/* sum */

		/* write to pipe to synchronize processes */
		write(fd_c2p[1], str, strlen(str));
	}
}
