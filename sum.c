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
	int n;                      // number of bytes read from a pipe

	char in_A[MAX_MESSAGE_LENGTH/2], in_B[MAX_MESSAGE_LENGTH/2]; // inputs
	int len;                                                     // used to strip newlines


	fd = shm_open(SO_PATH, O_CREAT | O_EXCL | O_RDWR, S_IRWXU | S_IRWXG);
	if (fd < 0)
	{
		printf("Error in shm_open()!\n");
		exit(-1);
	}
	ftruncate(fd, sizeof( char[MAX_MESSAGE_LENGTH] ) );
	printf("Success in creating shared memory object at /dev/shm%s\n", SO_PATH);


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
		printf("[Child] \tForked correctly!\n");
		int a, b, print_count;      // sum components and number of bytes written
		int sum;                    // sum TODO: delete
		char *shared_msg = (char *)mmap(NULL, MAX_MESSAGE_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

		while(1)
		{
			/* read from pipe to synchronize processes */
			printf("[Child] \tWaiting to read from pipe\n");
			errno = 0;
			n = read(fd_p2c[0], &m, 1);
			printf("[Child] \tReceived %i bytes\n", n);
			if ( n == -1 )
				printf("[Child] \tError reading from pipe: %s\n", strerror(errno));
			/*str[n] = '\0';
			if( strcmp(m, "r") == 0 )
				printf("[Child] \tSignaled ready to read from shm.\n");*/

			printf("[Child] \tshared_msg = %s\n", shared_msg);

			if( shared_msg == NULL )
			{
				printf("[Child] \tSHM is empty\n");
				exit(-1);
			}

			/* set a and b */
			a = atoi(strtok(shared_msg, ","));
			b = atoi(strtok(NULL, ","));

			/* sum */
			printf("[Child] \ta=%i\n",a);
			printf("[Child] \tb=%i\n",b);
			sum = a+b;

			/* write to shm */
			print_count = snprintf(shared_msg, MAX_MESSAGE_LENGTH+1, "%i", sum);
			if (print_count <= 0)
				printf("[Child] \tERROR PRINTING TO shared_msg.  Returned %i\n", print_count);

			printf("[Child] \tWrite to shm complete\n");

			/* write to pipe to synchronize processes */
			write(fd_c2p[1], "c", 1);
			printf("[Child] \tWrite to pipe complete\n");

			printf("[Child] \t--- Done writing ---\n");

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
			printf("shared_msg = '%s'\n", shared_msg);

			/* write to pipe to synchronize processes */
			write(fd_p2c[1], "c", 1);

			/* wait for child */
			printf("[Parent]\tWaiting for child\n");
			n = read(fd_c2p[0], &m, 1);
			printf("[Parent]\tReceived %i bytes\n", n);
			printf("[Parent]\tSum: %s\n", shared_msg);

			/* reset */
			strcpy(shared_msg, "\0");
			//shared_msg = "";
			//p_temp = "";
		}
	}
}
