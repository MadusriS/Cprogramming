/*#include<stdio.h>
void main(){
	char s[30];
	char t[30];
	scanf("%s %s",s,t);
	int l=strlen(s);
	int lt=strlen(t);
	char s1[30];
	int fnd=0;
	for(int i=l-1;i>=0;i--){
		for(int j=lt-1;j>=0;j--){
			if(s[i]==t[j] && j>=0){
				i--;j--;
			if(j==0 && s[i]==t[j]){ prinf("last occurence is %d",i);fnd =1;}}}
			if(fnd==1)break;}*/
#include<stdio.h>
#include<string.h>

int lstrindex(char s[],char t[]);

int main(void)
{
    char s[30] ;
    char t[30] ;

scanf("%s %s", s,t);
    int fnd;
   
     fnd = lstrindex(s,t);

    printf("Found the right index: %d\n", fnd);

}

int lstrindex(char s[],char t[])
{
    int i,j,k, result;
    result = -1;
    for(i=0;s[i]!='\0';i++)
    {
        for(j=i,k=0;t[k]!='\0' && s[j]==t[k];j++,k++);
        if(k>0 && t[k] == '\0')
            result = i;
    }
    return result;
}

