#include<stdio.h>
int getline(char s[],int lim)
{
    int c,i;
    for(i=0;i<lim-1&&(c=getchar())!=EOF&&c!='\n';++i)
        s[i]=c;
    if(c=='\n')
    {
        s[i]=c;
        ++i;
    }
    if(c!=EOF)
    {
        while((c=getchar())!=EOF&&c!='\n')
            i++;
    }
    s[i]='\0';
    return i;
}
    #define MAXLINE 1000
    int len;
    int max;
    char line[MAXLINE];
    char longest[MAXLINE];

    max=0;
    while((len=getline(line,MAXLINE))>1)
    {
        if(len>max)
        {
            max=len;
            copy(longest,line);
        }
    }
    if(max>0)
    {
        printf("%d:%s",max,longest);
    }
    return 0;

