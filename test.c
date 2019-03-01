#include <stdio.h>
#include <string.h>

int main(){
	char* command;
	scanf("%[^\n]",command);
	getchar();//to discard the newline character from STDIN
	printf("strLen: %lu\n", strlen(command));
	printf("%d\n", command[strlen(command)]);
	return 0;
}