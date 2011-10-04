#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX_STRING_SIZE 256

int main(int argc, char **argv)
{
	if(argc == 1)
	{
		printf("USAGE: $ changeCaseStr text\n");
		return -1;
	}

	char data[MAX_STRING_SIZE];
	char *str = data;

	str = argv[1];

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
	return 0;
}

