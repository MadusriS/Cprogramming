#include <stdio.h>

int main() {
    int c;
    int nl, nw, nc;
    int inWord;
    nl = nw = nc = inWord = 0;  
    printf("Test Case 1 (No input):\n");  
    while ((c = getchar()) != EOF) {
        nc++;
        if (c == '\n')
            nl++;    }
    printf("%d %d %d\n\n", nl, nw, nc);   
    printf("Test Case 2 (One-character word):\n");   
    while ((c = getchar()) != EOF) {
        nc++;
        if (c == '\n') {
            nl++;
            nw++;
        } else if (c == 'I') {
            inWord = 1;
        }
    }
    printf("%d %d %d\n\n", nl, nw, nc);
    printf("Test Case 3 (Two-character word):\n");
      while ((c = getchar()) != EOF) {
        nc++;
        if (c == '\n') {
            nl++;
            nw++;
        } else if (inWord) {
            inWord = 0;
        }
    }
    printf("%d %d %d\n\n", nl, nw, nc);
    printf("Test Case 4 (Two one-character words):\n");
       while ((c = getchar()) != EOF) {
        nc++;
        if (c == '\n') {
            nl++;
            nw++;
        } else if (inWord) {
            inWord = 0;
        }
    }
    printf("%d %d %d\n\n", nl, nw, nc);
    printf("Test Case 5 (Two one-character words, one word per line):\n");
      while ((c = getchar()) != EOF) {
        nc++;
        if (c == '\n') {
            nl++;
            nw++;
        } else if (inWord) {
            inWord = 0;
        }    }
    printf("%d %d %d\n\n", nl, nw, nc);
    return 0;
}
/*
#include<stdio.h>
void main(){
	int c ,flag;
	flag=0;
	while((c=getchar())!=EOF){
		if (c==' '|| c=='\n' || c=='\t'){
			if (flag==1){
				putchar('\n');
				flag=0;}}
			else if (flag==0){
				flag=1;
				putchar(c);
			}
			else {
				putchar(c);}}} */
