#include <stdio.h>
#include <string.h>

int main(){
	char* command;
	scanf("%[^\n]",command);
	char* what;

	while(strcmp(command,"exit")!=0){
		printf("$ ");

		scanf("%[^\n]",command);
		getchar();//to discard the newline character from STDIN
	}
	return 0;
}