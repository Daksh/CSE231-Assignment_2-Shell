#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main ()
{
	int pid, i, status;
	printf ("main %d parent %d\n", getpid(), getppid());
	for (i = 0; i < 3; i++) {
	pid = fork ();
	if (pid < 0) {
	printf ("Unable to fork\n");
	return 0;
	}
	if (pid != 0) {
	waitpid (pid, &status, 0);
	}
	}
	printf ("process %d (parent %d) is terminating\n", getpid(), getppid());
	return 0;
}