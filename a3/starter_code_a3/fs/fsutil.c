#include "fsutil.h"
#include "cache.h"
#include "debug.h"
#include "directory.h"
#include "file.h"
#include "filesys.h"
#include "free-map.h"
#include "inode.h"
#include "partition.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* List files in the root directory. */
int fsutil_ls(char *argv UNUSED) {
  struct dir *dir;
  char name[NAME_MAX + 1];

  printf("Files in the root directory:\n");
  dir = dir_open_root();
  if (dir == NULL)
    return 1;
  while (dir_readdir(dir, name))
    printf("%s\n", name);
  dir_close(dir);
  printf("End of listing.\n");
  return 0;
}

/* Prints the contents of file ARGV[1] to the system console as
   hex and ASCII. */
int fsutil_cat(char *file_name) {
  printf("Printing <%s> to the console...\n", file_name);
  struct file *file_s = get_file_by_fname(file_name);
  char *buffer;

  if (file_s == NULL) {
    file_s = filesys_open(file_name);
    if (file_s == NULL) {
      printf("Error\n");
      return -1;
    }
    add_to_file_table(file_s, file_name);
  }

  file_seek(file_s, 0);
  int file_size = file_length(file_s);
  buffer = malloc(file_size * sizeof(char));
  for (;;) {
    offset_t pos = file_tell(file_s);
    offset_t n = file_read(file_s, buffer, file_size);
    if (n == 0)
      break;

    hex_dump(pos, buffer, n, true);
  }
  free(buffer);
  remove_from_file_table(file_name);
  printf("Done printing\n");
  return 0;
}

/* Deletes file ARGV[1]. */
int fsutil_rm(char *file_name) {
  remove_from_file_table(file_name);
  return filesys_remove(file_name);
}

int fsutil_create(const char *fname, unsigned isize) {
  if (strlen(fname) == 0 || strlen(fname) >= 255) {
    printf("Error with filename: %ld\n", strlen(fname));
    return 0;
  }
  return filesys_create(fname, isize, false); //, true);
}

int fsutil_write(char *file_name, const void *buffer, unsigned size) {
  struct file *file_s = get_file_by_fname(file_name);
  if (file_s == NULL) {
    file_s = filesys_open(file_name);
    if (file_s == NULL) {
      return -1;
    }
    add_to_file_table(file_s, file_name);
  }
  return file_write(file_s, buffer, size);
}

int fsutil_read(char *file_name, void *buffer, unsigned size) {
  struct file *file_s = get_file_by_fname(file_name);
  if (file_s == NULL) {
    file_s = filesys_open(file_name);
    if (file_s == NULL) {
      return -1;
    }
    add_to_file_table(file_s, file_name);
  }
  return file_read(file_s, buffer, size);
}

int fsutil_size(char *file_name) {
  struct file *file_s = get_file_by_fname(file_name);
  if (file_s == NULL) {
    file_s = filesys_open(file_name);
    if (file_s == NULL) {
      return -1;
    }
    add_to_file_table(file_s, file_name);
  }
  offset_t cur_offset = file_s->pos;
  int length = file_length(file_s);
  file_seek(file_s, cur_offset);
  return length;
}

int fsutil_seek(char *file_name, int offset) {
  struct file *file_s = get_file_by_fname(file_name);
  if (file_s == NULL) {
    file_s = filesys_open(file_name);
    if (file_s == NULL) {
      return -1;
    }
    add_to_file_table(file_s, file_name);
  }
  file_seek(file_s, offset);
  return 0;
}

void fsutil_close(char *file_name) { remove_from_file_table(file_name); }

int fsutil_freespace() { return num_free_sectors(); }

void copy_in(File *filename){
  //check if the file exists
        // can use fsutil_ls to check if the file exists
  //open the source file and read
        //maybe we can use fsutil_read to read the file
  //dtermine the file sie to see how much data needs to be copied
        //maybe we can use fsutil_size to get the file size
  //check available space (may need to call external function)
  // create new file in the sysm hard drive with the same name in the root directory
          // maybe we can use fsutil_create to create the file
         //if this cannot be created, return an error flag
  //read from the source file and write to the new file
        // maybe we can use fsutil_write to write to the file
  //close the source file
        //maybe we can use fsutil_close to close the file
  // return success flag

if (fsutil_ls(filename->name) == 0){
  char *buffer;
  int size = fsutil_size(filename->name);
  int read = fsutil_read(filename->name, buffer, size);

  buffer = malloc(size * sizeof(char));
  //chekc available space
  if (fsutil_freespace() < size){
    printf("Not enough space to copy file\n");
    return;
  }
  else {
    //create new file
    if (fsutil_create(filename->name, size) == 0){
      fsutil_write(filename->name, buffer, size);
      fsutil_close(filename->name);
    }
  }
  free(buffer);
      
}
