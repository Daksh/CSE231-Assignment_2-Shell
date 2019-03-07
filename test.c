#include <stdio.h>

void check(int x[]){
	printf("%d %d\n", x[0],x[1]);
}

int main(){
	int a[10][2];
	for(int i=0; i<10; i++)
		for(int j=0; j<2; j++)
			a[i][j]=i*10+j;

	for(int i=0; i<10; i++)
		check(a[i]);

	// for(int i=0; i<10; i++){
	// 	for(int j=0; j<2; j++)
	// 		printf("%d ", a[i][j]);
	// 	printf("\n");
	// }
	return 0;
}