#include<stdio.h>
#include<string.h>
void main(){
	char s[30];
	char t[30];
	printf("enter string");
	scanf("%s",s);
	int i=0;for(int j=0;j<strlen(s);j++){
		switch(s[j]){
			case '\t':
			t[i]='\\';
			++i;
			t[i]='t';
			++i;
			break;
			case '\n':
			t[i]='\\';
			++i;
			t[i]='n';
			++i;
			break;
			default:
			t[i]=s[i];
			++i;
		break;}}
	t[i]='\0';
	printf("%s",t);

}
