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
#define MAXPIPES 20
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
 * File Descriptors Mapping
 *	0 - STDIN
 *	1 - STDOUT
 *	2 - STDERR 
 */
void setRedirections(char *argv[]){
	for(int i=0; argv[i]!=NULL; i++){
		bool argConsumed = true;
		bool numBefore = false;

		if(strcmp(argv[i],">>")==0){

			//TODO: Add customized error
			if(ASSERTF) assert(argv[i+1]!=NULL);
			
			close(1);
            
			int fdOpened = open(argv[i+1],O_WRONLY|O_APPEND|O_CREAT, 0777);
			assert(fdOpened!=-1);
		} else if (strcmp(argv[i],">")==0){
			if(ASSERTF) assert(argv[i+1]!=NULL);
			
			//To check which fd should be closed
			if( i-1 >= 0 && (strcmp(argv[i-1],"1") == 0 || strcmp(argv[i-1],"2") == 0) ){
				numBefore = true;

				if(strcmp(argv[i-1],"1") == 0) //1>fname
					close(1);
				else if(strcmp(argv[i-1],"2") == 0) //2>fname
					close(2);	
			} else
				close(1);

			if(argv[i+1][0]=='&'){
				if(argv[i+1][1]=='1')
					dup(1);
				else
					dup(2);
			} else{
				int fdOpened = open(argv[i+1],O_WRONLY|O_CREAT|O_TRUNC, 0777);
				assert(fdOpened!=-1);	
			}
			
		} else if (strcmp(argv[i],"<")==0){
			if(ASSERTF) assert(argv[i+1]!=NULL);
			close(0);
			int fdOpened = open(argv[i+1],O_RDONLY, 0777);
			assert(fdOpened!=-1);
		} else argConsumed = false;

		if(argConsumed){
			if(numBefore){
				assert(i-1>=0);
				i-=1;
				del(argv,i);						
			}

			del(argv, i);
			del(argv, i);//i+1 in the original list
			i-=1;
		} 
		// if(DEBUGPRINTING)printf("\nArguments left:\n");
		// if(DEBUGPRINTING)printAllArgs(argv);
	}
}

/*
 * Forks a child, and runs 'execvp' in it
 * We need this because, if we run it otherwise,
 * it would exit after execvp is over
 */
void execute(char * argv[]){
	char *cmd = argv[0];
	int pid, i, status;
	
	if(strcmp(cmd,"cd")==0 || strcmp(cmd,"chdir")==0) {
		chdir(argv[1]);
		return;
	}

	//Now two threads execute simultaneously (from next line itself)
	setRedirections(argv);
	cmd = argv[0]; // in case the first argument was a redirection, we need to reset 

	if(execvp(cmd, argv)==-1)
		printf("ERROR: Cannot run the command\n");
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

void executeAll(char * argv[]){
	//Last argument of argv is NULL

	char** commands[MAXPIPES];
	
	int commandCounter = 0;
	int argCounter = 0;
	bool check = true;
	for(int i=0; argv[i]!=NULL; i++){
		if(check){
			commands[commandCounter++] = &argv[i];
			check = false;
		}
		if(argv[i][0]=='|' && argv[i][1]=='\0'){
			check = true;
			argv[i] = NULL;
		}
	}
	commands[commandCounter++]=NULL;
	
	int i,in, fd [2];
	in = 0;

	for (i = 0; commands[i+1]!=NULL; ++i){
		pipe (fd);
		
		if (!fork ()){
			if (in != 0){
				dup2 (in, 0);
				close (in);
			}
			if (fd[1] != 1){
				dup2 (fd[1], 1);
				close (fd[1]);
			}

			execute(commands[i]);
		}
		close (fd [1]);
		in = fd [0];
	}

	int pid, status;
	pid = fork ();
	if (pid == 0){
		if (in != 0)
			dup2 (in, 0);
		execute(commands[i]);
	} else waitpid (pid, &status, 0);
	
}

void sigintHandler(int sigNumber){
	printf("\n");
	printf(PROMPT);
	fflush(stdout); 
}

int main (){
	signal(SIGINT, sigintHandler); 

	//so that we can pad spaces when/where needed
	char command[3*MAXCMDSIZE];
	char temp[MAXCMDSIZE];

	while(true){
		printf(PROMPT);

		//Take input command in temp
		temp[0] = '\0';
		scanf("%[^\n]",temp);
		getchar();//to discard the newline character from STDIN		
		
		//Add spaces before and after delimiters
		padWithSpaces(temp, command);

		//Strip the whitespaces from start and end 
		strip(command);

		if(strcmp(command,"exit")==0)
			break;
		else if(command[0]=='\0') //if the string is only whitespaces
			continue;

		if(DEBUGPRINTING) printf("Command: *(%s)*\n", command);

		char *argv[MAXARGS*MAXPIPES];
		tokenize(argv, command);

		printAllArgs(argv);

		executeAll(argv);
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
7. https://docs.travis-ci.com/user/languages/c/
8. https://travis-ci.com/
9. https://www.google.com/search?ei=Dqx-XMn0GJXt9QOV7YbwAw&q=testing+c+code+with+python+tests&oq=testing+c+code+with+python+tests&gs_l=psy-ab.3..33i22i29i30.88085.91878..91959...0.0..0.193.4706.0j31......0....1..gws-wiz.......0i71j0i67j0i131j0j0i131i67j0i22i30.56Vw460rlWE#kpvalbx=0
10. https://ep2016.europython.eu/media/conference/slides/writing-unit-tests-for-c-code-in-python.html
11. http://luisquintanilla.me/2018/02/18/testing-deploying-python-projects-travisci/
12. https://github.com/keithn/seatest
13. https://github.com/danfis/cu
14. https://gist.github.com/Kimundi/10d299e905941e82bee4969acc7b64a8
15. https://github.com/rsp/travis-hello-modern-cpp
16. https://stackoverflow.com/questions/33342663/how-to-properly-setup-travisci-for-a-simple-c-project
17. https://github.com/alanxz/rabbitmq-c/blob/master/.travis.yml
18. https://docs.travis-ci.com/user/languages/c/
19. https://github.com/deftio/travis-ci-cpp-example/blob/master/makefile
20. https://www.google.com/search?q=understanding+signal+handlers+in+c+and+what+happens+when+you+fork&oq=understanding+signal+handlers+in+c+and+what+happens+when+you+fork&aqs=chrome..69i57.10177j0j9&sourceid=chrome&ie=UTF-8
21. https://www.geeksforgeeks.org/write-a-c-program-that-doesnt-terminate-when-ctrlc-is-pressed/
22. https://stackoverflow.com/questions/5113545/enable-a-signal-handler-using-sigaction-in-c
23. https://stackoverflow.com/questions/9084099/re-opening-stdout-and-stdin-file-descriptors-after-closing-them
24. https://stackoverflow.com/questions/13495783/if-fork-what-does-it-mean
25. https://www.quora.com/I-still-dont-understand-fork-especially-forking-multiple-times-I-dont-get-which-one-is-working-on-the-parent-or-the-child-Can-anyone-explain
26. https://www.careercup.com/question?id=12461680
27. https://stackoverflow.com/questions/8082932/connecting-n-commands-with-pipes-in-a-shell
28. https://ubuntuforums.org/showthread.php?t=963686
*/
