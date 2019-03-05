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
	
	char *input[] = {"ls","-l","1>a.txt"};
	printf("%c\n", input[2][0]);
	return 0;
}





