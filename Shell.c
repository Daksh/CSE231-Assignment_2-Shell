#include <unistd.h> //for syscalls
#include <stdio.h> //for scanf, printf
#include <sys/wait.h>
#include <string.h> //for strlen(), strcmp()
#include <stdlib.h> //for malloc()
#include <stdbool.h> //for bool datatype
#include <ctype.h> //isspace()
#include <assert.h> 

#define PROMPT "$ "
#define ASSERTF true

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

int main (){
	char* command;

	while(strcmp(command,"exit")!=0){
		printf(PROMPT);

		scanf("%[^\n]",command);
		getchar();//to discard the newline character from STDIN		
		strip(command);
		
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
*/