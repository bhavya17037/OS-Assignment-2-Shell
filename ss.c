#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h> 
#include <errno.h>
#include <signal.h>
#include <ctype.h>

pid_t parent, child;
int killed;

void psigint(){
	//printf("current child = %d\n", pid);
	if(child != -1){
		kill(child, SIGINT);
	}else{
		kill(getpid(), SIGKILL);
	}
}

void sigint() 
{ 
    signal(SIGINT, sigint); /* reset signal */
    killed = 1;
    // printf("%s\n", "killing current running child!");
    kill(getpid(), SIGKILL);
    //printf("CHILD: I have received a SIGINT\n"); 
} 

char *trimwhitespace(char *str)
{
  char *end;

  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  
    return str;

  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  end[1] = '\0';

  return str;
}

void parser(char *input, char **command, char delim[]) {

	// printf("%s\n", "before parsing");
	// printf("%s\n", input);

	char *parsedInput = strtok(input, delim);

	int i = 0;
	while(parsedInput != NULL) {
		command[i++] = trimwhitespace(parsedInput);
		parsedInput = strtok(NULL, delim);
	}
	command[i++] = NULL;
	// printf("%s\n", "after parsing");
	// printf("%s\n", input);
}

int redirection(char *input, char *delim) {
	
	char *c[1024];

	// printf("%s\n", input);

	int index = 0;
	int flag = 0;
	int j = 0;
	while(input[j+1] != '\0') {
		if (input[j] == '>' && input[j+1] == '>') {
			flag = 1;
		}
		if (input[j] == '>' && input[j+1] != '>') {
			index += 1;
		}
		j++;
	}

	parser(input, c, delim);

	int i = 0;

	if (c[1] == NULL) {
		// printf("%s\n", "No redirection");
		if(strcmp(delim, ">>") == 0) {
			return -1;
		} else {
			return 0;
		}
	}

	if (strcmp(delim, "<") == 0) {
		// printf("%s\n", "Input redirection");
		while (c[i+1] != NULL) {
			i++;
		}
		char *arg = c[i];
		char *file[1024];
		parser(arg, file, " ");

		close(0);
		int fd = open(file[0], O_RDONLY);

		return fd;

	} else if (strcmp(delim, ">>") == 0) {

		int fd = -1;
		if (flag > 0) {
			// printf("%s\n", "in here");
			char *arg2 = trimwhitespace(c[index]);
			// printf("%s\n", arg2);

			char *file2[1024];
			parser(arg2, file2, " ");

			// printf("%s\n", "nope");
			// printf("%s\n", file2[0]);
			close(1);
			fd = open(file2[0], O_RDWR | O_APPEND | O_CREAT, 0666);
			// printf("%d\n",fd);
		}

		return fd;

	} else {

		// printf("%s\n", "Output redirection");
		int fd;

		while(c[i+1] != NULL) {
			char *arg1 = trimwhitespace(c[i]);
			char *arg2 = trimwhitespace(c[i+1]);

			char *file1[1024];
			char *file2[1024];

			parser(arg1, file1, " ");
			parser(arg2, file2, " ");
			int j = 0;
			while (file1[j+1] != NULL) {
				j++;
			}

			if (strcmp(file2[0], "&1") == 0) {

				// printf("%s\n", "Redirecting 2 to 1");

				close(2);
				dup(1);
				fd = 0;
			} else {
				if (strcmp(file1[j], "2") == 0) {

					// printf("%s\n", "closing STDERR");

					close(2);
				} else {
					// printf("%s\n", "Closing STDOUT");
					close(1);
				}
				// printf("%s\n", "creating file");
				fd = open(file2[0], O_CREAT | O_RDWR | O_TRUNC, 0666);
			}
			i++;
		}
		return fd;
	}
}

void copy(char *a1, char *a2){
	int i = 0;
	while(a1[i] != '\0'){
		// printf("%s\n", "exiting copy");
		a2[i] = a1[i];
		i++;
	}
	a2[i] = '\0';
	// printf("%s\n", "exiting copy");
}

void execute(char **command) {
	int i = 0;
	int fd[2];
	int pid;
	int in = 0;
	killed = 0;
	while (command[i] != NULL) {

		
		char *cmd = command[i];
	
		pipe(fd);
		pid = fork();
		child = pid;
		if (pid == 0) {
			signal(SIGINT, sigint);
			//while(killed == 0);
			char *args[1024];

			// printf("%s\n", cmd);
			
			// printf("%s\n", "before redirection <");
			// printf("%s\n", cmd);
			char t1[1024];
			copy(cmd, t1);
			// printf("%s\n", "after redirection <");
			int a = redirection(t1, "<");
			// printf("%s\n", "after redirection <");
			// printf("%s\n", cmd);

			if (a == -1) {
				printf("%s\n", "Invalid file");
			} else {
				// printf("%s\n", "before redirection >");
				// printf("%s\n", cmd);
				char t3[1024];
				copy(cmd, t3);
				int x = redirection(t3, ">>");

				if (x == -1) {
					// printf("%s\n", "redirection");
					char t2[1024];
					copy(cmd, t2);
					redirection(t2, ">");
				}

				// printf("%s\n", "after redirection >");
				// printf("%s\n", cmd);
				

				parser(cmd, args, " ");

				char *temp1[1024];

				for (int k = 0; args[k] != NULL; k++) {
					if (args[k][0] == '1' || args[k][0] == '2') {
						break;
					}

					if (args[k][0] == '<' || args[k][0] == '>') {
						break;
					}

					temp1[k] = args[k];
					// temp1[k+1] = "\0";
				}

				char *to_execute = args[0];

				// printf("%s\n", "After trimming >");
				// printf("%s\n", temp1);

				// printf("%s\n", "After trimming <");
				// printf("%s\n", temp2);

				// printf("%s%s\n", "command to execute: ", to_execute);
				// printf("%s\n", "Arguments passed");

				dup2(in, 0);
				if (command[i+1] != NULL) {
					dup2(fd[1], 1);
				}
				close(fd[0]);

				// printf("%s\n", to_execute);

				// for (int k = 0; temp1[k] != NULL; k++) {
				// 	printf("%s\n", temp1[k]);
				// }

				if (execvp(to_execute, temp1) < 0) {
					fprintf(stderr, "%s\n", "command doesn't exist");
					exit(EXIT_FAILURE);
				}
				// printf("%d\n", errno);
				// exit(errno);
			}
		} else if (pid < 0) {
			printf("%s\n", "Error while executing fork");
		} else {
			signal(SIGINT, psigint);
			wait(0);

			close(fd[1]);
			in = fd[0];
			i++;
		}
	}
}

int main() {
	killed = 0;
	while(1) {
		child = -1;
		char input[1024];
		char tempinput[1024];
		printf("%s", "shell: root$ ");
		fgets(input,1024,stdin);
		strncpy(tempinput, input, 1024);

		if (strcmp(input,"exit\n") == 0) {
			return 0;
		}
		char *tt[10];
		parser(tempinput, tt, " ");
		if(strcmp(tt[0], "cd") == 0 && tt[1] != NULL && tt[2] == NULL){
            chdir(tt[1]);
		}else{
			char *command[1024];
			memset(command, '\0', sizeof(command));
			parser(input, &command[0], "|");
		
			// int i = 0;
			// while(command[i] != NULL) {
			// 	printf("%s\n", command[i]);
			// 	i++;
			// }
			execute(&command[0]);
		}

	}
	return 0;
}