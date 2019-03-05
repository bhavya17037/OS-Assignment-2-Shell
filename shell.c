#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h> 
#include <errno.h> 

pid_t pid, curr_pid;
pid_t child_pid = -1;
int killed = 0;

void sigint_handler(int signo) {

	switch (signo) {
		case SIGINT:

			printf("%s\n", "inside handler");
			if(getpid() != curr_pid){
				printf("pid in handler: %d\n", getpid());
				killed = 1;
				//printf("%s\n", "yoo");
				kill(getpid(), SIGKILL);
				fflush(stdout);
				//printf("%s\n", "heree");
			}
			main();
			break;
		default:
			break;


	}
}

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

void parser(char *input, char **command, char *delim) {
	char *parsedInput = strtok(input, delim);

	int i = 0;
	while(parsedInput != NULL) {
		command[i++] = trimwhitespace(parsedInput);
		parsedInput = strtok(NULL, delim);
	}
	command[i++] = NULL;
}

int redirection(char *input, char *delim) {
	
	char *c[1024];

	parser(input, c, delim);

	int i = 0;

	if (c[1] == NULL) {
		// printf("%s\n", "No redirection");
		return 0;
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

		return 0;

	} else if (strcmp(delim, ">") == 0) {

		// printf("%s\n", "Output redirection");

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
				return 0;
			} else {
				if (strcmp(file1[j], "2") == 0) {

					// printf("%s\n", "closing STDERR");

					close(2);
				} else {
					// printf("%s\n", "Closing STDOUT");
					close(1);
				}
				// printf("%s\n", "creating file");
				int fd = open(file2[0], O_CREAT | O_RDWR, 0666);
				return fd;
			}
			i++;
		}
	}
}

void execute(char **command) {
	int i = 0;
	int fd[2];
	int in = 0;
	killed = 0;
	//signal(SIGINT, sigint_handler);
	while (command[i] != NULL) {

		// printf("%s%d\n", "Executing command: ", i);
		char *cmd = command[i];
		child_pid = -1;
		pipe(fd);
		pid = fork();
		
		if (pid == 0) {
			//signal(SIGINT, sigint_handler);
			//sigaction(SIGTSTP, &psa, NULL);
			child_pid = pid;
			printf("%s\n", "child_pid is 0...yay");
		
			printf("%s\n", "child process running...");
			while(killed == 0);
			//printf("%s\n", "child process stopped in between...");
			//return;

			int a = redirection(cmd, "<");

			if (a == -1) {
				printf("%s\n", "Invalid file");
			} else {
				redirection(cmd, ">");
				// redirection(cmd, ">>");

				char *args[1024];

				parser(cmd, args, ">");
				char *temp1 = args[0];

				// printf("%s\n", "After trimming >");
				// printf("%s\n", temp1);

				parser(temp1, args, "<");
				char *temp2 = args[0];

				// printf("%s\n", "After trimming <");
				// printf("%s\n", temp2);

				parser(temp2, args, " ");

				char *to_execute = args[0];

				// printf("%s%s\n", "command to execute: ", to_execute);
				// printf("%s\n", "Arguments passed");

				// for (int i = 0; args[i] != NULL; i++) {
				// 	printf("%s\n", args[i]);
				// }
				dup2(in, 0);
				if (command[i+1] != NULL) {
					dup2(fd[1], 1);
				}
				close(fd[0]);
				execvp(to_execute, args);



			}
		} else if (pid < 0) {
			//signal(SIGINT, sigint_handler);
			printf("%s\n", "Error while executing fork");
		} else {
			//signal(SIGINT, sigint_handler);
			wait(0);
			//child_pid = pid;
			close(fd[1]);
			in = fd[0];
			i++;
		}
	}
}

int main() {
	//signal(SIGINT, sigint_handler);
	killed = 0;
	while(1) {
		struct sigaction sa;
		memset (&sa, 0, sizeof (sa));
		sa.sa_handler = sigint_handler;
    	sigemptyset(&(sa.sa_mask));
    	sigaddset(&(sa.sa_mask), SIGINT);
    	sigaction(SIGINT, &sa, NULL);
		curr_pid = getpid();
		//printf("curr_pid: %d\n", curr_pid);
		char input[1024];
		printf("%s", "shell: root$ ");
		fgets(input,1024,stdin);

		if (strcmp(input,"exit") == 0) {
			break;
		}

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
	return 0;
}