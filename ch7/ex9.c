#include <stdio.h>
#include<ctype.h>

int main()
{
    int c;
    printf("give input\n");
    //scanf("%c\n",&c);
    while((c=getchar())!=EOF){
        if(c>='a' && c<='z' &&c!='\n'){
            printf("its lower\n");
            
        }
        else if(c!='\n'){
            printf("its higher\n");
        }
        
    }
     while((c=getchar())!=EOF){
         if(islower(c)){
             printf("iys lowr\n");
             
         }
         else{
             printf("its higgher\n");
         }
     }
    

    return 0;
}
