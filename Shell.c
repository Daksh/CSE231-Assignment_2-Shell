#include <unistd.h> //for syscalls
#include <stdio.h> //for scanf, printf
#include <sys/wait.h>
#include <string.h> //for strlen(), strcmp()
#include <stdlib.h> //for malloc()
#include <stdbool.h> //for bool datatype
// #include <ctype.h>

#define PROMPT "$ "

char* strip(char* str){
	int len = strlen(str);
	printf("strip: string Length %d\n", len);

	//Setting startI and endI to the first and last non-empty characters
	int startI = 0;
	int endI = len-1;
	for(; str[startI]==' ' || str[startI]=='\t'; startI++);
	for(; str[endI]==' ' || str[endI]=='\n' || str[endI]=='\t'; endI--);//maybe use isspace? from ctype.h
	printf("strip: ASCII of last character: %d\n", str[endI]);

	int newStrLen = endI-startI+1;
	
	//If there is nothing but spaces in the str, then we return empty string
	if(newStrLen<=0){
		char* newStr = (char*)malloc(sizeof(char));
		newStr[0] = '\0';
		return newStr;
	}

	char* newStr = (char*)malloc(sizeof(char)*newStrLen);//[endI-startI+1];

	for(int i=startI; i<=endI; i++)
		newStr[i-startI] = str[i];
	printf("strip: startI %d   endI %d\n", startI, endI);
	printf("strip: returning %s%s%s\n", "XXX",newStr,"XXX");
	return newStr;
}

int main (){
	char* command;
	bool exitFlag = false;
	printf("Space: %d\n", ' ');
	printf("Newline: %d\n", '\n');
	printf("Tab: %d\n", '\t');
	while(!exitFlag){
		printf(PROMPT);

		scanf("%[^\n]",command);
		getchar();//to discard the newline character from STDIN
		printf("%s%s%s\n", "XXX",command,"XXX");
		printf("main: strLen %lu\n", strlen(command));
		command = strip(command);
		printf("%s%s%s\n", "XXX",command,"XXX");

		exitFlag = strcmp(command,"exit")==0;
		// printf("%d\n", exitFlag);
		free(command);
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
*/