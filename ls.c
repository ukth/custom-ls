#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

# define MAX_FILE_N 100

char* month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

int monthOrYear(int year, struct tm* tm_ptr, char* str){
	// printf("!!%d\n",tm_ptr->tm_year);


	if (tm_ptr->tm_year+1900 == year) {
		str[0] = (tm_ptr->tm_hour+9) / 10 + 48;
		str[1] = (tm_ptr->tm_hour+9) % 10 + 48;
		str[2] = ':';
		str[3] = tm_ptr->tm_min / 10 + 48;
		str[4] = tm_ptr->tm_min % 10 + 48;
		str[5] = '\0';
	} else {
		str[0] = (tm_ptr->tm_year+1900) / 1000 + 48;
		str[1] = ((tm_ptr->tm_year+1900) / 100)%10 + 48;
		str[2] = ((tm_ptr->tm_year+1900) / 10)%10 + 48;
		str[3] = (tm_ptr->tm_year+1900) % 10 + 48;
		str[4] = '\0';
	}

}

int main(int argc, char *argv[])
{
	DIR				*dp;
	struct dirent	*dirp;
	struct dirent	*dirL[MAX_FILE_N];
	struct stat st;
	struct passwd *file_own;
	struct group *file_grp;
	struct tm *tm_ptr;
	
	// time_t t = time(NULL);
	// struct tm now = *localtime(&t);
	
	char mod[11];
	int i, j;
	int n;
	mode_t modes;
	int l_option = 0;
	char mOrY[6];
	int blocks = 0;
	char root[30]={0,};
	char path[100]={0,};

	if (argc < 2){
		// printf("usage: ls directory_name");
		dp = opendir(".");
	}
	if (argc == 2) {
		if (!strcmp(argv[1], "-l")){
			l_option = 1;
			dp = opendir(".");
		} else {
			dp = opendir(argv[1]);
			memcpy(root, argv[1], strlen(argv[1]));
			strcat(root, "/");
		}
	} else if (argc == 3) {
		if (!strcmp(argv[1], "-l")){
			dp = opendir(argv[2]);
			memcpy(root, argv[2], strlen(argv[2]));
			strcat(root, "/");
			l_option = 1;
		} else {
			printf("Invalid %s option\n", argv[1]);
			exit(0);
		}
		
	}

	if (dp == '\0'){
		printf("can't open %s", argv[1]);
	}

	i = 0;
	while ((dirp = readdir(dp)) != '\0'){
		
		// printf("%s\n",dirp->d_name);
		if (l_option && dirp->d_name[0] != '.'){
			dirL[i] = dirp;

			memcpy(path, root, strlen(root)+1);
			strcat(path, dirp->d_name);
			lstat(path,&st);
			
			blocks += st.st_blocks;
			// printf("%s %d\n",dirp->d_name, st.st_blocks);
			i++;
		}
		
	}
	n = i;


	for (i = 0; i < n - 1; i++) {
		for (j = i + 1; j < n; j++ ){
			// printf("%s\t%s\t%d\n",dirL[i]->d_name,dirL[j]->d_name,strcmp(dirL[i]->d_name, dirL[j]->d_name));
			if (strcasecmp(dirL[i]->d_name, dirL[j]->d_name) > 0) {
				// printf("A!!\n");
				dirp = dirL[i];
				dirL[i] = dirL[j];
				dirL[j] = dirp;
			}
		}
	}



	if (!l_option){
		for (i = 0; i < n; i++){
			printf("%s  ", dirL[i]->d_name);
		}
		printf("\n");
	} else {
		printf("total %d\n", blocks/2);
		for (i = 0; i < n; i++){
			for(j = 0; j < 10; j++){
				mod[j]='-';
				
			}
			mod[10] = '\0';

			// path[0]=0;
			// printf("PATH:%s\n", root);
			memcpy(path, root, strlen(root)+1);
			strcat(path, dirL[i]->d_name);
			lstat(path,&st);
			// printf("PATH:%s\n", path);
			modes=st.st_mode;

			if(S_ISDIR(modes)) {
				mod[0] = 'd';
			}
			if(modes & S_IRUSR) { 
				mod[1] = 'r';
			}
			if(modes & S_IWUSR) {
				mod[2] = 'w';
			}
			if(modes & S_IXUSR) {
				mod[3] = 'x';
			}
			if(modes & 0040) {
				mod[4] = 'r';
			}
			if(modes & 0020) {
				mod[5] = 'w';
			}
			if(modes & 0010) {
				mod[6] = 'x';
			}
			if(modes & 0004) {
				mod[7] = 'r';
			}
			if(modes & 0002) {
				mod[8] = 'w';
			}
			if(modes & 0001) {
				mod[9] = 'x';
			}

			file_own = getpwuid(st.st_uid);
			file_grp = getgrgid(st.st_gid);
			tm_ptr = gmtime(&st.st_mtime);

			monthOrYear(2021, tm_ptr, mOrY);

			// printf("%s %d %s %s %5d %s %02d %5s%-05s\n",
			// printf("%d\n",tm_ptr->tm_mon);
			   printf("%s %3d %s %s %5d %s %02d %5s %-05s\n",
				mod, st.st_nlink, file_own->pw_name, file_grp->gr_name, st.st_size,
				month[tm_ptr->tm_mon], tm_ptr->tm_mday, mOrY,
				dirL[i]->d_name);
		}
	}

	closedir(dp);
}
