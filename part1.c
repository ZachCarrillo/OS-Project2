/*
 * Implements a simple version of the find utility
 * Input: ./part1 -w <directory> -n <name> -i <inode> -m <mmin> -a <action>
 * -n -i and -m and mutually exlusive (code likely wont work if you try multiple at a time)
 * Output: print all the files
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
char * full_path;


void 
find(char *where, char *name, char *mmin, char *inum, char *action)
{
	/* Test arguments parsed. */
    read_sub(where, name, mmin, inum, action);
    exit(0);
}

int 
main(int argc, char **argv)
{
	int		w, n, m, i, a;
	char  *where, *name, *mmin, *inum, *action;
	while (1) {
		char		c;

		c = getopt(argc, argv, "w:n:m:i:a:");	/* A colon (‘:’) to
							 * indicate that it
							 * takes a required
							 * argument, e.g, -w
							 * testdir */
		if (c == -1) {
			/* We have finished processing all the arguments. */
			break;
		}
		switch (c) {
		case 'w':
			w = 1;
			where = optarg;
			printf("where: %s\n", optarg);
			break;
		case 'n':
			n = 1;
			name = optarg;
			printf("name: %s\n", optarg);
			break;
		case 'm':
			m = 1;
			mmin = optarg;
			printf("mmin: %s\n", optarg);
			break;
		case 'i':
			i = 1;
			inum = optarg;
			printf("inum: %s\n", optarg);
			break;
		case 'a':
			a = 1;
			action = optarg;
			printf("action: %s\n", optarg);
			break;
		case '?':
		default:
			printf("An invalid option detected.\n");
		}
	}

	/*
	 * Now you can pass the parameters to the find function. For example,
	 * if the action, where, name are all specified, then you can call
	 * find(where, name, action), but this depends on how you implement
	 * the find function.
	 */

	 // if there is no specified directory set it to "." the current working directory
	if (w!=1) 
	{
		where = ".";
		w = 1;
	}
	// if there is no specified name set it to NULL
	if (n!=1)
	{
		name = NULL;
		n = 1;
	}
	// if there is no specified mmin set it to NULL
	if (m!=1)
	{
		mmin = NULL;
		m = 1;
	}
	// if there is no specified inum set it to "NULL" setting it to NULL makes segmentation fault with atoi()
	if (i!=1)
	{
		inum = "NULL";
		i = 1;
	}
	// if there is no specified action set it to NULL
	if (a!=1)
	{
		action = ""; // setting action to NULL would break strcmp() so have to set as empty string.
		w = 1;
	}
	//run the find command
	find(where, name, mmin, inum, action);
	

	/* The following codes are optional, but better to study it. */

	/*
	 * Now set the values of "argc" and "argv" to the values after the
	 * options have been processed above.
	 */
	argc -= optind;
	argv += optind;

	/*
	 * Now do something with the remaining command-line arguments, if
	 * necessary.
	 */
	if (argc > 0) {
		printf("There are %d command-line arguments left to process:\n", argc);
		for (i = 0; i < argc; i++) {
			printf("Argument %d: '%s'\n", i + 1, argv[i]);
		}
	}
	return 0;
}

void read_sub(char* sub_dir, char *name, char *mmin, char *inum, char *action)
{
  DIR *sub_dp=opendir(sub_dir);
  struct dirent * sub_dirp;

  if(sub_dp!=NULL)
    {
       while((sub_dirp=readdir(sub_dp))!=NULL)
        {
         //printf("%s\n",sub_dirp->d_name);
         //if(stat(sub_dirp->d_name,&buf)==0)
         //printf("%d ", (int)buf.st_size);
		 	struct stat buf; // define a file status struct
			int bufinum; //the files inum
			int bufmmin; //the files mmin
	 		char * temp =sub_dirp->d_name;
        	char temp1[]=".";
	 		char temp2[]="..";

			if(strcmp(temp,temp1)!=0&&strcmp(temp,temp2)!=0)//recurcively loop into the sub-directory
			{
				char temp3[]="/";
				char *temp_sub=temp3;
				temp_sub=strcat(temp_sub,temp);
				char * temp_full_path=malloc(sizeof(char)*2000);
				temp_full_path=strcpy(temp_full_path,sub_dir);
				strcat(temp_full_path,temp_sub);
				DIR * subsubdp=opendir(temp_full_path);
				if(subsubdp!=NULL){ // if this is NULL we have found a file, if not then a directory was found
					closedir(subsubdp);
					read_sub(temp_full_path, name, mmin, inum, action);
				}
				if(stat(temp_full_path,&buf)==0)
				{
					//since time() and st_mtime return time in seconds you need to find the diffrence then divide by 60 to get minutes from last modification
					time_t curTime = time(NULL);
					//printf("name is: %s current time is %d, mtime is %d \n",temp , (int)curTime,(int)buf.st_mtime);
					bufmmin = ((int)curTime-(int)buf.st_mtime)/60;
					bufinum = (int)buf.st_ino;
					//printf("mmin = %d, inum = %d \n", bufmmin,bufinum);

				}

				if(name != NULL && strcmp(name, temp) == 0) //check name
				{
					printf("Found Name: %s\n",temp_full_path);
					if(strcmp(action, "delete") == 0) 
					{
						remove(temp_full_path);
						printf("Deleted File");
					}
				}
				else if(mmin != NULL && ((mmin[0] == '+' && bufmmin>atoi(mmin)) || mmin[0] == '-' && bufmmin < abs(atoi(mmin)) || bufmmin == atoi(mmin))) //check mmin
				{
					printf("Correct mmin :%s\n",temp_full_path);
					if(strcmp(action, "delete") == 0) 
					{
						remove(temp_full_path);
						printf("Deleted File");
					}
				}
				else if(bufinum == &inum) //check inum (UNIX only)
				{
					printf("Same inum: %s\n",temp_full_path);
					if(strcmp(action, "delete") == 0) 
					{
						remove(temp_full_path);
						printf("Deleted File");
					}
				}
				else if(name == NULL && mmin == NULL && inum == NULL)
				{
					printf("%s\n",temp_full_path);
					if(strcmp(action, "delete") == 0) 
					{
						remove(temp_full_path);
						printf("Deleted File");
					}
				}
			}
        }
       closedir(sub_dp);
    }
    else
    {
        printf("cannot open directory\n");
        exit(2);
    }

}