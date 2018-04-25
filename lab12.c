#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

char const * sperm(__mode_t mode) {
    static char local_buff[16] = {0};
    int i = 0;
    // user permissions
    if ((mode & S_IRUSR) == S_IRUSR) local_buff[i] = 'r';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IWUSR) == S_IWUSR) local_buff[i] = 'w';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IXUSR) == S_IXUSR) local_buff[i] = 'x';
    else local_buff[i] = '-';
    i++;
    // group permissions
    if ((mode & S_IRGRP) == S_IRGRP) local_buff[i] = 'r';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IWGRP) == S_IWGRP) local_buff[i] = 'w';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IXGRP) == S_IXGRP) local_buff[i] = 'x';
    else local_buff[i] = '-';
    i++;
    // other permissions
    if ((mode & S_IROTH) == S_IROTH) local_buff[i] = 'r';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IWOTH) == S_IWOTH) local_buff[i] = 'w';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IXOTH) == S_IXOTH) local_buff[i] = 'x';
    else local_buff[i] = '-';
    return local_buff;
}




int main(int argc, char *argv[])
{

	struct stat statBuf;

	if(argc < 2) {
		printf("Need filename\n");
		exit(1);
	}

	// Just 2 - regular ls
	if(argc < 3){
		DIR *dirPtr;
		struct dirent *entryPtr;
		if(stat(argv[1], &statBuf) < 0){
			perror("Error: ");
			exit(1);
		}
		dirPtr = opendir(argv[1]);
		while((entryPtr = readdir(dirPtr))){
			printf("%-20s", entryPtr->d_name);
		}
	}
	else{ // ls with flag
		DIR *dirPtr;
		struct dirent *entryPtr;
		if(stat(argv[2], &statBuf) < 0){
			perror("Error: ");
			exit(1);
		}
		if(strncmp(argv[1], "-l", 2) == 0){
			int numBlocks = 0;
			char file[1024];
			dirPtr = opendir(argv[2]);
			while((entryPtr = readdir(dirPtr))){
				snprintf(file, 1024, "%s/%s", argv[2], entryPtr->d_name);
				if(stat(file, &statBuf) == -1){
					printf("%s\n", entryPtr->d_name);
					perror("Error ");
					continue;
				}
				int block = (statBuf.st_blocks)/2;	
				printf("%d ", block);

				printf("%10.10s", sperm(statBuf.st_mode));
				printf("%4d", statBuf.st_nlink);

				struct passwd *pwd;
				if ((pwd = getpwuid(statBuf.st_uid)) != NULL)
					printf(" %-8.8s", pwd->pw_name);
				else
					printf(" %-8d", statBuf.st_uid);
				struct group *grp;
				if((grp = getgrgid(statBuf.st_gid)) != NULL)
					printf(" %-8.8s", grp->gr_name);
				else
					printf(" %-8d", statBuf.st_gid);
				printf(" %9jd", statBuf.st_size);


				struct tm *tm;
				char datestring[256];
				tm = localtime(&statBuf.st_mtime);
				strftime(datestring, sizeof(datestring), nl_langinfo(D_T_FMT), tm);

				printf(" %s %s\n", datestring, entryPtr->d_name);

				numBlocks += block;
			}
			printf("Total blocks: %d\n", numBlocks);
		}
		else if(strncmp(argv[1], "-i", 2) == 0){	
			dirPtr = opendir(argv[2]);
			while((entryPtr = readdir(dirPtr))){
				printf("%u %s\n", entryPtr->d_ino, entryPtr->d_name);
			}
		}
		else{
			printf("Error: Flag not known\n");
			return 1;
		}


	}

	return 0;

}
