#include <stdio.h>
#include <string.h>

int main(){
	char* command;
	scanf("%[^\n]",command);
	getchar();//to discard the newline character from STDIN
	printf("%s\n", command);
	scanf("%[^\n]",command);
	getchar();//to discard the newline character from STDIN
	printf("%s\n", command);
	printf("%d\n", strcmp(command, "daksh")); 
	return 0;
}