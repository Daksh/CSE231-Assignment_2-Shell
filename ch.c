#include <unistd.h>

struct command{
  const char **argv;
};

void spawn_proc (int in, int out, struct command *cmd) {
 	if (!fork ()){
		if (in != 0){
			dup2 (in, 0);
			close (in);
		}
		if (out != 1){
			dup2 (out, 1);
			close (out);
		}

		execvp (cmd->argv [0], (char * const *)cmd->argv);
	}
}

void fork_pipes (int n, struct command *cmd){
	int i;
	int in, fd [2];

	in = 0;

	for (i = 0; i < n - 1; ++i){
		pipe (fd);
		spawn_proc (in, fd [1], cmd + i);
		close (fd [1]);
		in = fd [0];
	}

	if (in != 0)
		dup2 (in, 0);

	execvp (cmd [i].argv [0], (char * const *)cmd [i].argv);
}

int main (){
	const char *ls[] = { "ls", "-l", 0 };
	const char *awk[] = { "awk", "{print $1}", 0 };
	const char *sort[] = { "sort", 0 };
	const char *uniq[] = { "uniq", 0 };

	struct command cmd [] = { {ls}, {sort}, {uniq} };

	fork_pipes (4, cmd);
	return 0;
}