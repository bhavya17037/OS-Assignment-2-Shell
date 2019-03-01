#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <unistd.h> 

void parser(char *input, char **command) {
	char *parsedInput = strtok(input, "|");

	int i = 0;
	while(parsedInput != NULL) {
		command[i++] = parsedInput;
		parsedInput = strtok(NULL, "|");
	}
}

void execute(char *) {

}

void execute_command(char *command){
	if(strstr(command, ">>")){
		// >> case
	}else if(strstr(command, ">&")){
		// >& case
	}else if(strstr(command, ">")){
		// > case
	}else if(strstr(command, "|")){
		// pipe case
	}else{
		// simple command case
	}
}

int main(){

	while(1) {
		char input[100000];
		printf("%s", "shell: root$ ");
		scanf("%s",input);

		if (strcmp(input,"exit") == 0) {
			break;
		}

		char *command[1000];
		memset(command, '\0', sizeof(command));
		parser(input, &command[0]);
		int i = 0;
		while(command[i] != NULL) {
			printf("%s\n", command[i]);
			i++;
		}
	}
	return 0;
}

