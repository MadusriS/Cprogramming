#include<stdio.h>

//char lines[100][100][100];
void printfile(FILE *file,const char *filename,int *pagecount);

int main(int argc,char* argv[]){
int pagecount=0;

for(int i=1;i<argc;i++){
File *file=fopen(argv[i],"r");
if(file!=NULL){
printfile(file,argv[i],&pagecount);
fclose(file);
pagecount++;
}}
return 0;}
void printfile(FILE*file,const char*filename,int *pagecount){
printf("\n==page%d:%s===\n",*pagecount+1,filename);
char line[100];
int linecount =0;

while(fgets(line,sizeof(line),file)!=NULL){
printf("%s",line);
linecount++;
if(linecount==20){
linecount=0;
(*pagecount)++;
printf("\n===page%d:%s===\n",*pagecount+1,filename);
}}
printf("\n");}






















