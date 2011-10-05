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
#define MAX_FLAG_LENGTH 10

int main(int argc, char **argv[])
{
	int fd, pid;
	char *msg;

	/* pipes */
	int fd_p2c[2], fd_c2p[2];   // parent to child & child to parent file
	                            // descriptors for pipes
	char data[MAX_FLAG_LENGTH];
	char *str = data;
	int n;

	char in_A[MAX_MESSAGE_LENGTH/2], in_B[MAX_MESSAGE_LENGTH/2];
	char temp_data[MAX_MESSAGE_LENGTH];
	char *p_temp = temp_data;
	int len;


	fd = shm_open(SO_PATH, O_CREAT | O_EXCL | O_RDWR, S_IRWXU | S_IRWXG);
	if (fd < 0)
	{
		printf("Error in shm_open()!\n");
		exit(-1);
	}
	printf("Success in creating shared memory object at /dev/shm%s\n", SO_PATH);
	char *shared_msg = (char *)mmap(NULL, MAX_MESSAGE_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

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
		printf("[Child] \tForked correctly!\n");
		char *comma;
		int a, b;
		char val;

		/* read from pipe to synchronize processes */
		printf("[Child] \tWaiting to read from pipe\n");
		n = read(fd_p2c[0], str, MAX_FLAG_LENGTH);
		if( strcmp(str, "read") == 0 )
			printf("[Child] \tSignaled ready to read from shm.\n");

		printf("[Child] \tshared_msg = %s\n", shared_msg);
		/* read from shm */
		if( shared_msg == NULL )
		{
			printf("[Child] \tSHM is empty\n");
			exit(-1);
		}

		/* set a and b */
		a = atoi(strtok(shared_msg, ","));
		b = atoi(strtok(NULL, ","));

		/* sum */
		printf("[Child] \ta=%i,a");
		printf("[Child] \tb=%i,b");
		//val = itoa(a+b);


		str = "calc";
		/* write to pipe to synchronize processes */
		write(fd_c2p[1], str, strlen(str));
	}

	while(1)
	{
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

		/* form message */
		strcpy(p_temp, in_A);
		strcat(p_temp, ",");
		strcat(p_temp, in_B);

		/* write to shm */
		strcpy(msg, p_temp);
		printf("shared_msg->content = '%s'\n", msg);

		/* tell child */
		str = "read";
		write(fd_p2c[1], str, strlen(str));

		/* wait for child */
		printf("[Parent]\tWaiting for child");
		read(fd_c2p[0], str, MAX_FLAG_LENGTH);
		printf("[Parent]\tfrom pipe: %s\n",str);
	}
}
