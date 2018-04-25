/* Author: Jacob Pankey */
/* Course: CS452	*/
/* Professor Dulimarta	*/
/* Program 3 		*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#define BLOCKSIZE 4096.0
void getStats(char * file, int depth );
int getBlocks(char *file);
int getBytes (char *file);
int getNumFiles(char *file);
int cmpfunc(const void *,const void*);
char** sort(char **text);
/* Print file statistics */
FILE * fp;
/* Save output to allow sorting */
char out[1000000][PATH_MAX];
/* Total lines of output */
int line = 0;

int main(int argc, char *argv[])
{
	/* Booleans to keep track of flags */
	int readFlag = 0;
	int byteFlag = 0;
	int numFlag = 0;
	int sortFlag = 0;
	char *dir;
	int i;


	// Check for flags
	for( i = 0; i < argc; i++)
	{
		if(strcmp(argv[i], "-b") == 0)
			byteFlag = 1;
		if(strcmp(argv[i], "-h") == 0)
			readFlag = 1;
		if(strcmp(argv[i], "-n") == 0)
			numFlag = 1;
		if(strcmp(argv[i], "-s") == 0)
			sortFlag = 1;
	}
	// Is the first character of the last command line
	// argument a tac. If yes start program from current
	// directory. Else start program from supplied directory
	if(! (argv[argc - 1][0] == '-') && argc != 1)
		dir = argv[argc - 1];
	else
		dir = ".";
	
	printf("%s\n", dir);
	if ( readFlag )
		getBlocks(dir);
	else if ( byteFlag ) 
		getBytes(dir);
	else
		getBytes(dir);
	// -n Option for file count
	if ( numFlag )
	{
		printf("File Count:\n");
		printf("Total = %d\n", getNumFiles(dir));
	}
	// - s Option for sorted output
	if(sortFlag)
		qsort(out, line, PATH_MAX, cmpfunc);
	// Print
	for(i = 0; i < line; i++)
		printf("%s\n", out[i]);
	
	
	fp = fopen("data.txt", "w+");
//	getStats(dir, 0);
	fclose(fp);
	return 0;
}

/************************************************
 * Function performs similar to du function without
 * any arguments supplied. Reports the number of
 * blocks of disk usage from this file and below.
 * @ Param file - Specify the file from which to
 * begin traversing
 * @ Return blocks - Returns blocks of disk space
 *************************************************/
int getBlocks(char* file)
{
	DIR *dirPtr;
	char dirStr[PATH_MAX];
	struct dirent *entryPtr;
	struct stat statbuf;
	double size = 0;
	int blocks = 0;
	
	/* Open Directory */
	if ((dirPtr = opendir(file)) == NULL)
	{	
		printf("Something went wrong");
		exit(EXIT_FAILURE);	
	}
	
	/* Read each entry in the current directory */
	while ((entryPtr = readdir(dirPtr)))
	{
		// No need to do anything for parent directory
		if(strcmp(entryPtr->d_name, "..") == 0)
			continue;

		// One block allocated for current directory
		if(strcmp(entryPtr->d_name, ".") == 0)
		{
			blocks++;
			continue;
		}
		sprintf(dirStr, "%s/%s",file,entryPtr->d_name);
		//stcpy(dirStr, file);
                //strcat(dirStr, "/");
                //strcat(dirStr, entryPtr->d_name);

		// Call stat and fill struct
		if ( stat(dirStr, &statbuf) == -1)
		{
			printf("%s: %s\n", entryPtr->d_name, strerror(errno));
			continue;
		}

		if ( S_ISDIR (statbuf.st_mode))
		{		
			blocks += getBlocks(dirStr);
		}
		else if(S_ISREG (statbuf.st_mode))
		{
			size = statbuf.st_size;
			// Every file appears to automatically recieve
			// one block allocated
			if(ceil(size/BLOCKSIZE) == 0)
				blocks++;
			else
				blocks += ceil(size/BLOCKSIZE);
	
		}else{
			perror("Unkown file type");
		}
	}
	sprintf(out[line], "%d", (blocks * 4));	
	strcat(out[line], " K");
	strcat(out[line], "\t");
	strcat(out[line], file);
	line++;
	closedir(dirPtr);
	return blocks;
}
/***********************************************
 * Comparison function supplied to qsort to allow
 * -s option that sorts the ouput. Accepts two
 * string arguments. Tokenizes file size from
 * string and comparares files sizes
 **********************************************/
int cmpfunc(const void *a,const void *b)
{
	char *tok1 = (char*)malloc(PATH_MAX * sizeof(char));
	char *tok2 = (char*)malloc(PATH_MAX * sizeof(char));
	
	strcpy(tok1, a);
	tok1 = strtok(tok1, " ");
	strcpy(tok2, b);
	tok2 =  strtok(tok2, " ");
	int num1 = atoi(tok1);
	int num2 = atoi(tok2);
	free(tok1);
	free(tok2);
	return num2 - num1;
}

