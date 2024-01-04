#include<stdio.h>
void main(){
	int c;
	int len=0;
	while((c=getchar())!=EOF){
		if(c!=' ' &&  c!='\n'){
			printf("|");}
		else{
			putchar('\n');}}}

