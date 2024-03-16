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
  //I take this pattern
  //I go through a list of file that is in my root directory
  //there should be a function that I can call to search through the root directory
  //then I iterate over the list of files
  // unless there is a smarter methods
  //then after the for loop ends, I print the file name

  // the for loop should be something like this
  // for the length of the list of files in the root directory
  // read each of the file
  // if inside the file there is a match with the pattern
  // I put the name of the file in a list?
  // or should I just print it out?
  //until I reach the end of the list.
  return;
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