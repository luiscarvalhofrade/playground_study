/*
	This document aims to explain how to deal with multiple calls of 
	external commands using pipes like bash does.
	
	First, we need to understand how to deal with forks to create 
	child process and pipes to create communication channels between
	processes, that are achieved by using fds.

	These series of documents seek to start developing basic knowledge
	and in a incremental way achieve more complex and real scenarios.	
*/

/*
	The basic way to deal with pipes and forks is to read their manual 
	on terminal. Reading the pipe manual, they wrote a small example to 
	illustrate a simple and basic way to use both pipe and for. I'm going
	to replicate it here and I'm going to split all the important parts
	and try to explain it in a more direct way.
*/

#include <sys/types.h> //
#include <sys/wait.h> //
#include <stdio.h> //
#include <stdlib.h> //
#include <unistd.h> //
#include <string.h> //

int	main(int argc, char *argv[])
{
	int		pipefd[2];
	pid_t	cpid;
	char	buf;

	// this first part is just to handle argc input
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <string>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	/*
		first call of pipe. Its parameters is pipefd, an array with 2 positions
		this array is initialized when passed as param to pipe(), basiclly the function pipe
		generates 2 pairs of fd, the first position, of pipefd[0] has a number reserved
		as fd to read and the second to be used for writing an info between processes.

		it is important to understand something really important about fds. They are 
		int numbers between 0 and 1024 that always when they are need, the system
		block and defines one specific number in a special place (u_block list) that wherever
		you are dealing with fds in the system that number represents a specific fd.
		So you cant have 2 process with the same fd and they are dealing with different
		documents for example.
	*/
	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	/*
		Thats the "magic" of pipe and fork. Basically, when you call fork(), you create a
		child process from the parent that run in different memory spaces, but have the
		same content, so child processes are copies from the parent. because the function
		called pipe(pipefd) before forking, a copy of array pipefd[2] is also copied to the child
		process and it is the way to communicate between parent and child process.
		Always, pipefd[0] is used as the fd to read something passed by the other process and
		when you have something to pass to the other process, you are going to use pipefd[1].
	*/
	cpid = fork();
	if (cpid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}
	/*
		when you call the fork, it generates a data type pid_t that has 2 numbers, and really 
		oversimplying this part, when you have a pid_t == 0, it is pointing to the child
		process and the other one number, is the parent. Because that, here in this example the if statment
		target the child process and the else the parent process.
	*/
	if (cpid == 0) {    /* Child reads from pipe */
		close(pipefd[1]);          /* Close unused write end */

		while (read(pipefd[0], &buf, 1) > 0)
			write(STDOUT_FILENO, &buf, 1);

		write(STDOUT_FILENO, "\n", 1);
		close(pipefd[0]);
		_exit(EXIT_SUCCESS);
	
	}
	/*
		this example is really strightfoward -> parent process literally write
		the arg[1] passed when the program starts using its pipefd[1] copy and
		the child process read its using its copy of pipefd[0]. But this order
		isnt necessary... it is possible to both processes exchange information,
		it is only necessary to respect the fact that pipefd[0] is only to read
		and pipefd[1] is the fd passed to write something.
	*/
	else
	{            /* Parent writes argv[1] to pipe */
		close(pipefd[0]);          /* Close unused read end */
		write(pipefd[1], argv[1], strlen(argv[1]));
		close(pipefd[1]);          /* Reader will see EOF */
		wait(NULL);                /* Wait for child */
		exit(EXIT_SUCCESS);
	}
}
