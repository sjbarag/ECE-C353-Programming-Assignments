/*
 * Author: Sean Barag
 * Inspiration taken from Dr. Kandasamy's notes

 * This program changes the case of the characters in a string in a child
 * process, usings pipes to communicate.
 *
 * To compile:
 *     $ gcc -std=c99 changeCaseStr changeCaseStr.c
 * or use the included Makefile.
 */

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
	if(argc < 2)
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
		/* read from parent->child pipe */
		n = read(fd_p2c[0], str, MAX_STRING_SIZE); // read n bytes from fd_p2c
		close(fd_p2c[0]);
		str[n] = '\0'; // gotta null terminate!

		/* change case; leaves non-alphabetic chars alone */
		for( int i = 0; i < MAX_STRING_SIZE; i++)
		{
			if( str[i] == '\0' )
				break;
			if (str[i] >= 'a' && str[i] <= 'z')
				str[i] = toupper(str[i]);
			else if (str[i] >= 'A' && str[i] <= 'Z')
				str[i] = tolower(str[i]);
		}

		/* write to child->parent pipe */
		write(fd_c2p[1], str, strlen(str));
		close(fd_c2p[1]);
		exit(0);
	}

	/* store from cmdline */
	str = argv[1];

	/* write to parent->child pipe */
	write(fd_p2c[1], str, strlen(str));
	close(fd_p2c[1]);

	/* read from child->parent pipe */
	n = read(fd_c2p[0], str, MAX_STRING_SIZE);
	close(fd_c2p[0]);
	str[n] = '\0'; // gotta null terminate!

	/* print final output */
	printf("%s\n", str);

	exit(0);
}
