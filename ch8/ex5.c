/*This C program is a simplified implementation of the Unix du (disk usage) command. It recursively traverses directories and prints the size of each file, the filename, and the owner of the file. */
/*The current directory (.) in /home/user/documents/ is /home/user/documents/.
The parent directory (..) in /home/user/documents/ is /home/user*/
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <pwd.h>
/*struct stat declaration,
 if (stat(name, &stbuf) == -1)  where is stat function?
 In the provided C program, the struct stat declaration is part of the #include directives at the beginning of the program. Specifically, it comes from the <sys/types.h> and <sys/stat.h> header files. These headers define the structure and functions related to file status, including the struct stat used to hold file information.
*/



#define MAX_PATH 1024

#ifndef DIRSIZ
#define DIRSIZ 14
#endif
/*dirwalk Function:

Parameters: dir is the current directory path, (*fcn)(char *) is a function pointer representing the function to be called on each file.
Opens the specified directory and reads its entries.
Calls the provided function (fcn) for each file in the directory.*/

void dirwalk( char *dir,void (*fcn)(char *)){

	char name[MAX_PATH];
	struct dirent *dp;/*Declares a pointer to a dirent structure. The dirent structure represents a directory entry.*/
	DIR *dfd;/*Declares a pointer to a DIR structure. The DIR structure represents a directory stream.It is typically associated with directory-related functions, such as opening, reading, and closing directories.the DIR structure is often defined in the <dirent.h> header.*/

	if((dfd = opendir(dir))==NULL){
		puts("Error: Cannot open Directory");
		return;
	}
	puts(dir);
	// Get each dir entry
	while((dp=readdir(dfd)) != NULL){
		// Skip . and .. is redundant.
		if(strcmp(dp->d_name,".") == 0
			|| strcmp(dp->d_name,"..") ==0 )/*Skips the entries for the current directory (.) and the parent directory (..), as they are not needed for processing.
*/
			continue;
		if(strlen(dir)+strlen(dp->d_name)+2 > sizeof(name))/*Checks if the combined length of the directory path, the entry name, and additional characters (such as the directory separator) would exceed the size of the name array. If it does, it prints an error message.*/
			puts("Error: Name too long!");
		else{
			sprintf(name,"%s/%s",dir,dp->d_name);
			// Call fsize
			(*fcn)(name);
		}
	}
	closedir(dfd);
}
/*fsize Function:

Parameters: name is the path to the file or directory.
Obtains file statistics using stat and stores the result in stbuf.
If the entry is a directory, it calls dirwalk recursively on that directory.
Prints the size, name, and owner of the file.*/

void fsize(char *name){
	struct stat stbuf;/*The function starts by obtaining file statistics using the stat function. The stat function 					fills the stbuf structure with information about the file, such as size, mode, owner, and more*/

	if(stat(name,&stbuf) == -1){
		puts("Error: Cannot get file stats!");
		return;
	}

	if((stbuf.st_mode & S_IFMT) == S_IFDIR){/* stbuf.st_mode: This field of the stat structure (stbuf) contains information about the file mode and permissions.
	S_IFMT: This is a macro that represents the file type mask. It is used to mask out the file type bits from the file mode.
	It checks if the file entry is a directory by examining the file mode bits using the S_IFMT mask. If it is a directory, it calls the dirwalk function recursively.*/
		dirwalk(name,fsize);
	}
	struct passwd *pwd = getpwuid(stbuf.st_uid);/*It uses the getpwuid function to retrieve the owner's information based on the user ID (st_uid) obtained from the file's statistics. The result is a struct passwd structure containing details about the owner.*/
	//print file name,size and owner
	printf("the size is %81d ,the file name is  %s ,and the Owner: %s\n",(int)stbuf.st_size,name,pwd->pw_name);
}

/*main Function:

If no command-line arguments are provided, it calls fsize(".") (current directory).
If command-line arguments are provided, it calls fsize for each specified file or directory.*/


int main(int argc,char *argv[]){

	if(argc==1)
		fsize(".");
	else 
		while(--argc>0)
			fsize(*++argv);
	return 0;
}
