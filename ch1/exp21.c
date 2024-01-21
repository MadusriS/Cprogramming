/*#include<stdio.h>
void main(){
	int c;
	int f=1;
	while((c=getchar())!=EOF){
		if(c==' '){
			f++;
		       	if(f%8==0){putchar('\t');}}
		else{
			if(f%8!=0){
				for(int i=0;i<f;i++) putchar(' ');}
			putchar(c);
			f=0;}
		}}*/#include <stdio.h>
#define TAB_WIDTH 4 
void entab() {
    int ch, column, numSp;
    column = 0;
    numSp = 0;
    while ((ch = getchar()) != EOF) {
        if (ch == ' ') {
            numSp++;
            if ((column + numSp) % TAB_WIDTH == 0) {
                putchar('\t');  
                numSp = 0;
            }
        } else {
        
            while (numSp > 0) {
                putchar(' ');
                numSp--;
            }

            putchar(ch);

            if (ch == '\t') {

                column = (column / TAB_WIDTH + 1) * TAB_WIDTH;
            }
            else{

                column++;
            }
        }
    }
}

int main() {
    entab();
    return 0;
}


		
