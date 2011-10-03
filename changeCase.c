#include <stdio.h>
#include <ctype.h>    /* For tolower() function */

void main()
{
	char answer = 'N';  /* Records yes or no to continue the loop */

	/* check for more input */
	printf("Do you want to enter another value? (Y or N): ");
	scanf(" %c", &answer );
	if (answer >= 'a' && answer <= 'z')
		printf("%c\n", toupper(answer) );
	if (answer >= 'A' && answer <= 'Z')
		printf("%c\n",tolower(answer) );
}
