#include<stdio.h>

#include<string.h>
int any(char s1[30],char s2[30]){
	for(int i=0;i<strlen(s1);i++){
		for(int j=0;j<strlen(s2);j++){
			if(s1[i]==s2[j]){
				return i;}}}
	return -1;
		
}

void main(){
	char s1[30],s2[30];
	printf("enter the strings");
	scanf("%s %s ",s1,s2);
	printf("the first occurence of a character in string s2 %d", any(s1,s2));}
