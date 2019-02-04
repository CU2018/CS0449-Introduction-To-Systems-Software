//Siyu Zhang
//siz24
#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//global variables
int BUFFER_SIZE = 300;   //generous size of input buffer
const char* delim = " \t\n";   //legal delimiters 

//read arguments from the user
void read_command (char* buffer) 
{
	fgets(buffer, BUFFER_SIZE, stdin);
}
//get the number of arguments
int get_num_argument(char* buffer)  
{ 
	//need a copy of the buffer since strtok will modify the original array
	char copy[BUFFER_SIZE];
	stpcpy(copy, buffer);
	
	char* token = strtok(copy, delim);
	int i;
	for (i = 0; token != NULL; i++)
	{
		token = strtok(NULL, delim);
	}
	return i;
}
//parse user's input into tokens
void parse_command(char* buffer, char** argv)	
{
	//need a copy of the buffer since strtok will modify the original array
	char copy[BUFFER_SIZE];
	stpcpy(copy, buffer);
	
	char* token = strtok(copy, delim);
	int i;
	for(i = 0; token != NULL; i++)
	{
		argv[i] = (char*)malloc(sizeof(char) * strlen(token)); //since argv is a point to an array of string
		strcpy(argv[i], token);  //copy the string to current slot
		token = strtok(NULL, delim); 
	}
	argv[i] = NULL; //for execvp to detect how many arguments you've given
}
//buildin: exit
int check_exit(const char* arg0, const char* arg1)	
{
	if ((strcmp(arg0, "exit") == 0))  //user enters with exit code: exit number
	{
		if (arg1 != NULL)
			return atoi(arg1);
		else 
			return 0;
	}
	else
		return -1;
}
//buildin: cd
int check_cd (const char* arg0, const char* arg1)	
{
	if (strcmp(arg0, "cd") == 0) //user tries to use the cd function
	{
		if (chdir(arg1) == -1) //chdir failed to change the working directory
		{
			if (arg1 == NULL)  //missing the directory, ask the user to type in
				printf("cd:  : Please enter a directory\n");
			else  //cannot find the directory
			{
				printf("cd: %s: No such file or directory\n", arg1);
			}
		}
		return 0;
	}
	else
		return -1;
}
//check whether the input has more than one redirection markers
int check_double_redirect(int argc, char** argv)
{
	int input = 0;
	int output = 0;
	for (int i = 1; i < argc - 1; i++)
	{
		if (strcmp(argv[i], "<") == 0) 
			input++;
		else if (strcmp(argv[i], ">") == 0) 
			output++;
	}
	if (input > 1 || output > 1) //the user tried to redirect stdin or stdout more than once
		return -1;
	else 
		return 0;
}
//open the redirection file by using freopen
int open_redirection(int argc, char** argv)
{
	for (int i = 1; i <= argc - 1; i++)
	{
		if (strcmp(argv[i], "<") == 0)   //current token is < input redirection, read-only
		{
			if (freopen(argv[i + 1], "r", stdin) != NULL)  //successfully open the file
			{
				free(argv[i]);
				argv[i] = NULL;
			}
			else //failed to open the file
			{
				return -1;
			}
		}
		else if (strcmp(argv[i], ">") == 0)  //current token is > output redirection, write-only
		{
			if (freopen(argv[i + 1], "w", stdout) != NULL)  //successfully open the file
			{
				free(argv[i]);
				argv[i] = NULL;
			}
			else //failed to open the file
			{
				return -1;
			}
		}
	}
	return 0;
}
//run the regular program by parent and child processes
void run_regular_program(int argc, char** argv)
{
	if (fork() != 0)  //this is the parent process
	{
		int child_status;
		int child_pid = waitpid(-1, &child_status, 0);
		
		if (child_pid == -1)  //waitpid return with error
		{
			perror("Error running program");
		}
		else if (!WIFEXITED(child_status))  //child process exited abnormally
		{
			if (WIFSIGNALED(child_status))  //child process is interrupted by a signal
			{
				printf("\nProgram was terminated by signal: %s\n", strsignal(WTERMSIG(child_status)));
			}
			else
			{
				printf("Program terminated abnormally\n");
			}
		}
		else if (WIFEXITED(child_status))
		{
			int exit_code = WEXITSTATUS(child_status);
			if (exit_code == 2)
			{
				printf("-myshell: Cannot open the redirection file\n");
			}
		}
	}
	else //this is the child process: fork()==0
	{
		signal(SIGINT, SIG_DFL);     //for successfully using CTRL+C to end the child process only
		
		if (check_double_redirect(argc, argv) == -1)  //the user tried to redirect stdin or stdout more than once
		{
			printf("-myshell: You cannot redirect stdin or stdout more than once\n");
			exit(-1);
		}
		
		int check_redirection = open_redirection(argc, argv);		//open redirection files
		if (check_redirection == 0)  //successfully modify the argv, could run the regular program
		{
			execvp(argv[0], argv);  //run the regular program
		
			perror("Error running program");  //print error message if execvp failed
			exit(1);
		}
		else  //fail to open the file, send 2 to parent to print error message
		{
			exit(2);  
		}
	}
}

int main()
{
	//a list of variables
	char buffer[BUFFER_SIZE];
	int argc;
	char** argv;
	
	signal(SIGINT, SIG_IGN);  //set SIGINT to SIG_IGN to ignore it
	
	while (1)
	{
		printf("myshell> ");
		read_command(buffer);
		
		argc = get_num_argument(buffer);
		argv = malloc(sizeof(char*) * (argc + 1));  //malloc a char** with a correct size, one more slot for setting NULL
		parse_command(buffer, argv);
		
		//if no arguments entered, continue asking for it
		if (argv[0] == NULL)  
			continue;
		//buildin: exit
		int exit_code = check_exit(argv[0], argv[1]);
		if (exit_code != -1)
		{
			return exit_code;
		}
		//buildin: cd
		int typing_cd = check_cd(argv[0], argv[1]);
		if (typing_cd == -1) //the user does not enter cd, begin to run the regular programs
		{
			run_regular_program(argc, argv);
		}
	}
		
	return 0;
}