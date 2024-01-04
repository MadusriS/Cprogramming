#include<stdio.h>
void main(){
	int sp1=0;
	int c;
	while((c=getchar())!=EOF){
		if(sp1!=1 && c==' '){ sp1=1;putchar(c);}
		if(c!=' '){putchar(c);sp1=0;}}}

