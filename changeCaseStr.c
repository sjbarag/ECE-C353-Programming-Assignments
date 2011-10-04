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

	str = argv[1];

	if( (pid = fork()) < 0) /* fork! */
	{
		printf("OH NOEZ!  Fork error.\n");
		exit(-1);
	}
	else if(pid == 0)       /* if child */
	{
		printf("Child here!\n");
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

	return 0;
}

