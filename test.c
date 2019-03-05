#include <stdio.h>
#include <string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>  

int main(){
	// int fdOpened = open("out",O_WRONLY|O_CREAT);
	// int fdOpened = open("out",O_RDONLY);
	// int fdOpened = open("out",O_WRONLY|O_CREAT);
	int fdOpened = open("out",O_APPEND|O_CREAT);
	printf("%d\n", fdOpened);
	return 0;
}