#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>

void parser(char *input, char **command, char* delim) {
	char *parsedInput = strtok(input, delim);

	int i = 0;
	while(parsedInput != NULL) {
		command[i++] = parsedInput;
		parsedInput = strtok(NULL, delim);
	}
}

void execute(char **comm) {

	//char *args[10];
	// args[0] = comm[0];
	// args[1] = comm[1];
	// printf("%s\n", comm[0]);
	// args[2] = NULL;
	int pid = fork();
	if(pid == 0){
		if(execvp(comm[0], comm) < 0){
			printf("Something went wrong! Could not execute!\n");
		}
		exit(0);
	}else if(pid == -1){
		printf("Illegal command!\n");
		return;
	}else{
		wait(NULL);
		return;
	}
}

void execute_command(char* input, char **command){
	if(strstr(input, ">>")){
		// >> case
	}else if(strstr(input, ">&")){
		// >& case
	}else if(strstr(input, ">")){
		// > case
	}else if(strstr(input, "|")){
		// pipe case
	}else{
		// simple command case
		char *comm[100];
		parser(input, comm, " ");
		execute(comm);

	}
}

int main(){

	while(1) {
		char input[100000];
		printf("shell: root$ ");
		scanf("%100000[^\n]",input);

		if (strcmp(input,"exit") == 0) {
			break;
		}

		char *command[1000];
		memset(command, '\0', sizeof(command));
		parser(input, &command[0], "|");
		int i = 0;
		execute_command(input, &command[0]);
	}
	return 0;
}

