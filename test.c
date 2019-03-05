#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h> 
#include <errno.h>

int main() {
	int pid;

	pid = fork();

	if (pid == 0) {
		printf("%s\n", "Child" );
		char *args[] = {"abcd", NULL};
		execvp(args[0], args);
	} else {
		printf("%s\n", "In parent");
		wait(0);
		
	}
	return 0;
}





