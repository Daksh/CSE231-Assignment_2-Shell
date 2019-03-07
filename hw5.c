#include <stdio.h>
#include <unistd.h>

int main(){
	int pid; 
	int fd[2];
	int ret; 

	ret = pipe (fd); 
	if (ret == -1) {
		printf ("Unable to create pipe\n");
		return 0;
	}

	pid = fork ();	
	if (pid == 0) { 
		char* const args[] = {"/bin/ls", NULL};
		
		close(fd[0]); //[^1] Closing the read end of the pipe 
		
		close(1);//Closing STDOUT
		dup(fd[1]);//Duplicating Write End of pipe (to 1 - which is the lowest FD available)

		close(fd[1]); //[^1] Closing write end of the pipe

		ret = execv (args[0], args);
		
		/* NOT REACHED*/
		printf ("failed to exec ls\n");
	} else if (pid > 0) { 
		char* const args[] = {"/usr/bin/wc", "-l", NULL};
		
		close(fd[1]); 

		close(0); //Closing STDIN
		dup(fd[0]);//Duplicating Read end of pipe (to 0 - which is the lowest FD available)
		
		close(fd[0]); //[^1] Closing read end of the pipe

		execv (args[0], args);
		
		/* NOT REACHED*/
		printf ("failed to exec wc\n");
	} else { //Probably the fork() did not work properly
		printf ("Unable to fork\n");
	}
	return 0;
}