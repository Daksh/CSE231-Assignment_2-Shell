#include <unistd.h> //for syscalls
#include <stdio.h> //for scanf, printf
#include <sys/wait.h>
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
	
	//make "dfsd fs df sd f" as one arg
	// also ' '
	
	int len = strlen(command);
	int argvCounter = 0;

	for(int i=0; i<len;){
		// printf("Setting start of arg #%d at %dth index\n", argvCounter, i);
		argv[argvCounter++] = &command[i];
		while(i<len && !isspace(command[i])) i++;
		// printf("Adding a NULL Char on %dth index\n",i);
		// printf("%c at char\n", command[i]);
		command[i++]='\0';
		while(i<len && isspace(command[i])) i++;
	}
	argv[argvCounter++]=NULL;
	// printf("Total args: %d\n", argvCounter);
}

int main (){
	char command[MAXCMDSIZE];

	while(strcmp(command,"exit")!=0){
		printf(PROMPT);

		scanf("%[^\n]",command);
		getchar();//to discard the newline character from STDIN		
		strip(command);

		char *argv[MAXARGS];
		tokenize(argv, command);
		
		// for(int i=0; argv[i]!=NULL; i++)
		// 	printf("%dth argument: %s\n", i,argv[i]);
		char *cmd = argv[0];

		execvp(cmd, argv);
	}
	// int pid, i, status;
	// printf ("main %d parent %d\n", getpid(), getppid());
	// for (i = 0; i < 3; i++) {
	// pid = fork ();
	// if (pid < 0) {
	// printf ("Unable to fork\n");
	// return 0;
	// }
	// if (pid != 0) {
	// waitpid (pid, &status, 0);
	// }
	// }
	// printf ("process %d (parent %d) is terminating\n", getpid(), getppid());
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