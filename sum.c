#include <stdio.h>
#include <sys/mman.h>
//#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
//#include <time.h>
#include <errno.h>

#define SO_PATH "/sjb89_hw1-5"
#define MAX_MESSAGE_LENGTH 50




int main(int argc, char **argv[])
{
	int fd, pid;                // file descriptor and process ID, respectively

	/* pipes */
	int fd_p2c[2], fd_c2p[2];   // parent to child & child to parent file
	                            // descriptors for pipes
	char m;                     // pipe sync message
	int n_pipe;                 // number of bytes read from a pipe

	char in_A[MAX_MESSAGE_LENGTH/2], in_B[MAX_MESSAGE_LENGTH/2]; // inputs
	int len;                    // used to strip newlines
	int errstr;                 // store errno if something happens


	/* create shm object */
	fd = shm_open(SO_PATH, O_CREAT | O_EXCL | O_RDWR, S_IRWXU | S_IRWXG);
	if (fd < 0)
	{
		errstr = errno;
		printf("Error in creating shared memory object: %s\nExiting.\n", strerror(errno));
		exit(-1);
	}

	/* set size of shm */
	ftruncate(fd, sizeof( char[MAX_MESSAGE_LENGTH] ) );

	/* create pipes */
	if( (pipe(fd_p2c) < 0) || (pipe(fd_c2p) < 0) )
	{
		errstr = errno;
		printf("Error creating pipes: %s\nExiting.\n", strerror(errno));
		exit(-1);
	}

	/* fork! */
	if( (pid = fork()) < 0)
	{
		errstr = errno;
		printf("OH NOEZ!  Fork error: %s\nExiting.\n", strerror(errno));
		exit(-1);
	}
	else if(pid == 0)       /* if child */
	{
		int a, b, n_shm;      // sum components and number of bytes written

		/* map shared memory! */
		char *shared_msg = (char *)mmap(NULL, MAX_MESSAGE_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

		while(1)
		{
			/* read from pipe to synchronize processes */
			errno = 0;
			n_pipe = read(fd_p2c[0], &m, 1);
			if ( n_pipe == -1 )
			{
				errstr = errno;
				printf("Error reading from pipe: %s\n", strerror(errstr));
			}

			/* set a and b */
			a = atoi(strtok(shared_msg, ","));
			b = atoi(strtok(NULL, ","));

			/* write sum to shm */
			n_shm = snprintf(shared_msg, MAX_MESSAGE_LENGTH+1, "%i", a+b);
			if (n_shm <= 0)
			{
				errstr = errno;
				printf("Error writing to shared_msg: %s\n", strerror(errstr));
			}

			/* write to pipe to synchronize processes */
			write(fd_c2p[1], "c", 1);

			/* reset */
			a = 0;
			b = 0;
		}
	}
	else if(pid > 0) /* parent code */
	{
		while(1)
		{
			/* map shared memory! */
			char *shared_msg = (char *)mmap(NULL, MAX_MESSAGE_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

			/* read numbers */
			printf("Enter the first number: \t");
			fgets(in_A, MAX_MESSAGE_LENGTH, stdin);
			printf("Enter the second number:\t");
			fgets(in_B, MAX_MESSAGE_LENGTH, stdin);

			/* remove trailing newlines */
			len = strlen(in_A) - 1;
			if (in_A[len] == '\n')
				in_A[len] = '\0';
			len = strlen(in_B) - 1;
			if (in_B[len] == '\n')
				in_B[len] = '\0';

			/* build message in shm */
			strcpy(shared_msg, in_A);
			strcat(shared_msg, ",");
			strcat(shared_msg, in_B);

			/* write to pipe to synchronize processes */
			write(fd_p2c[1], "c", 1);

			/* wait for child */
			n_pipe = read(fd_c2p[0], &m, 1);
			if ( n_pipe <= 0 )
			{
				errstr = errno;
				printf("Error writing to pipe: %s\n", errstr);
			}

			/* print sum */
			printf("Sum:                    \t%s\n\n", shared_msg);  // extra spaces because I'm anal with spacing.

			/* reset */
			strcpy(shared_msg, "\0");
		}
	}
}
