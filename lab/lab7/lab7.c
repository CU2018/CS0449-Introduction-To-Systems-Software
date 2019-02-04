//siz24
//Siyu Zhang

#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		printf("uh, you gotta gimme an executable to run...\n");
		return 0;
	}

	if(fork() == 0)
	{
		// This will execute the program whose name is in the first
		// command-line argument, and whose arguments are after it.
		execvp(argv[1], &argv[1]);
		
		perror("Error running program: ");
		printf("\n\n");
		exit(1);

		/************* HERE **************
		You should use perror() to print an error message.
		Then use exit() to exit with a non-zero error exit code.

		ALWAYS ALWAYS ALWAYS ALWAYS exit() AFTER AN exec() CALL!!
		LEAVING THAT OUT WILL LIKELY LEAD TO FORKBOMBS IN PROJECT 4.
		*********************************/
	}
	else
	{
		signal(SIGINT, SIG_IGN);  //set SIGINT to SIG_IGN to ignore it
		/************* HERE **************
		You should use the signal() function to *ignore* SIGINT.
		(Don't worry about handling an error from signal() here, it almost can't fail :P)
		*********************************/

		// This waits for a child to exit in some way. *How* it exited
		// will be put in the status variable.
		int child_status;
		int childpid = waitpid(-1, &child_status, 0);
		printf("----------\n");
		
		if (childpid == -1) //waitpid return with error 
		{
			perror("Error running program: ");
			printf("\n\n");
		}
		else if (WIFEXITED(child_status))
		{
			int exit_code = WEXITSTATUS(child_status);
			if (exit_code == EXIT_SUCCESS)
				printf("Program Exited Successfully!\n\n");
			else
				printf ("Program exited with error code %d\n\n", WEXITSTATUS(child_status));
		}
		else if (WIFSIGNALED(child_status))
		{
			printf("Program was terminated by signal: %s\n\n", strsignal(WTERMSIG(child_status)));
		}
		else
		{
			printf("Program terminated some other way!\n\n");
		}

		/************* HERE **************
		- if waitpid() returned an error value,
			use perror().
		- else, if the child exited successfully (with an exit status of 0),
			print "exited successfully".
		- else, if the child exited, but with a non-zero exit status,
			print "exited with code %d", and print the exit status.
		- else, if the child terminated because of a signal,
			print "terminated due to signal %s", and use strsignal() to print the signal.
		- else
			print "terminated some other way!"
		*********************************/
	}

	return 0;
}
