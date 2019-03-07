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

void sigintHandler(int sigNumber){
	// printf("sigintHandler called with signal #%d\n", sigNumber);
	// fflush(stdout); 
}

int main(){
	signal(SIGINT, sigintHandler); 
	int i=1;
	if(!fork()){
		//in child
		signal(SIGINT, sigintHandler); 
	}
	//parent
	while(1){
		i++;
		if(i%100000000==0) printf("%d\n", i);
	}
	return 0;
}