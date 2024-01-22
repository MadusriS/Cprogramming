
/* Implement a simple version of the #define processor (i.e, no arguments)
 * suitable for use with  C programs, based on the routines of this section.
 * You may also find getch and ungetch helpful.
 */

/*
 * Use getword for #define, key and value
 * and use install routines to install it.
 *
 */
 /*sample input:
#define MAX_LENGTH 100
#define PI 3.14159

int main() {
    char str[MAX_LENGTH];
    printf("Enter the radius: ");
    scanf("%s", str);
    int radius = atoi(str);

    double area = PI * radius * radius;
    printf("The area of a circle with radius %d is: %f\n", radius, area);

    return 0;
}
  sample output:
MAX_LENGTH ->100  PI->3

*/
//REFER EXERCISE 5 FIRST////////////////////////

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXWORD 1000

int mgetword(char *, int, int *);



struct nlist {          /* table entry: */
    struct nlist *next; /* next entry in chain */
    char *name;         /* defined name */
    char *defn;         /* replacement text */
};

#define HASHSIZE 101

static struct nlist *hashtab[HASHSIZE]; 

unsigned hash(char *s) {
    unsigned hashval;

    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;

    return hashval % HASHSIZE;
}



struct nlist *lookup(char *s) {
    struct nlist *np;

    for (np = hashtab[hash(s)]; np != NULL; np = np->next)
        if (strcmp(s, np->name) == 0)
            return np; 
    return NULL; 
}

struct nlist *lookup(char *);
struct nlist *install(char *name, char *defn) {
    struct nlist *np;
    unsigned hashval;

    if ((np = lookup(name)) == NULL) { 
        np = (struct nlist *)malloc(sizeof(*np));

        if (np == NULL || (np->name = strdup(name)) == NULL)
            return NULL;
        hashval = hash(name);
        np->next = hashtab[hashval];
        hashtab[hashval] = np;
    } else                      
        free((void *)np->defn);

    if ((np->defn = strdup(defn)) == NULL)
        return NULL;

    return np;
}

struct nlist *undef(char *name) {
    struct nlist *found;

    found = lookup(name);

    if (found == NULL)
        return NULL;
    else {
        if (found->next != NULL) {
            found->next = found->next->next;
            found = found->next;
        } else {
            hashtab[hash(name)] = NULL;
            free((void *)found);
        }
    }
    return found;
}

int main(void) {

    int lineno = 0;
    char word[MAXWORD];

    char key[MAXWORD], value[MAXWORD];

    struct nlist *result;

    while (mgetword(word, MAXWORD, &lineno) != 'x') {
      
        if (strcmp(word, "#define ") == 0) {
            mgetword(key, MAXWORD, &lineno);
            mgetword(value, MAXWORD, &lineno);
            install(key, value);
            result = lookup(key);
            printf("%s->%s", result->name, result->defn);
        }
    }

    return 0;
}

;

#define IN 1
#define OUT 0

int mgetword(char *word, int lim, int *lineno_addr) {
    int c, d, getch(void), comment, string, directive;
    void ungetch(int);
    char *w = word;

    comment = string = directive = OUT;

    while (isspace(c = getch())) {
        if (c == '\n') {

            *lineno_addr = *lineno_addr + 1;
        }
    }


    if (c == '/') {
        if ((d = getch()) == '*') {
            comment = IN;
        } else {
            comment = OUT;
            ungetch(d);
        }
    }


    if (c == '\"') {
        string = IN;
    }


    if (c == '#') {
        directive = IN;
    }

    if (c == '\\') {
        c = getch(); 
    }

    if (comment == OUT && string == OUT && directive == OUT) {

        if (c != EOF)
            *w++ = c;

        if (!isalnum(c) && c != '_') {
            *w = '\0';
            return c;
        }

        for (; --lim > 0; w++) {
            *w = getch();
            if (!isalnum(*w) && *w != '_') {
                ungetch(*w);
                break;
            }
        }
        *w = '\0';
        return word[0];
    } else if (comment == IN) {
        *w++ = c;
        *w++ = d;

        while ((*w++ = c = getch())) {
            if (c == '*') {
                if ((c = getch()) == '/') {
                    *w++ = c;
                    comment = OUT;
                    break;
                } else {
                    ungetch(c);
                }
            }
        }
        *w = '\0';

    } else if (string == IN) {
        *w++ = c;
        while ((*w++ = getch()) != '\"') {
            if (*w == '\\')
                *w++ = getch();
        }
        string = OUT;
        *w = '\0';
    } else if (directive == IN) {
        *w++ = c;
        while ((*w++ = getch()) != ' ') {
            if (c == '\\') { 
                *w++ = getch();
            }
        }

        directive = OUT;
        *w = '\0';
    }

    return c;
}

#define BUFSIZE 100

char buf[BUFSIZE];
int bufp = 0;      

int getch(void) {
    return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c) {
    if (bufp >= BUFSIZE)
        printf("ungetch: too many characters\n");
    else
        buf[bufp++] = c;
    return;
}

