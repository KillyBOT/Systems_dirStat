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

#define TYPE_FILE 8
#define TYPE_FOLDER 4

long getDirSize(DIR* dir){
  int size = 0;
  struct dirent* dirStream = malloc(sizeof(struct dirent*));
  dirStream = readdir(dir);
  while(dirStream != NULL) {
    if(dirStream->d_type == TYPE_FOLDER)size++;
    dirStream = readdir(dir);
  }
  free(dirStream);
  return size;
}

long getFileStats(char* path, struct dirent* dirStream, int indent){
  
  DIR* dir;
  int temp = 0b100000000;
  long size;
  struct stat* fs = malloc(sizeof(struct stat));
  char finalPath[512];
  strcpy(finalPath, path);
  strcat(finalPath, "/");
  strcat(finalPath, dirStream->d_name);

  for(int x = 0; x < indent; x++) printf("\t");

  if(stat(finalPath, fs) < 0) {
    printf("Error getting stats on file %s, %d: %s\n", finalPath, errno, strerror(errno));
  }

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
    printf(" %ld", getDirSize(dir));
    closedir(dir);
  } else printf(" 1");

  printf(" %4d %4d", fs->st_uid,fs->st_gid);

  printf(" %8ld", fs->st_size);

  printf(" %.19s", ctime(&(fs->st_atime)));

  printf(" %s", dirStream->d_name);

  printf("\n");

  size = fs->st_size;

  free(fs);

  return size;
}

long getDirStats(char* path, int indent){
  DIR* dir = opendir(path);
  struct dirent* dirStream = malloc(sizeof(struct dirent*));
  long totalSize = 0;
  if(dir == NULL){
    printf("Failed to open directory%s: %s\n", path,strerror(errno));
    free(dirStream);
    closedir(dir);
    return 0;
  }

  dirStream = readdir(dir);
  while(dirStream != NULL){

    totalSize += getFileStats(path,dirStream,indent);

    if(dirStream->d_type == TYPE_FOLDER && strcmp(dirStream->d_name, "..") != 0 && strcmp(dirStream->d_name,".") != 0){
      printf("\n");
      char newPath[512];

      sprintf(newPath, "%s/%s", path, dirStream->d_name);

      totalSize += getDirStats(newPath,indent + 1);
    }


    dirStream = readdir(dir);

  }

  free(dirStream);
  closedir(dir);
  printf("\n");

  return totalSize;
}

int main(int argc, char* argv[]){
  char buffer[128];
  long totalSize = getDirStats(argv[1],0);
  sprintf(buffer, "%ldB, %ldKB, %ldMB, %ldGB",totalSize,totalSize / 1024, totalSize / 1046576, totalSize / 1073741824);
  printf("\n\nTotal size: %s\n",buffer);
}