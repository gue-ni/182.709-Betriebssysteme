#include <unistd.h>
#include <stdio.h>
#include <string.h>


int main()
{
/*	
	char *cmd[] = {"ls", "-l", (char *)0 };
	execv("/bin/bash", cmd);
	execlp("ls", "ls", "-l", NULL);
	fprintf(stdout, "Hello");
	(void)fork();	
*/

	int pipefd[2];
	pipe(pipefd);

	pid_t pid = fork();


	if (pid != 0){
		FILE *fd = fdopen(pipefd[1], "r+");
		fputs("hello", fd);
	} else {
		char buf[10];
		FILE *fd = fdopen(pipefd[0], "r+");
		fgets(buf, sizeof(buf), fd);
		fprintf(stdout, "Read: %s\n", buf);
	}



	return 0;
}

