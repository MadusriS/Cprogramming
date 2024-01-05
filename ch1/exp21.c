#include<stdio.h>
void main(){
	int c;
	int f=1;
	while((c=getchar())!=EOF){
		if(c==' '){
			f++;
		       	if(f%8==0){putchar('\t');}}
		else{
			if(f%8!=0){
				for(int i=0;i<f;i++) putchar(' ');}
			putchar(c);
			f=0;}
		}}