/***********************************************
 * Simulate UNIX du utility. Calculates the bytes
 * of disk usage beginning at a specified directory
 * and all subdirectories of original directory
 * @PARAM char *file: Specify directory to run du 
 * function from
 * @RETURN int size: Allows recursive function to
 * pass subdirectory size to parent directory
 ***********************************************/
int getBytes(char *file)
{
	DIR *dirPtr;
	char dirStr[1024];
	struct dirent *entryPtr;
	struct stat statbuf;
	int size = 0;
	
	/* Open Directory */
	if ((dirPtr = opendir(file)) == NULL)
	{	
		printf("Something went wrong");
		exit(EXIT_FAILURE);	
	}
	
	/* Read each entry in the current directory */
	while ((entryPtr = readdir(dirPtr)))
	{
		// No need to do anything for parent directory
		if(strcmp(entryPtr->d_name, "..") == 0)
			continue;

		// One block allocated for current directory
		if(strcmp(entryPtr->d_name, ".") == 0)
		{
			size += 4096;
			continue;
		}
		strcpy(dirStr, file);
                strcat(dirStr, "/");
                strcat(dirStr, entryPtr->d_name);

		// Call stat and fill struct
		if ( stat(dirStr, &statbuf) == -1)
		{
			printf("%s: %s\n", entryPtr->d_name, strerror(errno));
			continue;
		}
		if ( S_ISDIR (statbuf.st_mode))
			size += getBytes(dirStr);
		else if(S_ISREG (statbuf.st_mode))
			size += statbuf.st_size;
		else
			perror("Unkown file type");
	}	
	
	closedir(dirPtr);
	sprintf(out[line], "%d", size);
	strcat(out[line], "\t");
	strcat(out[line], file);
	line++;	
	return size;
}
/*****************************************
 * Function determines the number of files
 * in a directory when the -n flag is 
 * specified. Recurses through each directory
 * and counts total files as sum of files
 * within a directory and all of its
 * subdirectories
 ****************************************/
int getNumFiles(char *file)
{
	DIR *dirPtr;
	char dirStr[1024];
	struct dirent *entryPtr;
	struct stat statbuf;
	int numFiles = 0;	
	/* Open Directory */
	if ((dirPtr = opendir(file)) == NULL)
	{	
		printf("Something went wrong");
		exit(EXIT_FAILURE);	
	}
	
	/* Read each entry in the current directory */
	while ((entryPtr = readdir(dirPtr)))
	{
		// No need to do anything for parent directory
		if(strcmp(entryPtr->d_name, "..") == 0)
			continue;

		// One block allocated for current directory
		if(strcmp(entryPtr->d_name, ".") == 0)
			continue;

		strcpy(dirStr, file);
                strcat(dirStr, "/");
                strcat(dirStr, entryPtr->d_name);

		// Call stat and fill struct
		if ( stat(dirStr, &statbuf) == -1)
		{
			printf("%s: %s\n", entryPtr->d_name, strerror(errno));
			continue;
		}
		if ( S_ISDIR (statbuf.st_mode))
			numFiles += getNumFiles(dirStr);
		else if(S_ISREG (statbuf.st_mode))
			numFiles++;
		else
			perror("Unkown file type");
	}	
	closedir(dirPtr);
	printf("%d\t%s\n",numFiles, file);
	
	return numFiles;

}
/**********************************************
 * This function is called to collect additional
 * statistics in relation to the depth of a a
 * file. These statistics include the directories
 * size, number of files, name, and depth. These
 * values do not include the value of subdirectories
 *********************************************/
void getStats( char* file, int depth )
{
	DIR *dirPtr;
	char dirStr[1024];
	struct dirent *entryPtr;
	struct stat statbuf;
	int size = 0;
	int numFiles = 0;

	/* Open Directory */
	if ((dirPtr = opendir(file)) == NULL)
	{	
		printf("Something went wrong");
		exit(EXIT_FAILURE);	
	}
	
	/* Read each entry in the current directory */
	while ((entryPtr = readdir(dirPtr)))
	{
		// No need to do anything for parent directory
		if(strcmp(entryPtr->d_name, "..") == 0)
			continue;

		// One block allocated for current directory
		if(strcmp(entryPtr->d_name, ".") == 0)
		{
			size += 4096;
			continue;
		}
		strcpy(dirStr, file);
                strcat(dirStr, "/");
                strcat(dirStr, entryPtr->d_name);

		// Call stat and fill struct
		if ( stat(dirStr, &statbuf) == -1)
		{
			printf("%s: %s\n", entryPtr->d_name, strerror(errno));
			continue;
		}
		if ( S_ISDIR (statbuf.st_mode))
			getStats(dirStr, depth + 1);
		else if(S_ISREG (statbuf.st_mode))
		{
			size += statbuf.st_size;
			numFiles++;
		}
		else
			perror("Unkown file type");
	}	
	closedir(dirPtr);
	/* Print Directory Name, Depth, numFiles, Size */
	fprintf(fp, "%s, %d, %d, %d\n", file, depth, numFiles, size);
	
}
