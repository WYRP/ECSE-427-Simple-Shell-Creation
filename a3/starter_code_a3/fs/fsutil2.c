#include "fsutil2.h"
#include "bitmap.h"
#include "cache.h"
#include "debug.h"
#include "directory.h"
#include "file.h"
#include "filesys.h"
#include "free-map.h"
#include "fsutil.h"
#include "inode.h"
#include "off_t.h"
#include "partition.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/statvfs.h>

//helper function to get system file size
int get_file_size(char *fname) {
  struct stat st;
  stat(fname, &st);
  return st.st_size;
}

//helper function to get system free space
int get_free_space() {
  struct statvfs buf;
  statvfs(".", &buf);
  return buf.f_bsize * buf.f_bfree;
}

/*copy from the real hard drive to the shell hard drive*/
int copy_in(char *fname) {
  int size = get_file_size(fname);
  char *buffer = malloc((size + 1) * sizeof(char));
  memset(buffer, 0, (size + 1));

  FILE *f = fopen(fname, "r");
  if (f == NULL){
    printf("File does not exist \n");
    return 0;
  }
  fgets(buffer, size, f);
  fclose(f);

  fsutil_create(fname, size);
  fsutil_write(fname, buffer, size);
return 0; // file does not exist error flag
}
 
/*copy contents from the shell hard drive to the real hard drive*/
int copy_out(char *fname) {
  int size = fsutil_size(fname);
  char *buffer = malloc((size + 1) * sizeof(char));
  memset(buffer, 0, (size + 1));
  fsutil_read(fname, buffer, size);

  FILE *f = fopen(fname, "w");
  if (f == NULL){
    printf("File does not exist \n");
    return 0;
  }
  fputs(buffer, f);
  fclose(f);
  return -1; 
}

void find_file(char *pattern) {
  struct dir *dir;
  dir = dir_open_root();
  char name[NAME_MAX + 1];

  if (dir == NULL){
    printf("Directory not found\n");
    return;
  }
  while (dir_readdir(dir, name)){
    char *buffer = malloc((fsutil_size(name) + 1) * sizeof(char));
    fsutil_read(name, buffer, fsutil_size(name));
    if (strstr(buffer, pattern) != NULL) {
      printf("%s\n", name);
    }
    free(buffer);
  }

  dir_close(dir);
  return;

  // //used part of the code from fsutil_ls to create a 
  // //an array of file names in the root directory
  // struct dir *dir;
  // char name[NAME_MAX + 1]; //NAME_MAX = 30
  // dir = dir_open_root();
  // int num_files = 0;
  // if (dir == NULL){
  //   printf("Directory not found\n");
  //   return;
  // }
  // while (dir_readdir(dir, name))
  //   num_files++;
  // const char *ls_of_files[num_files];

  // int counter = 0;
  // while (dir_readdir(dir, name)){
  //   ls_of_files[counter] = name;
  //   counter++;
  // }
  // dir_close(dir);

  // for (int i = 0; i < num_files-1; i++) {
  //   const char *file_name = ls_of_files[i];
  //   char *buffer = malloc((fsutil_size(file_name) + 1) * sizeof(char));
  //   fsutil_read(fsutil_size(file_name), buffer, file_name);
  //   if (strstr(buffer, pattern) != NULL) {
  //     printf("Pattern Found%s\n", file_name);
  //   }
  // }
  // printf("Pattern not found\n");
  // return;
}

void fragmentation_degree() {
  // TODO
}

int defragment() {
  // TODO
  return 0;
}

void recover(int flag) {
  if (flag == 0) { // recover deleted inodes

    // TODO
  } else if (flag == 1) { // recover all non-empty sectors

    // TODO
  } else if (flag == 2) { // data past end of file.

    // TODO
  }
}