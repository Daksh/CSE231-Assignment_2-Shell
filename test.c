#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  

int main(){
	printf("hello\n");
	fprintf(stderr, "%s", "Stack overflow!\n"); 
	return 0;
}