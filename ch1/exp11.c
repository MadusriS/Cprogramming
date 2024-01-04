#include<stdio.h>
void main(){
	int c ,flag;
	flag=0;
	while((c=getchar())!=EOF){
		if (c==' '|| c=='\n' || c=='\t'){
			if (flag==1){
				putchar('\n');
				flag=0;}}
			else if (flag==0){
				flag=1;
				putchar(c);
			}
			else {
				putchar(c);}}}
