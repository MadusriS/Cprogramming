/*#include<stdio.h>
#include<string.h>
int f=0;
void squeeze(char s1[30],char s2[30]){
	for(int j=0;j<strlen(s1);j++){
	for(int i=0;i<strlen(s2);i++){
		if(s1[j]==s2[i]){
			f=1;
		break;}}
	if(f==0)putchar(s1[j]);
	f=0;	}
}
void main(){
	char s1[30];
	char s2[30];
	printf("enter string 1 and string 2");
	scanf("%s,%s",s1,s2);
	squeeze(s1,s2);}*/
#include<stdio.h>
#include<string.h>
int f=0;
void squeeze(char s1[30],char s2[30]){
        for(int j=0;j<strlen(s1);j++){
        for(int i=0;i<strlen(s2);i++){
                if(s1[j]==s2[i]){
                        f=1;
			printf("%c %c %d \n",s1[j],s2[i],f);
                break;}}
        if(f==0)putchar(s1[j]);
        f=0;
        }
}
void main(){
        char s1[30];
        char s2[30];
        printf("enter string 1 and string 2");
        scanf("%s %s",s1,s2);
        squeeze(s1,s2);}

