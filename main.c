#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <string.h>

int getFolderStats(){
  DIR* currentDir;
  struct dirent* dir_stream;
  char filePath[512];
  char finalSizeBuffer[128];
  struct stat* currentFileStat = malloc(sizeof(struct stat));
  int totalFileSize = 0;

  currentDir = opendir(argv[1]);
  printf("Opened directory [%s]\n\n",argv[1]);

  dir_stream = readdir(currentDir);
  printf("All files and folders in directory [%s]:\n\n",argv[1]);
  while(dir_stream != NULL){
    printf("Name: %s\n",dir_stream->d_name);
    printf("Type: %s\n",(dir_stream->d_type == 4 ? "Folder" : "File"));

    sprintf(filePath, "%s/%s",argv[1],dir_stream->d_name);

    if(dir_stream->d_type == 8){
      if(stat(filePath,currentFileStat) < 0){
        printf("Error getting stats on file\n");
        printf("%d: %s\n",errno, strerror(errno));
      }

      getFileStats(filePath,currentFileStat);
      totalFileSize += currentFileStat->st_size;
    }

    dir_stream = readdir(currentDir);
    printf("\n");
  }

  sprintf(finalSizeBuffer, "%dB, %dKB, %dMB, %dGB",totalFileSize,totalFileSize / 1024, totalFileSize / 1046576, totalFileSize / 1073741824);
  printf("Total size of files in directory [%s]: %s\n",argv[1],finalSizeBuffer);

  closedir(currentDir);

  free(currentFileStat);
  free(dir_stream);
}

void getFileStats(char* fileName, struct stat* s){

  char buffer[64];
  sprintf(buffer, "%ldB, %ldKB, %ldMB, %ldGB",s->st_size,s->st_size / 1024, s->st_size / 1046576, s->st_size / 1073741824);
  int temp = 0b100000000;

  printf("File stats for file [%s]:\n",fileName);
  printf("File size: %s\n",buffer);
  printf("File permissions: -");
  for(int x = 0; x < 9; x++){
    if((temp & s->st_mode) != 0){
      if((x % 3) == 0) printf("r");
      if((x % 3) == 1) printf("w");
      if((x % 3) == 2) printf("x");
    }else{
      printf("-");
    }

    temp >>= 1;
  }
  printf("\n");
  printf("User ID: %d | Group ID: %d\n",s->st_uid,s->st_gid);
  printf("Last time of access: %s",ctime(&(s->st_atime)));
  printf("Last time of modification: %s",ctime(&(s->st_mtime)));
  printf("Last time of status change: %s",ctime(&(s->st_ctime)));
}

int main(int argc, char* argv[]){
  getFileStats()
}
