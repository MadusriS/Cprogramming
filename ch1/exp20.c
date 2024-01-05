#include<stdio.h>
#define tabsp 8
void main(){
	int c;
	int pos=0;
	int nosp;
	while((c=getchar())!=EOF){
		if(c=='\t'){
			nosp=((pos)%tabsp);
		pos=0;}

		while(nosp>0){
			putchar('#');
			nosp--;
		}
                if(c=='\n'){
                        pos=0;}
                else{

                        putchar(c);
                        pos++;}
	}}



