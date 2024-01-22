#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>  
#define MAXWORD 100
#define HASHSIZE 101
struct nlist {
    struct nlist *next;
    char *name;
    char *defn;
};
struct nlist *hashtab[HASHSIZE];  
unsigned hash(char *s); 

void error(int, char*);
int getch(void);
void getdef(void);
int getword(char *, int);
struct nlist *install(char *, char *);
struct nlist *lookup(char *);
void skipblanks(void);
int undef(char *);  
void ungetch(int);
void ungets(char *);
int main()
{
    char w[MAXWORD];
    struct nlist *p;
    while (getword(w, MAXWORD) != EOF)
        if (strcmp(w, "#") == 0)
            getdef();
        else if (!isalpha(w[0]))
            printf("%s", w);
        else if ((p = lookup(w)) == NULL)
            printf("%s", w);
        else
            ungets(p->defn);
    return 0;  
}
void getdef()
{
    int c, i;
    char def[MAXWORD], dir[MAXWORD], name[MAXWORD];
    skipblanks();
    if (!isalpha(getword(dir, MAXWORD)))
        error(dir[0], "getdef: expecting a directive after #");
    else if (strcmp(dir, "define") == 0) {
        skipblanks();
        if (!isalpha(getword(name, MAXWORD)))
            error(name[0], "getdef: non alpha name expected");
        else {
            skipblanks();
            for (i = 0; i < MAXWORD - 1; i++)
                if ((def[i] = getch()) == EOF || def[i] == '\n')  // Corrected 'n' to '\n'
                    break;
            def[i] = '\0';
            if (i <= 0)
                error('n', "getdef: non alpha in define");
            else
                install(name, def);
        }
    } else if (strcmp(dir, "undef") == 0) {
        skipblanks();
        if (!isalpha(getword(name, MAXWORD)))
            error(name[0], "getdef: non alpha in undef");
        else
            undef(name);
    } else
        error(dir[0], "getdef: expecting a directive after #");
}
int undef(char *name)
{
    struct nlist *np1, *np2;

    if ((np1 = lookup(name)) == NULL) 
        return 1;

    for (np1 = np2 = hashtab[hash(name)]; np1 != NULL;
         np2 = np1, np1 = np1->next) {
        if (strcmp(name, np1->name) == 0) { 
            if (np1 == np2)
                hashtab[hash(name)] = np1->next;
            else
                np2->next = np1->next;

            /* Free memory */
            free(np1->name);
            free(np1->defn);
            free(np1);

            return 0;
        }
    }

    return 1; 
}
void error(int c, char *s)
{
    printf("error:%s\n", s);
    while (c != EOF && c != '\n')  
        c = getch();
}
void skipblanks()
{
    int c;
    while ((c = getch()) == ' ' || c == '\t')  
        ;
    ungetch(c);
}
void ungets(char s[])
{
    int len = strlen(s);
    void ungetch(int);
    while (len > 0)
        ungetch(s[--len]);
}


