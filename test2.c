#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <assert.h> //assertions

int main(){
	printf("hello\n");
	assert(4==2);
	fprintf(stderr, "%s", "Stack overflow!\n"); 
	return 0;
}