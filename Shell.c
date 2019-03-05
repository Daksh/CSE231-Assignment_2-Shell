#include <unistd.h> //for syscalls
#include <stdio.h> //for scanf, printf
#include <sys/wait.h> // waitpid
#include <string.h> //for strlen(), strcmp()
#include <stdlib.h> //for malloc()
#include <stdbool.h> //for bool datatype
#include <ctype.h> //isspace()
#include <assert.h> //assertions

//To open a file
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  

#define PROMPT "$ "
#define ASSERTF true
#define MAXCMDSIZE 4096
#define MAXARGS 20
#define DEBUGPRINTING false

/*
 * Prints the list of args in argv, before NULL
 * Only works when DEBUGPRINTING is true
 */
void printAllArgs(char *argv[]){
	if(DEBUGPRINTING)
		for(int i=0; argv[i]!=NULL; i++)
			printf("ARG #%d: *(%s)*\n", i,argv[i]);
}

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
 * Removes the argNumber'th argument from argv
 */
void del(char *argv[], int argNumber){
	for(int i=argNumber; argv[i]!=NULL; i++)
		argv[i]=argv[i+1];
}

/* 

File Descriptors Mapping
	0 - STDIN
	1 - STDOUT
	2 - STDERR 
 */
void setRedirections(char *argv[]){
	for(int i=0; argv[i]!=NULL; i++){
		bool argConsumed = true;
		if(strcmp(argv[i],">>")==0){

			//TODO: Add customized error
			if(ASSERTF) assert(argv[i+1]!=NULL);
			
			close(1);
			int fdOpened = open(argv[i+1],O_APPEND|O_CREAT);
			if(ASSERTF) assert(fdOpened!=-1);
			dup(fdOpened);
		} else if (strcmp(argv[i],">")==0){
			if(ASSERTF) assert(argv[i+1]!=NULL);
			close(1);
			int fdOpened = open(argv[i+1],O_WRONLY|O_CREAT);
			if(ASSERTF) assert(fdOpened!=-1);
			dup(fdOpened);
		} else if (strcmp(argv[i],"<")==0){
			if(ASSERTF) assert(argv[i+1]!=NULL);
			close(0);
			int fdOpened = open(argv[i+1],O_RDONLY);
			if(ASSERTF) assert(fdOpened!=-1);
			dup(fdOpened);
		} else argConsumed = false;

		if(argConsumed){
			del(argv, i);
			del(argv, i);//i+1 in the original list
		} 
	}
	if(DEBUGPRINTING)printf("\nArguments left:\n");
	if(DEBUGPRINTING)printAllArgs(argv);
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
		setRedirections(argv);
		if(execvp(cmd, argv)==-1)
			printf("ERROR: Cannot run the command\n");
		exit(0);//This command will not be reached, unless there is an error
	}
	else //Parent (pid > 0), pid here is of the child
		waitpid (pid, &status, 0);
}

/*
 * Adds a space before and after each delimiter, 
 * so that tokenizer can recognize them separately
 */
void padWithSpaces(char* from, char* to){
	int toStart = 0;
	for(int i = 0; i <= strlen(from); i++){
		if(from[i]=='<' || from[i]=='>' || from[i]=='|'){
			to[toStart++] = ' ';
			to[toStart++] = from[i++];
			if(from[i]=='>')
				to[toStart++] = from[i++];
			to[toStart++] = ' ';
		}
		to[toStart++] = from[i];
	}
}

int main (){
	//so that we can pad spaces when/where needed
	char command[3*MAXCMDSIZE];
	char temp[MAXCMDSIZE];

	while(true){
		printf(PROMPT);

		scanf("%[^\n]",temp);
		getchar();//to discard the newline character from STDIN		
		padWithSpaces(temp, command);
		strip(command);

		if(strcmp(command,"exit")==0)
			break;

		if(DEBUGPRINTING) printf("Command: *(%s)*\n", command);

		char *argv[MAXARGS];
		tokenize(argv, command);

		printAllArgs(argv);

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