#include<stdio.h>
void main(){
	int c;
c= getchar();
while(c!=EOF){
	c=getchar();
}
if(c==EOF){
	printf("%d",c);}}
