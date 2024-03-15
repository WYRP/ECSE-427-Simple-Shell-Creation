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
printf("I inside copy in\n");
FILE *f = fopen(fname, "r");
if (f == NULL){
  printf("Error: could not create file\n");
    return 9; //file create error
}
else {
  char *buffer;
  int size = get_file_size(fname);

  buffer = malloc(size * sizeof(char));
  char *content_from_read = fgets(buffer, size, f);
  //chekc available space in the shell hard drive
  if (fsutil_freespace() < size){
    //put as much content as possible from the real hard drive 
    // to the shell hard drive
    int available_space = fsutil_freespace();
    long mylong = (long)content_from_read;
    char* avail_buffer = malloc(available_space * sizeof(char));
    if (fsutil_create(fname, available_space)==0){
    fsutil_write(fname, avail_buffer, available_space);
    printf("Warning: could only write %d out of %ld bytes (reached end of file", available_space, mylong);
    return 11; //file write error 
    }
  }
  else {
    //create new file with the same information in the shell hard drive
    fsutil_create(fname, size);
    if (fsutil_create(fname, size) == 0){
      fsutil_write(fname, buffer, size);
      fsutil_close(fname);
    }
  }
  free(buffer);
  fclose(f);
  }
return 0; // file does not exist error flag
}

/*copy contents from the shell hard drive to the real hard drive*/
int copy_out(char *fname) {
  if (fsutil_ls(fname) == 0){
    int size = fsutil_size(fname);
    char *buffer = malloc(size * sizeof(char));
    int content_read_from_file;
    //chekc available space in the real hard drive
    if (get_free_space() < size){
      return 1; //no memory space error flag
    }
    else{
      content_read_from_file = fsutil_read(fname, buffer, size);
      if (content_read_from_file == -1){
        printf("Error: could not read from file in Shell Memory\n");
        return 2; //file read error flag
      }
      else {
        FILE *f = fopen(fname, "w");
        if (f == NULL){
          printf("Error: could not create file\n");
          return 3; //file create error flag
        }
        else {
          fwrite(buffer, 1, size, f);
          fclose(f);
          fsutil_close(fname);
          return -1; //success flag
        }
      }
    }
  }
  return 0; //file does not exist error flag
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