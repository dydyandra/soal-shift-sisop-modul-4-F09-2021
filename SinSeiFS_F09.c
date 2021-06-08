#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include<stdbool.h>  


static  const  char *dirpath = "/home/dyandra/Downloads";
char *en1 = "/AtoZ_";
int flagGlobal;


char* getDirFile(char* path){
    char fullPath[2048];
    char rpath[2048];
    char tipe[2048];
    char namaFile[2048];
    char namaFolder[2048];
    // char *ketemu;
    char buf[2048];
    char temp[2048];
    // char slash = '/';
    // char dot = '.';


    memset(fullPath, 0, sizeof(fullPath));
    memset(rpath, 0, sizeof(rpath));
    memset(namaFile, 0, sizeof(namaFile));
    memset(namaFolder, 0, sizeof(namaFolder));
    memset(tipe, 0, sizeof(tipe));


    int i;
    bool isEncrypted = false;

    // if(!strcmp(path, "/")){
    //     path = "/home/dyandra/Downloads";
    //     strcpy(fullPath, path);
    // }
    char *ketemu;
    if (strcmp(path, "/")){
        ketemu = strstr(path, en1);
        if(ketemu){
            isEncrypted = true;
            flagGlobal = 1;
            ketemu++;
        }
    }

    if(!strcmp(path, "/")){
        path = dirpath;
        strcpy(fullPath, path);
    }

    else if (isEncrypted){
        // int minus = strlen(path) - strlen(ketemu);
        strncpy(rpath, path, strlen(path) - strlen(ketemu));

        strcpy(buf, ketemu);
        char *a; 
        char *b = buf;

        i = 0;
        a=strtok_r(b, "/", &b);
        for(;a!=NULL;a=strtok_r(NULL, "/", &b)){
            memset(temp, 0, sizeof(temp));

            if (!i){
                strcpy(temp, a);
                strcat(rpath, temp);

                i = 1;
                
                continue;
            }

            strcpy(tipe, rpath);
            strcat(tipe, "/");
            // strcat(rpath, slash);
            strcat(tipe, a);

            int lengthTipe = strlen(tipe);
            int lengthPath = strlen(path);

            if (lengthTipe != lengthPath){
                strcpy(namaFolder, a);
                int j = 0;
                for(; j<strlen(namaFolder); j++){
                    if(namaFolder[j] >= 'A' && namaFolder[j] <= 'Z'){
                        namaFolder[j] = 'Z' + 'A' - namaFolder[j];
                    }

                    else if (namaFolder[j] >= 'a' && namaFolder[j] <= 'z'){
                        namaFolder[j] = 'z' + 'a' - namaFolder[j];
                    }
                }
                strcat(rpath, namaFolder);
            }

            else if(lengthTipe == lengthPath){
                char Folder[2048];
                strcpy(Folder, dirpath);
                strcat(rpath, "/");
                strcat(Folder, rpath);
                strcat(Folder, a);

                // printf("%s", Folder);
                DIR *dp = opendir(Folder);

                if(dp){
                    strcpy(namaFolder, a);
                    int j = 0;
                    for(; j<strlen(namaFolder); j++){
                        if(namaFolder[j] >= 'A' && namaFolder[j] <= 'Z'){
                            namaFolder[j] = 'Z' + 'A' - namaFolder[j];
                        }

                        else if (namaFolder[j] >= 'a' && namaFolder[j] <= 'z'){
                            namaFolder[j] = 'z' + 'a' - namaFolder[j];
                        }
                    }
                    strcat(rpath, namaFolder);
                }

                else if(dp == 0){
                    char *titik = strchr(a, '.');

                    if(titik != 0){
                        int minus2 = strlen(a) - strlen(titik);
                        strncpy(namaFile, a, minus2);

                        int j = 0;
                        for(; j<strlen(namaFile); j++){
                            if(namaFile[j] >= 'A' && namaFile[j] <= 'Z'){
                               namaFile[j] = 'Z' + 'A' - namaFile[j];
                            }

                            else if (namaFile[j] >= 'a' && namaFile[j] <= 'z'){
                                namaFile[j] = 'z' + 'a' - namaFile[j];
                            }
                        }
                        strcat(namaFile, titik) ;
                        }
                        // strcat(namaFile, titik) ;
                    

                    else{
                        strcpy(namaFile, a);
                       int j = 0;
                        for(; j<strlen(namaFile); j++){
                            if(namaFile[j] >= 'A' && namaFile[j] <= 'Z'){
                               namaFile[j] = 'Z' + 'A' - namaFile[j];
                            }

                            else if (namaFile[j] >= 'a' && namaFile[j] <= 'z'){
                                namaFile[j] = 'z' + 'a' - namaFile[j];
                            }
                        }

                    }strcat(rpath, namaFile);
                }
            }



        }
        strcpy(fullPath, dirpath);
        strcat(fullPath, rpath);
    }
    else{
        strcpy(fullPath, dirpath);
        strcat(fullPath, path);

    }
    char* result = fullPath;
    return result;
}


