#include<stdio.h>

void expand(char s1[],char s2[]){
  int j=0,i=0;
  while(s1[j] != '\0'){
    if (s1[j]>= 'a' &&  s1[j] <= 'z' && s1[j+1] == '-' && s1[j+1]!='\0' &&  s1[j+2] >= 'a' &&  s1[j+2] <= 'z' && s1[j+2] !='\0'){
      int z = s1[j+2]-s1[j];
      int c;
      for (c=0;c<=z;c++){
    s2[i]= c+s1[j];
    i++;
      }
      j=j+3;
            
    }
    else if (s1[j]>= 'A' &&  s1[j] <= 'Z' && s1[j+1] == '-' && s1[j+1]!='\0' &&  s1[j+2] >= 'A' &&  s1[j+2] <= 'Z' && s1[j+2] !='\0'){
      int z = s1[j+2]-s1[j];
      int c;
      for (c=0;c<=z;c++){
    s2[i]= c+s1[j];
    i++;
      }
      j=j+3;
            
    }
    else if (s1[j]>= '0' &&  s1[j] <= '9' && s1[j+1] == '-' && s1[j+1]!='\0' &&  s1[j+2] >= '0' &&  s1[j+2] <= '9' && s1[j+2] !='\0'){
      int z = s1[j+2]-s1[j];
      int c;
      for (c=0;c<=z;c++){
    s2[i]= c+s1[j];
    i++;
      }
      j=j+3;
            
    }
    else  if (j!= 0  && s1[j] == '-'  && (s1[j-1] < s1[j+1])){
      int z = s1[j+1]-(1+s1[j-1]);
      int c;
      for (c=0;c<=z;c++){
    s2[i]= c+(s1[j-1]+1);
    i++;
      }
      j=j+2;
      
    }      
    else if ( s1[j]>= 32 &&  s1[j] <= 127 && (s1[j+1] != '-' || s1[j+1]>= 32 &&  s1[j+1] <= 127 )){
      s2[i] = s1[j];
      
      j++;
      i++;      
     }
  }
  s2[i]='\n';
  i++;
  s2[i]='\0';
}


int main() {

  int c;
  char s2[100];
  expand("-a-c,a-c-g,A-Z0-9--", s2);
  
  printf("%s",s2);
 
 
}


