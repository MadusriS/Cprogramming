/*#include<stdio.h>
void main(){
	int c;int i=0;
	char s[200];
	int lim=20;
	while((c = getchar()) != EOF) {
   if(c != '\n')
   {
   if(i < lim - 1){
   s[i] = c;
   i++;}}

	}
	s[i]='\0';
printf("%s",s);}*/
#include <stdio.h>

#define MAX_STRING_LENGTH 100

int main(void)
{	int i = 0,
		lim = MAX_STRING_LENGTH,
		c;
	char s[MAX_STRING_LENGTH];

	while (i < (lim - 1))
	{
		c = getchar();

		if (c == EOF)
			break;
		else if (c == '\n')
			break;

		s[i++] = c;
	}

	s[i] = '\0';
       printf("%s",s);	

	return 0;
}
