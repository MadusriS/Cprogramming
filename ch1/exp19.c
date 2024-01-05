#include<stdio.h>
#define maxl 2000
void rev(char l[]);
int _getline(char l[],int maxli);
void main(){
int len;
char l[maxl];
       	while((len=_getline(l,maxl))>0){
		rev(l);printf("%s",l);}}


int _getline(char l[],int maxli){
	int i,c;
	for(i=0;i<maxl-1 && (c=getchar())!=EOF && c!='\n';i++){
		l[i]=c;
	}
	if(c=='\n'){
		l[i]=c;
		++i;
	}
	l[i]='\0';
	return i;}
void rev(char l[]){
	int i;
	for( i=0;l[i]!='\0';i++);
	--i;
	int j=0;
	char temp;
	while(j<i){
		temp=l[i];
		l[i]=l[j];
		l[j]=temp;
	++j;--i;}}
