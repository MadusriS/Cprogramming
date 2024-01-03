#include<stdio.h>
void main(){
	int fahr,cel;
	for(fahr=300;fahr>=0;fahr=fahr-20){
		cel=5*(fahr-32)/9;
		printf("%d\t%d\n",fahr,cel);
	}}
