#include<stdio.h>
#include<ctype.h>
#include<math.h>
#define SIZE 2
#define BUFSIZE 100

char buf[BUFSIZE];
int bufp = 0;


/*int getch(void) {
    return (bufp > 0) ? buf[--bufp] : getchar();
}


void ungetch(int c) {
    if (bufp >= BUFSIZE)
        printf("ungetch: too many characters\n");
    else
        buf[bufp++] = c;
}
*/

int getint(float *pn) {
    int d=0;
    int c, sign;
    while (isspace(c = getchar()));
    if (!isdigit(c) && c!='.'&& c != EOF && c != '+' && c != '-') {
        //ungetch(c);
        return -1; //it will end the program if it is not a sign or number 
    }

    sign = (c == '-') ? -1 : 1;

   if (c == '+' || c == '-')
        //c = getch();
        c=getchar();
    if (!isdigit(c) && c!='.')
        return 0;//therefore if a non valid character encounters that will end the program
        
    for (*pn = 0.0; isdigit(c)||c=='.' ; c = getchar()){
        printf("\nthe c is%c\n",c);
        if(c=='.'){d=0;}
        
       if(isdigit(c)){*pn = 10.0* *pn + (c - '0');d++;}
        printf("\n%f pn",*pn);
    }

*pn=(*pn)*(pow(10,-d));
printf("\n%f pnpower",*pn);
    *pn *= sign;
    printf("\n%f pnpowersign",*pn);


    return c;
}

int main(void) {
    int n, s;
     float array[SIZE];
     int getint(float *);

    for (n = 0; n < SIZE && getint(&array[n]) != EOF; n++) {
        //the array here stors all the input we give 6g: if we give +55 it will be stored in 0th index of array;
        /* if i give this input:++9
        my output will be:
in storing in n = 0, getint 0//+9 is valid not the extra + given hence 0;
in storing in n = 1, getint 9//
storing in n = 2, getint 1213678336
0
9
1213678336
*/
/* ( (res!=0) ? n++:0 ) will add only valid values to the array */
        printf("in storing in n = %d, getint %f\n", n, array[n]);
    }

    printf("finally storing in n = %d, getint %f\n", n, array[n-1]);

    for (s = 0; s <n; s++)
        printf("%f\n", array[s]);

    return 0;
}


