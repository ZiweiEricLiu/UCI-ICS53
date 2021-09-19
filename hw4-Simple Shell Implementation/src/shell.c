#include "shell_util.h"
#include "linkedList.h"
#include "helpers.h"

// Library Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

int reapflag = 0;
List_t bg_list;

void sigchld_handler(int s){
	reapflag = 1;
}

void sigusr1_handler(int s){
	node_t* n;
	for(n = bg_list.head; n!=NULL; n=n->next){
		printBGPEntry((ProcessEntry_t*)n->value);
	}
}

int main(int argc, char *argv[])
{
	int i; //loop counter
	char *args[MAX_TOKENS + 1];
	int exec_result;
	int exit_status;
	pid_t pid;
	pid_t wait_result;
	sigset_t mask_all, mask_child, prev_one;	

	sigfillset(&mask_all);
	sigemptyset(&mask_child);
	sigaddset(&mask_child, SIGCHLD);

    //Initialize the linked list
    bg_list.head = NULL;
    bg_list.length = 0;
    bg_list.comparator = timeComparator;  // Don't forget to initialize this to your comparator!!!

	// Setup segmentation fault handler
	if(signal(SIGSEGV, sigsegv_handler) == SIG_ERR)
	{
		perror("Failed to set signal handler");
		exit(-1);
	}

	if(signal(SIGCHLD, sigchld_handler) == SIG_ERR){
		perror("Failed to set sigchld handler");
		exit(-1);
	}
	if(signal(SIGUSR1, sigusr1_handler) == SIG_ERR){
		perror("Failed to set sigusr1 handler");
		exit(-1);
	}

	while(1) {
		// DO NOT MODIFY buffer
		// The buffer is dynamically allocated, we need to free it at the end of the loop
		if(reapflag){
			while((pid = waitpid(-1, &exit_status, WNOHANG)) > 0){
				ProcessEntry_t* p = findPeFromList(&bg_list, pid);				
				fprintf(stdout, BG_TERM, p->pid, p->cmd); 
				removeByPid(&bg_list, pid);
			}
			reapflag = 0;
		}
		char * const buffer = NULL;
		size_t buf_size = 0;

		// Print the shell prompt
		display_shell_prompt();
		
		// Read line from STDIN
		ssize_t nbytes = getline((char **)&buffer, &buf_size, stdin);

		// No more input from STDIN, free buffer and terminate
		if(nbytes == -1) {
			free(buffer);
			break;
		}

		// Remove newline character from buffer, if it's there
		if(buffer[nbytes - 1] == '\n')
			buffer[nbytes- 1] = '\0';

		// Handling empty strings
		if(strcmp(buffer, "") == 0) {
			free(buffer);
			continue;
		}

		char* cmd_args = (char*)malloc(buf_size);
		strcpy(cmd_args, buffer);
	
		// Parsing input string into a sequence of tokens
		size_t numTokens;
		*args = NULL;
		numTokens = tokenizer(buffer, args);


		if(strcmp(args[0],"exit") == 0) {
			// Terminating the shell
			free(buffer);
			if(bg_list.length != 0){
				int i;
				int len = bg_list.length;
				for(i=0; i<len; i++){
					ProcessEntry_t* n = (ProcessEntry_t*)removeFront(&bg_list);
					kill(n->pid, SIGKILL);
					waitpid(pid, &exit_status, 0);
					fprintf(stdout, BG_TERM, n->pid, n->cmd);
					free(n); 
				}
			}
			return 0;
		}

		if(strcmp(args[0],"cd") == 0){
			char* cwd = (char*)malloc(100*sizeof(char));
			if(numTokens == 1){
				chdir(getenv("HOME"));
				getcwd(cwd, 100*sizeof(char));
				fprintf(stdout, "%s\n", cwd);
			}else{
				if(chdir(args[1]) != 0){
					fprintf(stderr, DIR_ERR);
				}else{
					getcwd(cwd, 100*sizeof(char));
					fprintf(stdout, "%s\n", cwd);
				}
			}
			free(cwd);
			continue;
		}
		
		if(strcmp(args[0], "estatus") == 0){
			printf("%d\n", WEXITSTATUS(exit_status));
			continue;
		}


		int rdf=-1, pf=-1;
		int redir_in = -1;
		int redir_out = -1;
		int redir_err = -1;
		int redir_dout = -1;
		int argc = numTokens;

		if(numTokens > 1 && strcmp(args[numTokens-1], "&")==0){
			argc--;
		}
		int i;
		int lf=0, rf=0, erf=0, errf=0;
		int pipefd[2];
		//char* f = malloc(50*sizeof(char));
		//strcpy(f, "");
		for(i=1; i<argc; i++){
			if(strcmp(args[i], "<") == 0){
				if(rdf == -1){
					rdf = i;
				}
				if(lf | i==argc-1){
					fprintf(stderr, RD_ERR);
					errf = 1;
					break;
				}else{
					if(strcmp(args[i+1], "<") == 0 | strcmp(args[i+1], "2>")==0|
					   strcmp(args[i+1], ">>") == 0| strcmp(args[i+1], ">") == 0){
						fprintf(stderr, RD_ERR);
						errf = 1;
						break;
					}
					redir_in = i+1;
					lf = 1;
				}  
			}else if(strcmp(args[i],">") == 0){
				if(rdf == -1){
					rdf = i;
				}
				if(rf | i==argc-1){
					fprintf(stderr, RD_ERR);
					errf = 1;
					break;		
				}else{
					if(strcmp(args[i+1], "<") == 0 | strcmp(args[i+1], "2>")==0|
					   strcmp(args[i+1], ">>") == 0| strcmp(args[i+1], ">") == 0){
						fprintf(stderr, RD_ERR);
						errf = 1;
						break;
					}
					redir_out = i+1;
					rf = 1;
				}
			}else if(strcmp(args[i], "2>") == 0){
				if(rdf == -1){
					rdf = i;
				}
				if(erf | i==argc-1){
					fprintf(stderr, RD_ERR);
					errf = 1;
					break;
				}else{
					if(strcmp(args[i+1], "<") == 0 | strcmp(args[i+1], "2>")==0|
					   strcmp(args[i+1], ">>") == 0| strcmp(args[i+1], ">") == 0){
						fprintf(stderr, RD_ERR);
						errf = 1;
						break;
					}
					redir_err = i+1;
					erf = 1;
				}
			}else if(strcmp(args[i], ">>") == 0){
				if(rdf == -1){
					rdf = i;
				}
				if(rf | i==argc-1){
					fprintf(stderr, RD_ERR);
					errf = 1;
					break;
				}else{
					if(strcmp(args[i+1], "<") == 0 | strcmp(args[i+1], "2>")==0|
					   strcmp(args[i+1], ">>") == 0| strcmp(args[i+1], ">") == 0){
						fprintf(stderr, RD_ERR);
						errf = 1;
						break;
					}
					redir_dout = i+1;
					rf = 1;
				}
			}else if(strcmp(args[i], "|") == 0){
				if(pf == -1){
					pf = i;
				}else{
					fprintf(stderr, PIPE_ERR);
					errf = 1;
					break;
				}
				if(i==argc-1){
					fprintf(stderr, PIPE_ERR);
					errf = 1;
					break;
				}else{
					if(strcmp(args[i+1], "|") == 0){
						fprintf(stderr, PIPE_ERR);
						errf = 1;
						break;
					}		
				}
			}
		}
		//free(f);
		if(redir_in != -1 && redir_out != -1){
			if(strcmp(args[redir_in], args[redir_out])==0){
				fprintf(stderr, RD_ERR);
				continue;
			}
		}
		if(redir_in != -1 && redir_err != -1){
			if(strcmp(args[redir_in], args[redir_err])==0){
				fprintf(stderr, RD_ERR);
				continue;
			}
		}
		if(redir_in != -1 && redir_dout != -1){
			if(strcmp(args[redir_in], args[redir_dout])==0){
				fprintf(stderr, RD_ERR);
				continue;
			}
		}
		if(redir_out != -1 && redir_err != -1){
			if(strcmp(args[redir_out], args[redir_err])==0){
				fprintf(stderr, RD_ERR);
				continue;
			}
		}
		if(redir_err != -1 && redir_dout != -1){
			if(strcmp(args[redir_err], args[redir_dout])==0){
				fprintf(stderr, RD_ERR);
				continue;
			}
		}
		if(errf){
			continue;
		}
		sigprocmask(SIG_BLOCK, &mask_child, &prev_one);
		pid = fork();   //In need of error handling......
		
		if (pid == 0){ //If zero, then it's the child process
			sigprocmask(SIG_SETMASK, &prev_one, NULL);
			pid_t gcpid;
			if(numTokens > 1 && strcmp(args[numTokens-1], "&")==0){
				args[numTokens-1] = NULL;
				numTokens--;
			}
			if(pf != -1){
				args[pf] = NULL;
				if(pipe(pipefd) == -1){
					fprintf(stderr, PIPE_ERR);
					errf = 1;
					break;
				}
				gcpid = fork();
				if(gcpid == 0){ //grandchild
					close(pipefd[1]);
					dup2(pipefd[0], STDIN_FILENO);
					exec_result = execvp(args[pf+1], &args[pf+1]);
					close(pipefd[0]);
					if(exec_result == -1){ //Error checking
						printf(EXEC_ERR, args[0]);
						exit(EXIT_FAILURE);
					}
		    			exit(EXIT_SUCCESS);
				}else{ //child
					close(pipefd[0]);
					dup2(pipefd[1], STDOUT_FILENO);
				}
			}
			if(rdf != -1){
				args[rdf] = NULL;
				int fd;
				if(redir_in != -1){
					if((fd = open(args[redir_in], O_RDONLY)) >= 0){
						dup2(fd, STDIN_FILENO);
					}else{
						fprintf(stderr, RD_ERR);
						exit(EXIT_FAILURE);
					}

				}
				if(redir_out != -1){
					if((fd = open(args[redir_out], O_WRONLY|O_CREAT, 0777)) >= 0){
						dup2(fd, STDOUT_FILENO);
					}else{
						fprintf(stderr, RD_ERR);
						exit(EXIT_FAILURE);
					}

				}
				if(redir_err != -1){
					if((fd = open(args[redir_err], O_WRONLY|O_CREAT, 0777)) >= 0){
						dup2(fd, STDERR_FILENO);
					}else{
						fprintf(stderr, RD_ERR);
						exit(EXIT_FAILURE);
					}
				}
				if(redir_dout != -1){
					if((fd = open(args[redir_dout], O_WRONLY|O_CREAT|O_APPEND, 0777)) >= 0){
						dup2(fd, STDOUT_FILENO);
					}else{
						fprintf(stderr, RD_ERR);
						exit(EXIT_FAILURE);
					}
				}
			}
			exec_result = execvp(args[0], &args[0]);
			if(pf != -1){
				close(pipefd[1]);
				waitpid(gcpid, NULL, 0);
			}
			if(exec_result == -1){ //Error checking
				printf(EXEC_ERR, args[0]);
				exit(EXIT_FAILURE);
			}
		    	exit(EXIT_SUCCESS);
		}else{ // Parent Process
			if(numTokens > 1 && strcmp(args[numTokens-1],"&")==0){
				sigprocmask(SIG_BLOCK, &mask_all, NULL);
			
				ProcessEntry_t* pe = (ProcessEntry_t*)malloc(sizeof(ProcessEntry_t));
				pe->cmd = cmd_args;
				pe->pid = pid;
				time(&(pe->seconds));

				insertInOrder(&bg_list, pe);
			
				sigprocmask(SIG_SETMASK, &prev_one, NULL);
				
			}else{
				wait_result = waitpid(pid, &exit_status, 0);
				if(wait_result == -1){
					printf(WAIT_ERR);
					exit(EXIT_FAILURE);
				}
			}
		sigprocmask(SIG_SETMASK, &prev_one, NULL);
		// Free the buffer allocated from getline
		free(buffer);
		}
	}
	return 0;
}

