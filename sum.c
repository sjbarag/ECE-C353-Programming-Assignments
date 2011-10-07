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
	int len;                                                     // used to strip newlines


	/* create shm object */
	fd = shm_open(SO_PATH, O_CREAT | O_EXCL | O_RDWR, S_IRWXU | S_IRWXG);
	if (fd < 0)
	{
		printf("Error in shm_open()!\n");
		exit(-1);
	}

	/* set size of shm */
	ftruncate(fd, sizeof( char[MAX_MESSAGE_LENGTH] ) );
	printf("Success in creating shared memory object at /dev/shm%s\n", SO_PATH); // dbg


	/* create pipes */
	if( (pipe(fd_p2c) < 0) || (pipe(fd_c2p) < 0) )
	{
		printf("Error creating pipes.  Exiting.\n");
		exit(-1);
	}

	/* fork! */
	if( (pid = fork()) < 0)
	{
		printf("OH NOEZ!  Fork error.\n");
		exit(-1);
	}
	else if(pid == 0)       /* if child */
	{
		printf("[Child] \tForked correctly!\n"); // dbg
		int a, b, n_shm;      // sum components and number of bytes written

		/* map shared memory! */
		char *shared_msg = (char *)mmap(NULL, MAX_MESSAGE_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

		while(1)
		{
			/* read from pipe to synchronize processes */
			printf("[Child] \tWaiting to read from pipe\n"); // dbg
			errno = 0;
			n_pipe = read(fd_p2c[0], &m, 1);
			printf("[Child] \tReceived %i bytes\n", n_pipe); // dbg
			if ( n_pipe == -1 )
				printf("[Child] \tError reading from pipe: %s\n", strerror(errno));
			/*str[n] = '\0';
			if( strcmp(m, "r") == 0 )
				printf("[Child] \tSignaled ready to read from shm.\n");*/

			printf("[Child] \tshared_msg = %s\n", shared_msg); // dbg

			if( shared_msg == NULL )
			{
				printf("[Child] \tSHM is empty\n");
				exit(-1);
			}

			/* set a and b */
			a = atoi(strtok(shared_msg, ","));
			b = atoi(strtok(NULL, ","));

			/* dbg */
			printf("[Child] \ta=%i\n",a);
			printf("[Child] \tb=%i\n",b);

			/* write sum to shm */
			n_shm = snprintf(shared_msg, MAX_MESSAGE_LENGTH+1, "%i", a+b);
			if (n_shm <= 0)
				printf("[Child] \tError writing to shared_msg.  Returned %i\n", n_shm);

			printf("[Child] \tWrite to shm complete\n"); // dbg

			/* write to pipe to synchronize processes */
			write(fd_c2p[1], "c", 1);
			printf("[Child] \tWrite to pipe complete\n"); // dbg

			printf("[Child] \t--- Done writing ---\n"); // dbg

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
			printf("Enter the first number:");
			fgets(in_A, MAX_MESSAGE_LENGTH, stdin);
			printf("Enter the second number:");
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
			printf("shared_msg = '%s'\n", shared_msg); // dbg

			/* write to pipe to synchronize processes */
			write(fd_p2c[1], "c", 1);

			/* wait for child */
			printf("[Parent]\tWaiting for child\n");  // dbg
			n_pipe = read(fd_c2p[0], &m, 1);
			printf("[Parent]\tReceived %i bytes\n", n_pipe); // dbg
			printf("[Parent]\tSum: %s\n", shared_msg);

			/* reset */
			strcpy(shared_msg, "\0");
		}
	}
}
