#include "hello.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  

int main(int argc, char *argv[])
{	

	char *a_arg = NULL;
	int opt_o = 0;
	int c;

	while ( (c = getopt(argc, argv, "a:o") ) != -1 )
	{
		switch (c)
		{
			case 'a': a_arg = optarg;
				break;
			case 'o': opt_o++;
				break;
			case '?': 
				break;
		}
	}

	printf("%d\n", argv[argc]);

	if ( a_arg != NULL )
	{
		printf("%s\n", "a used");
	}

	if ( opt_o > 1 ){
		printf("o more than once\n");
	}

	char *myprog;

	myprog = argv[0];
	usage(myprog);


	return 0;
}