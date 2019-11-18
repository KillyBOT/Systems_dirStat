#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <math.h>

#define TYPE_FILE 8
#define TYPE_FOLDER 4

long getDirNum(DIR* dir){
  long size = 0;
  struct dirent* dirStream = malloc(sizeof(struct dirent));

  dirStream = readdir(dir);

  while(dirStream != NULL) {
    if(dirStream->d_type == TYPE_FOLDER)size++;
    dirStream = readdir(dir);
  }

  free(dirStream);

  return size;
}

int getLargestDirNum(char* path, DIR* dir){
  int size = (int)log10(getDirNum(dir)) + 1;
  DIR* subDir;
  struct dirent* dirStream = malloc(sizeof(struct dirent));
  char folderPath[512];

  dirStream = readdir(dir);

  while(dirStream != NULL) {
    if(dirStream->d_type == TYPE_FOLDER && strcmp(dirStream->d_name, ".") != 0){
      sprintf(folderPath, "%s/%s",path,dirStream->d_name);
      subDir = opendir(folderPath);
      if((int)log10(getDirNum(subDir)) > size) size = (int)log10(getDirNum(subDir));
      closedir(subDir);
    }
  }

  free(dirStream);

  return size + 1;
}

int getLargestFileSize(char* path, DIR* dir){
  int size = 0;
  char filePath[512];
  struct dirent* dirStream = malloc(sizeof(struct dirent));
  struct stat* fs = malloc(sizeof(struct stat));

  dirStream = readdir(dir);

  while(dirStream != NULL) {
    sprintf(filePath, "%s/%s",path,dirStream->d_name);
    stat(filePath, fs);

    if((int)log10(fs->st_size) > size) size = (int)log10(fs->st_size);

    dirStream = readdir(dir);
  }

  free(dirStream);
  free(fs);

  return size + 1;
}

long getFileStats(char* path, struct dirent* dirStream, int lDirNum, int lFileSize){
  
  DIR* dir;
  int temp = 0b100000000;
  long size;
  struct stat* fs = malloc(sizeof(struct stat));
  char finalPath[512];
  strcpy(finalPath, path);
  strcat(finalPath, "/");
  strcat(finalPath, dirStream->d_name);

  if(stat(finalPath, fs) < 0) printf("Error getting stats on file %s, %d: %s\n", finalPath, errno, strerror(errno));

  if(dirStream->d_type == TYPE_FOLDER) printf("d");
  else printf("-");

  for(int x = 0; x < 9; x++){
    if((temp & fs->st_mode) != 0){
      if((x % 3) == 0) printf("r");
      if((x % 3) == 1) printf("w");
      if((x % 3) == 2) printf("x");
    }else{
      printf("-");
    }

    temp >>= 1;
  }

  if(dirStream->d_type == TYPE_FOLDER) {
    dir = opendir(finalPath);
    printf(" %*ld", lDirNum,getDirNum(dir));
    closedir(dir);
  } else printf(" %*d",lDirNum, 1);

  printf(" %4d %4d", fs->st_uid,fs->st_gid);

  printf(" %*ld", lFileSize, fs->st_size);

  printf(" %.19s", ctime(&(fs->st_atime)));

  if(dirStream->d_type == TYPE_FOLDER)printf(" \033[1;34m%s", dirStream->d_name);
  else if((fs->st_mode & 0b1000000) == 0b1000000)printf(" \033[1;32m%s", dirStream->d_name);
  else printf(" %s", dirStream->d_name);

  printf("\033[0m\n");

  size = fs->st_size;

  free(fs);

  return size;
}

long getDirStats(char* path, int ifR, int indent){
  DIR* dir;
  struct dirent* dirStream;
  long totalSize;
  int lFileSize;
  int lDirNum;
  char newPath[512];

  dir = opendir(path);

  if(dir == NULL){
    printf("Failed to open directory %s: %s\n", path,strerror(errno));
    free(dirStream);
    closedir(dir);
    return totalSize;
  }

  dirStream = malloc(sizeof(struct dirent*));

  totalSize = 0;
  
  lFileSize = getLargestFileSize(path, dir);
  closedir(dir);

  dir = opendir(path);
  lDirNum = getLargestDirNum(path, dir);
  closedir(dir);

  dir = opendir(path);

  dirStream = readdir(dir);

  while(dirStream != NULL){
    
    for(int x = 0; x < indent; x++) printf("\t");

    totalSize += getFileStats(path,dirStream,lDirNum,lFileSize);

    if(ifR && dirStream->d_type == TYPE_FOLDER && strcmp(dirStream->d_name, "..") != 0 && strcmp(dirStream->d_name,".") != 0){
      sprintf(newPath, "%s/%s", path, dirStream->d_name);
      totalSize += getDirStats(newPath,ifR,indent + 1);
    }

    dirStream = readdir(dir);

  }

  free(dirStream);
  closedir(dir);

  return totalSize;
}

int main(int argc, char* argv[]){
  char buffer[128];
  int ifR = (argc > 2 && strcmp(argv[2],"-r") == 0) ? 1 : 0;
  long totalSize = getDirStats(argv[1],ifR,0);
  sprintf(buffer, "%ldB, %ldKB, %ldMB, %ldGB",totalSize,totalSize / 1024, totalSize / 1046576, totalSize / 1073741824);
  printf("\n\nTotal size: %s\n",buffer);
}