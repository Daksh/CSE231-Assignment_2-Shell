#include <unistd.h> //for syscalls
#include <stdio.h> //for scanf, printf
#include <sys/wait.h> // waitpid
#include <string.h> //for strlen(), strcmp()
#include <stdlib.h> //for malloc()
#include <stdbool.h> //for bool datatype
#include <ctype.h> //isspace()
#include <assert.h> //assertions

#define PROMPT "$ "
#define ASSERTF true
#define MAXCMDSIZE 4096
#define MAXARGS 20

void strip(char* str){
	int len = strlen(str); //strlen gives length of string, without '\0'

	//Setting startI and endI to the first and last non-empty characters
	int startI = 0;
	int endI = len-1;
	for(; startI < len && isspace(str[startI]); startI++);
	for(; endI >= 0 && isspace(str[endI]); endI--);
	
	if(endI==-1){
		str[0] = '\0';
		return;
	}

	if(ASSERTF) assert(str[len]=='\0');
	if(ASSERTF) assert(endI<len && endI>=0);
	if(ASSERTF) assert(startI<len && startI>=0);
	if(ASSERTF) assert(startI<=endI);

	int i;
	for(i = startI; i<=endI; i++)
		str[i-startI] = str[i];
	str[i-startI] = '\0';
}

void tokenize(char *argv[], char* command){
	//take care of '<', '<<', '|' and so on

	int len = strlen(command);
	int argvCounter = 0;

	for(int i=0; i<len;){
		argv[argvCounter++] = &command[i];
		while(i<len && !isspace(command[i])) i++;
		command[i++]='\0';
		while(i<len && isspace(command[i])) i++;
	}
	argv[argvCounter++]=NULL;
}

/*
 * Forks a child, and runs 'execvp' in it
 * We need this because, if we run it otherwise,
 * it would exit after execvp is over
 */
void execute(char * argv[]){
	char *cmd = argv[0];
	int pid, i, status;

	//Now two threads execute simultaneously (from next line itself)
	pid = fork();
	assert(pid>=0);
	if(pid == 0){ //Child
		if(execvp(cmd, argv)==-1)
			printf("ERROR: Cannot run the command\n");
		exit(0);//This command will not be reached, unless there is an error
	}
	else //Parent (pid > 0), pid here is of the child
		waitpid (pid, &status, 0);
}

int main (){
	char command[MAXCMDSIZE];

	while(true){
		printf(PROMPT);

		scanf("%[^\n]",command);
		getchar();//to discard the newline character from STDIN		
		strip(command);
		if(strcmp(command,"exit")==0)
			break;

		char *argv[MAXARGS];
		tokenize(argv, command);
				
		execute(argv);
	}
	
	return 0;
}

/*
1. In case strip does not work properly, can refer to Linux's implementation at https://stackoverflow.com/a/1488419/2806163
2. String slice: https://stackoverflow.com/a/26620524/2806163
3. isspace - https://www.programiz.com/c-programming/library-function/ctype.h/isspace
4. execlp - https://stackoverflow.com/a/26131243/2806163
5. execvp - https://stackoverflow.com/a/27542113/2806163
6. fgets (try it) - https://stackoverflow.com/a/3919126/2806163
*/