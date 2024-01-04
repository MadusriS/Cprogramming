#include<stdio.h>
void main(){
	int sp,nl,tab;
	sp=0;
	nl=0;
	tab=0;
	int c;
	while((c=getchar())!=EOF){
		if(c==' ') sp++;
		if(c=='\n') nl++;
		if(c=='\t') tab++;
	}
	printf("there are %d tabs,%d space ,%d newlines",tab,sp,nl);} 