static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
    // path = /abcde
    char fullPath[1000];
    memset(fullPath, 0, 1000) ;
    flagGlobal = 0 ;
    strcpy(fullPath, getDirFile(path)) ;

    int res = 0 ;
	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fullPath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;

        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
            res = (filler(buf, de->d_name, &st, 0)) ;
        }
        else if (flagGlobal) {
            if (de->d_type & DT_DIR) {
                char temp[1024] ;
                memset(temp, 0, 1024) ;
                strcpy(temp, de->d_name) ;
                int j;
                for(j = 0; j<strlen(temp); j++){
                        if(temp[j] >= 'A' && temp[j] <= 'Z'){
                        temp[j] = 'Z' + 'A' - temp[j];
                    }

                    else if (temp[j] >= 'a' && temp[j] <= 'z'){
                        temp[j] = 'z' + 'a' - temp[j];
                    }
                }
                res = (filler(buf, temp, &st, 0));
            }
            else {
                char* dot ;
                dot = strchr(de->d_name, '.') ;
                
                char namaFile[1024] ;
                memset(namaFile, 0, 1024) ;
                if (dot) {
                    strncpy(namaFile, de->d_name, strlen(de->d_name) - strlen(dot)) ;
                        int i;
                        for(i = 0; i<strlen(namaFile); i++){
                            if(namaFile[i] >= 'A' && namaFile[i] <= 'Z'){
                                namaFile[i] = 'Z' + 'A' - namaFile[i];
                            }

                            else if (namaFile[i] >= 'a' && namaFile[i] <= 'z'){
                                namaFile[i] = 'z' + 'a' - namaFile[i];
                            }
                        }
                    strcat(namaFile, dot) ;
                }
                else {
                    strcpy(namaFile, de->d_name) ;
                        int i;
                        for(i = 0; i<strlen(namaFile); i++){
                            if(namaFile[i] >= 'A' && namaFile[i] <= 'Z'){
                                namaFile[i] = 'Z' + 'A' - namaFile[i];
                            }

                            else if (namaFile[i] >= 'a' && namaFile[i] <= 'z'){
                                namaFile[i] = 'z' + 'a' - namaFile[i];
                            }
                        }
                }
                res = (filler(buf, namaFile, &st, 0));
            }
        }
        else 
            res = (filler(buf, de->d_name, &st, 0));
        

        if(res!=0) break;
	}

	closedir(dp);
	return 0;
}


static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{

    char fullPath[2048];
    memset(fullPath, 0, sizeof(fullPath));

    // char getPath[2048] = getDirFile(path);
    strcpy(fullPath, getDirFile(path));

    int fd;
    int res;
    (void) fi;

    fd = open(fullPath, O_RDONLY);

    if (fd == -1) return -errno;

    res = pread(fd, buf, size, offset);

    if (res == -1) res = -errno;

    close(fd);

    return res;
}

static  int  xmp_getattr(const char *path, struct stat *stbuf)
{
    char fullPath[2048];
    memset(fullPath, 0, sizeof(fullPath));

    // char getPath[2048] = getDirFile(path);
    strcpy(fullPath, getDirFile(path));

    int res;
    res = lstat(fullPath, stbuf);

    if (res == -1) return -errno;
    return 0;
}

void log_rename(char *src, char *dst){
    char *lastSlash = strchr(dst, '/');
    if (strstr(lastSlash, "/AtoZ_")) {
        char t1[1024] ; bzero(t1, 1024) ;
        char t2[1024] ; bzero(t2, 1024) ;
        sprintf(t1, "%s%s", dirpath, src) ;
        sprintf(t2, "%s%s", dirpath, dst) ;
        logRecord(t1, t2, 1) ;
    }
}


void log_mkdir(char *path){
    char* lastSlash = strchr(path, '/') ;
    if (strstr(lastSlash, "/AtoZ_")) {
    char temp[1024] ; bzero(temp, 1024) ;
    sprintf(temp, "%s%s", dirpath, path) ;
    logRecord("null", temp, 2) ;
    }
}

void logRecord(char old_dir[], char new_dir[], int mode) {
    FILE* file = fopen("encode.log", "a") ;

    char str[2048] ;
    if (mode == 1) {
        sprintf(str, "%s --> %s", old_dir, new_dir) ;

        fprintf(file, "%s\n", str) ;
    }
    else if (mode == 2) {
        sprintf(str, "%s", new_dir) ;

        fprintf(file, "%s\n", str) ;
    }

    fclose(file) ;
}

static int xmp_rename(const char *from, const char *to)
{

    log_rename(from, to);
    char f_from[2048]; 
    char f_to[2048];
    memset(f_from, 0, sizeof(f_from)) ; 
    memset(f_to, 0, sizeof(f_to)) ;
    strcpy(f_from, getDirFile(from)) ;
    strcpy(f_to, getDirFile(to)) ;

	int res;

	res = rename(f_from, f_to);
	if (res == -1)
		return -errno;

	return 0;
}



static int xmp_mkdir(const char *path, mode_t mode)
{
    log_mkdir(path);
    char fullPath[2048] ;
    memset(fullPath, 0, sizeof(fullPath)) ;
    strcpy(fullPath, getDirFile(path)) ;
    
	int res;

	res = mkdir(fullPath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .rename = xmp_rename,
    .mkdir = xmp_mkdir,
};

/* End XMP Field */

int  main(int  argc, char *argv[])
{
    
    umask(0);

    return fuse_main(argc, argv, &xmp_oper, NULL);
}



