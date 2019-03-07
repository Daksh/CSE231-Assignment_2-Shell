#include <stdio.h>
#include <string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>  

int main(){
	char temp[10000];
	while(1){
		temp[0]='\0';
		scanf("%[^\n]",temp);
		getchar();//to discard the newline character from STDIN	
		printf("%s\n", temp);
	}
	return 0;
}