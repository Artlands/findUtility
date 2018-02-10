#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define BOOL int
#define TRUE 1
#define FALSE 0

BOOL time_cal(char* file_path, char* mmin){	// calculate time
	struct stat buf;	// define a file status structure, including i-node information
	stat(file_path, &buf);

	time_t modified_time = buf.st_mtime;	// get the modified time
	time_t current_time = time(NULL);	// get the current time
	long elapsed_time = difftime(current_time, modified_time)/60;	// calculate the elapsed time in minutes
	long value = atoi(mmin);	// convert a string to number

	if((mmin[0] == '-') && (elapsed_time <= -value))	// decide if time matches 
		return TRUE;
	if((mmin[0] == '+') && (elapsed_time >= value))
		return TRUE;
	return FALSE;
}

void print_path(char* file_path, int delete){	// print file path or delete infomation 
	if(delete == 0)		//do not delete anything
		printf("%s \n", file_path);
	else{
		remove(file_path);
		printf("%s has been removed.\n", file_path);
	}
}

void find(char* where, char* criteria, int name, int mmin, int inode, int delete){	//find directory function
	DIR *sub_dp  = opendir(where);	// open a directory stream
	struct dirent * sub_dirp;
	char temp1[] = ".";
	char temp2[] = "..";
	char temp3[] = "/";
	BOOL timematch = FALSE;

	if(sub_dp != NULL){		//check whether the directory stream is opened successfully
		while((sub_dirp = readdir(sub_dp)) != NULL){	//find every entry one by one

			char * temp = sub_dirp -> d_name;	// get the name

			if(strcmp(temp, temp1) !=0 && strcmp(temp, temp2) != 0){	// ingnore . and .. in directory

				char *temp_sub = malloc(sizeof(char) * 2000);		
				temp_sub = strcpy(temp_sub, temp3);	//add the '/' in front of the entry's name
				strcat(temp_sub, temp);												

				char *temp_full_path = malloc(sizeof(char) * 2000);		// create a variable to hold the full path	
				temp_full_path = strcpy(temp_full_path, where);
				strcat(temp_full_path, temp_sub);					
				
				DIR *subsubdp = opendir(temp_full_path);	//open the file path just created 

				if(subsubdp != NULL){	//if not null means we find a subdirectory, otherwise, its a file
					closedir(subsubdp);		// close the stream
					find(temp_full_path, criteria, name, mmin, inode, delete);	//call the recursive function call.
				} 
				else
				{
					if ((name == 1) && (strcmp(sub_dirp->d_name, criteria) == 0))
						print_path(temp_full_path, delete);
					if ((mmin == 1) && time_cal(temp_full_path, criteria)) 
						print_path(temp_full_path, delete);
					if ((inode == 1) && (sub_dirp->d_ino == atoi(criteria)))
						print_path(temp_full_path, delete);
					if (name == 0 && mmin == 0 && inode == 0)
						print_path(temp_full_path, delete);	
				}
				
			}
		}
		closedir(sub_dp);	//close the stream
	}									
	else{
		printf("cannot open directory\n");
		exit(2);
	}

}


int main(int argc, char ** argv){
	int w = 0, n = 0, m = 0, i = 0, d = 0, delete = 0;
	char *where_arg, *criteria_arg;
	while(1){
		char c;
		c = getopt(argc, argv, "w:n:m:i:d");	//a colon to indicate that it takes a required argument
		if(c == -1){	//we have fininshed processing all the arguments
			break;
		}
		switch(c){
		case 'w':
			w = 1;
			where_arg = optarg;
			break;
		case 'n':
			n = 1;
			criteria_arg = optarg;
			break;
		case 'm':
			m = 1;
			criteria_arg = optarg;
			break;
		case 'i':
			i = 1;
			criteria_arg = optarg;
			break;
		case 'd':
			d = 1;
			delete = 1;
			break;
		case'?':
		default:
			printf("not argument\n");
		}
	}
	if(w == 1){
		find(where_arg, criteria_arg, n, m, i, delete);
	}

	argc -= optind;		//Set the values of argc and argv to the values 
	argv += optind;		//after the optins have been processed above

	if(argc > 0){
		printf("There are %d command-line arguments left to process:\n", argc);
		for (int i = 0; i < argc; i++){
			printf("	Argument %d: '%s'\n", i+1, argv[i] );
		}
	return 0;
	}

}