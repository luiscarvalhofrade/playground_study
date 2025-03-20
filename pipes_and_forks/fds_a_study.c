/*
	Basic principle: alls close a fd when it is not necessary 
	anymore. Forgetting to close them is a problem.

	Scenarios for fds and how to deal with it
	1. <
		- redirect input, so you need to dup the STDIN_FILENO
		to make it the fd from where the system will call when 
		it is available to the command to receive input. 
	2. >
		- redirect outup, so you need to dup the STDOUT_FILENO
		to make it the fd generated when calling open which you 
		want to use as file to save the output in.
	3. >>
		
	4. <<

	5. pipefd
		- the idea here is that when you use pipe(pipefd)
		you are generating one way communication between
		to processes. If it is necessary to create biderectional
		communication it is necessary to call pipe twice 
		pipe(pipefd1) and pipe(pipefd2).


*/