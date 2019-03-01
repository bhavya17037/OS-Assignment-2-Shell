#include<stdio.h>
#include<string.h>
#include <sys/types.h> 
#include <unistd.h> 

char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}

void parser(char *input, char **command) {
	char *parsedInput = strtok(input, "|");

	int i = 0;
	while(parsedInput != NULL) {
		command[i++] = trimwhitespace(parsedInput);
		parsedInput = strtok(NULL, "|");
	}
}

void execute(char *command[]) {
	int i = 0;

	while (command[i] != NULL) {
		int fd[2];

		int pid = fork();

		if (pid == 0) {
			close(0);
			dup(fd[0]);
			close(fd[0]);
			close(fd[1]);
			execute_command(command[i]);
		} else if (pid < 0) {
			printf("%s\n", "Error while executing fork");
		} else {
			close(1);
			dup(fd[1]);
			close(fd[0]);
			close(fd[1]);
			wait();
		}
	}
}

void execute_command(char *command) {

}

int main() {

	while(1) {
		char input[100000];
		printf("%s", "shell: root$ ");
		fgets(input,100000,stdin);

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

