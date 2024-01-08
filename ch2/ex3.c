#include<stdio.h>
#include<string.h>
#include<math.h>
int hextodc(char *hex){
   int y = 0;
   int dec = 0;
   int x, i;
   for(i = strlen(hex) - 1 ; i >= 0 ; --i)
   {
      if(hex[i]>='0'&&hex[i]<='9'){
         x = hex[i] - '0';
      }
      else{
         x = hex[i] - 'A' + 10;
      }
      dec = dec + x * pow(16 , y);
      ++y;
   }
   return dec;
}
int main(){
   char hex[100];
   printf("Enter Hexadecimal: ");
   scanf("%s", hex);
   printf("Decimal: %d", hextodc(hex));
   return 0;
}
