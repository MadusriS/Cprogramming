#include<stdio.h>
void main(){
	int c;
	int arr[26]={0};
	while((c=getchar())!=EOF){
		if(c!=' ' && c!='\n'){
			arr[c-'a']++;}
	}
	for(int i=0;i<26;i++){
		 
	printf("%c",(char)(97+i));
	for(int j=0;j<arr[i];j++)
	{
		printf("|");}
	putchar('\n');
	}}
