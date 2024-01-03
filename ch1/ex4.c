#include<stdio.h>
void main(){
	int fahr,cel;
for(cel=0;cel<=100;cel=cel+1){
	fahr=((cel/5)*9)+32;
	printf("cel=%d,fahr=%d",cel,fahr);}}
