#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

#define MAX_STRING_SIZE 256

int main(int argc, char **argv)
{
	/* check for correct number of arguments.
	 *   no need to declare anything if we're not going to run!
	 */
	if(argc == 1)
	{
		printf("USAGE: $ changeCaseStr 'text'\n");
		return -1;
	}

	char data[MAX_STRING_SIZE];   // array in which to store the string from argv[1]
	char *str = data;             // pointer to data
	int pid;                      // process ids!
	int n;                        // number of bits (for child)

	/* pipe stuff */
	int fd_p2c[2], fd_c2p[2];     // parent to child & child to parent file
	                              //descriptors for pipes



	/* create pipes! */
	if( (pipe(fd_p2c) < 0) || (pipe(fd_c2p) < 0) )
	{
		printf("Error creating pipes.  Exiting. \n");
		exit(-1);
	}

	/* fork! */
	if( (pid = fork()) < 0) /* fork! */
	{
		printf("OH NOEZ!  Fork error.\n");
		exit(-1);
	}
	else if(pid == 0)       /* if child */
	{
		printf("Child here!\n");
		n = read(fd_p2c[0], str, MAX_STRING_SIZE); // read n bytes from fd_p2c
		close(fd_p2c[0]);
		printf("Child: Got %d bytes from pipe.\n", n);
		str[n] = '\0'; // gotta null terminate!

		for( int i = 0; i < MAX_STRING_SIZE; i++)
		{
			if( str[i] == '\0' )
				break;
			if (str[i] >= 'a' && str[i] <= 'z')
				printf("%c", toupper(str[i]) );
			else if (str[i] >= 'A' && str[i] <= 'Z')
				printf("%c", tolower(str[i]) );
			else
				printf("%c", str[i] );
		}
		printf("\n");
		exit(0);
	}

	printf("Parent here!\n");
	/* store from cmdline */
	str = argv[1];
	write(fd_p2c[1], str, strlen(str));
	close(fd_p2c[1]);

	exit(0);
}

