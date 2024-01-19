#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAXWORD 100

struct tnode {  
    char *word;    int count;     struct tnode *left;     struct tnode *right; 
};
struct words {
    char *words;
    int counts;
};


struct tnode *addtree(struct tnode *, char *);
int getword(char *, int);
void sort(struct words *); int countword;
int i = 0;



int main()
{
    struct tnode *root;
    char word[MAXWORD];
    
    root = NULL;
    while(getword(word, MAXWORD) != EOF)
        if(isalpha(word[0]))
            root = addtree(root, word);
    struct words sortword[countword];
    void fillarray(struct tnode *, struct words *); 
    fillarray(root, sortword);
    sort(sortword); 
    int j;
    for(j = 0; j < countword; j++)
        printf("%4d %s\n", sortword[j].counts, sortword[j].words);
    return 0;
}

void fillarray(struct tnode *p, struct words sortword[])
{
    if(p != NULL)
    {
        fillarray(p->left, sortword);
        sortword[i++] = (struct words) {p->word, p->count};
        fillarray(p->right, sortword);
    }
}

void sort(struct words sortword[])
{
    struct words temp;
    int i, j;
    for(i = 1; i<countword; i++)
    {
        temp = sortword[i];
        for(j = i-1; j >=0 && sortword[j].counts > temp.counts; j--)
        {
            sortword[j+1] = sortword[j];
            sortword[j] = temp;
        }
    }
}

struct tnode *talloc(void);
char *s_dup(char *s);


struct tnode *addtree(struct tnode *p, char *w)
{
    int cond;
    
    if(p == NULL)     {
        p = talloc();        p->word = s_dup(w);
        countword++;
        p->count = 1;
        p->left = p->right = NULL;
    }
    else if((cond = strcmp(w, p->word)) == 0)
        p->count++;     else if(cond < 0)     {
        p->left = addtree(p->left, w);
    }
    else      {
        p->right = addtree(p->right, w);
    }
    return p;
}


struct tnode *talloc(void)
{
    return(struct tnode *) malloc(sizeof(struct tnode));
}


char *s_dup(char *s)
{
    char *p;
    
    p = (char *)malloc(strlen(s)+1); 
    if(p != NULL)
        strcpy(p, s);
    return p;
}

#define BUFSIZE 100

char buf[BUFSIZE];          
int bufp = 0;               

int getch(void)  
{
   return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c) 
{
    if(bufp >= BUFSIZE)
        printf("ungetch: too many characnters\n");
    else
        buf[bufp++] = c;
}


int getword(char *word, int lim)
{
    int c, d; 
    int getch(void);
    void ungetch(int);
    int in_comment(void);
    char *w = word;
    
    while(isspace(c = getch()))
        ;
    if(c != EOF)
        *w++ = c;
    if(isalpha(c) || c == ' ' || c == '#')
    {
        for(;--lim > 0; w++)
            if(!isalnum(*w= getch()) && *w != '_')
            {
                ungetch(*w);
                break;
            }
    }
    else if(c=='\'' || c == '"')
    {
        for(; --lim > 0; w++)
            if((*w = getch()) == '\\')
                *++w = getch();
            else if(*w == c)
            {
                w++;
                break;
            }
            else if(*w == EOF)
                break;
    }
    else if(c == '/')
    {
        if(((d = getch()) == '*'))
            c = in_comment();
        else
            ungetch(d);
    }
    *w = '\0';
    return c;
}


int in_comment(void)
{
    int c;
    
    while((c = getch()) != EOF)
        if(c == '*')
        {
            if((c = getch()) == '/')
                break;
            else
                ungetch(c);
        }
    return c;
}
