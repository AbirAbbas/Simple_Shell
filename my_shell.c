#define _XOPEN_SOURCE 500

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>

extern char **getln();

int returnInteger(char * number);
int returnHash (char * number);
int fileExist (const char * fileName);
void sig_handler(int signal);

int main() {
	int counter;
	int i;
	int checkBlock;
	char **args; 
	char directory[1024];
	char command[1024];
	char fileName[1024];
	int status;
	//char * environment[] = { (char *) "PATH=/bin", 0 };
	
	while(1) {
		if (getcwd(directory, sizeof(directory)) != NULL) {
			fprintf(stdout, "Shell:~%s$ ", directory);
		}
		else {
			perror("getcwd() error");
		}

		args = getln();
		i = 0;
		
		for(counter = 0; args[counter] != NULL; counter++) {
			i++;
			//Counter function
		}
		
		if (counter > 0 && strcmp(args[i-1],"&") == 0) {
			checkBlock = 1;
			args[i-1] = NULL;
		}
		else {
			checkBlock = 0;
		}
		
		
		if (counter == 0) {
			
		}
		else if (strcmp(args[0], "exit") == 0) {
			exit(0);
			break;
		}
		else if (strcmp(args[0], "add") == 0) {
			int sum = 0;
			for(counter = 1; args[counter] != NULL; counter++) {
				if (args[counter + 1] != NULL) {
					printf("%d + ", returnInteger(args[counter]));
				}
				else {
					printf("%d = ", returnInteger(args[counter]));
				}
				sum += returnInteger(args[counter]);
			}
			
			printf("%d\n", sum);
		}
		else if (strcmp(args[0], "args") == 0) {
			//decrement for initial command
			counter--;
			
			printf("argc = %d, args = ", counter);
			
			for(counter = 1; args[counter] != NULL; counter++) {
				if (args[counter + 1] != NULL) {
					printf("%s, ", args[counter]);
				}
				else {
					printf("%s", args[counter]);
				}
			}
			
			printf("\n");
		}
		else if (strcmp(args[0], "cd") == 0) {
			chdir(args[1]);
		}
		else if (strcmp(args[0], "hash") == 0) {
			if (counter > 2) {
				for (i = 1; args[i] != NULL; i++) {
					printf("%s = %d\n", args[i], returnHash(args[i]));
				}
			}
			else if (counter == 2) {
				printf("%s = %d\n", args[1], returnHash(args[1]));
			}
			else {
				printf("Invalid Syntax\n");
			}
		}
		else {
			
			pid_t pid = fork();
			
			if (sigset(pid, sig_handler) < 0) {
				printf("Error while calling sigset!\n");
			}
			
								
			if (pid == 0) {				
				strcpy(command, "/bin/");
				strcat(command, args[0]);
				
				for (i = 0; args[i] != NULL; i++) {
					
						if (strcmp(args[i], "<") == 0 && args[i+1] != NULL) {
							strcpy(fileName, args[i+1]);
							if (fileExist(fileName)) {
								if (freopen(fileName,"r+", stdin) == NULL) {
									printf("Error while opening file, exiting child process.\n");
									exit(0);
								}
								
								int readjust = 0;
								
								for(readjust = i; args[readjust-1] != NULL; readjust++) args[readjust] = args[readjust+2];
							}
							else {
								printf("Invalid file name!\n");
								exit(0);
							}
						}
				}
				
				for (i = 0; args[i] != NULL; i++) {
					
						if (strcmp(args[i], ">") == 0 && args[i+1] != NULL) {
							strcpy(fileName, args[i+1]);
							
							if (freopen(fileName, "w+", stdout) == NULL) {
								printf("Error while creating file exiting child process.\n");
								exit(0);
							}
							
							int readjust = 0;
							
							//
							for(readjust = i; args[readjust-1] != NULL; readjust++) args[readjust] = args[readjust+2];
						}
				}
				
				if (execvp(args[0], args) == -1) {
					printf("Shell: %s: command not found\n", args[0]);
					exit(-1);
				}
				else {
					exit(-1);
				}
			}
			else if (pid < 0) {
				printf("Error while forking\n");
			}
			if (checkBlock == 0) {
				waitpid(pid, &status, 0);
			}
			
		}	
	}
	
	return 0;
}

int returnInteger(char * number) {
	int isHex = 0;
	int i = 0;
	
	for (i = 0; i < strlen(number); i++) {
		if (isalpha(number[i])) {
			isHex = 1;
		}
	}
	
	if (isHex == 0) {
		return atoi(number);
	}
	else {
		return (int)strtol(number, NULL, 16);
	}
}

int returnHash(char * number) {
	
	int sum = 0;
	int i = 0;
	
	for (i = 0; i < strlen(number); i ++) {
		sum += (int)number[i] * (i + 2);
	}
	
	return sum; 
}

int fileExist (const char * fileName) {
	
	FILE * f = NULL;
	if ((f = fopen(fileName, "r"))) {
		fclose(f);
		return 1;
	}
	
	return 0;
}

void sig_handler(int signal) {
	int status;
	wait(&status);
}

