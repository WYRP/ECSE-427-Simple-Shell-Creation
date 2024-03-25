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
#include "../interpreter.h"


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

/*copy from the real hard drive to the shell hard drive*/
int copy_in(char *fname) {
  int size = get_file_size(fname);
  char *buffer = malloc((size + 1) * sizeof(char));
  memset(buffer, 0, (size + 1));

  FILE *f = fopen(fname, "r");
  if (f == NULL){
    return FILE_DOES_NOT_EXIST;
  }
  fgets(buffer, size, f);
  fclose(f);


  int shell_space = fsutil_freespace();
  if (shell_space == 0) {
    printf("No space left on the shell hard drive\n");
    return NO_MEM_SPACE;
  }
  if (size > shell_space) {
    fsutil_create(fname, shell_space);
    fsutil_write(fname, buffer, shell_space);
    free(buffer); 
    printf("Warning: could only write %d out of %d bytes (reached end of file)", shell_space, size);
    return 0; //partial write
  }
  else{
    fsutil_create(fname, size);
    fsutil_write(fname, buffer, size);
    free(buffer);
    return 0; //success
  }
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
}

void fragmentation_degree() {
  struct dir *dir;
  char name[NAME_MAX + 1];
  int framentable_counter = 0;
  int fragmented_counter = 0;
  int fragmentation_degree = 0;

  dir = dir_open_root();
  if (dir == NULL)
    return 0; // Directory not found
  while (dir_readdir(dir, name)){
    if (fsutil_size(name) > 512){
      struct file *f = get_file_by_fname(name);
      struct inode *fileNode = file_get_inode(f); //fileNode contains the inode of the file f
      block_sector_t* mySectors = get_inode_data_sectors(fileNode); //sector indecies of the file f
      if (get_num_fragmented(mySectors)){
        fragmented_counter++;
      }
      framentable_counter++;
    }
  }
  fragmentation_degree = (fragmented_counter / framentable_counter);
  dir_close(dir);
  print("framentation degree: %d \n", fragmentation_degree);
  return; 
}

//helper function to check if a file is fragmented
//returns boolean value
bool get_num_fragmented(block_sector_t* mySectors){
  for (int i = 0; i < sizeof(mySectors); i++){
    if (mySectors[i+1] - mySectors[i] > 3 ){
      return true;
  }
  return false;
}

//This function can be partly implemented by reading all files into memory (not necessarily the shell
// memory, just a buffer);
//???? am I doing this right?
int defragment() {
  struct dir *dir;
  char name[NAME_MAX + 1];
  int size_of_all_files = num_free_sectors(); //not sure if this is the right function to use
  char *buffer = malloc(size_of_all_files);

  printf("Files in the root directory:\n");
  dir = dir_open_root();
  if (dir == NULL)
    return 1;
  while (dir_readdir(dir, name))
    buffer = strcat(buffer, fsutil_read(name, buffer, fsutil_size(name)));
  dir_close(dir);

  // then I suppose I need to redistribute the files into the file system again.
  return 0;
}

void recover(int flag) {
  if (flag == 0) { // recover deleted inodes

    // TODO
    //scan all empty sectors
    int start = 0;
    int i = bitmap_scan(free_map, start, bitmap_size(free_map), 0);
    while(i != BITMAP_ERROR && start < bitmap_size(free_map)){
        //check if contains inode that was deleted
        struct inode *inode = inode_open(i);

        if (inode != NULL && inode_is_removed(inode)){
          //recover
          inode->removed = false;
          int size = inode_length(inode);
          char *buffer = malloc(size);
          char fname[100];
          sprintf(fname, "recovered0-%d", i);
          inode_read_at(inode, buffer, size, 0);
          fsutil_create(fname, size);
          fsutil_write(fname, buffer, size);
        }
        start++;
        i = bitmap_scan(free_map, start, bitmap_size(free_map), 0);
    }
  } else if (flag == 1) { // recover all non-empty sectors

    // TODO
  } else if (flag == 2) { // data past end of file.

    // TODO
  }
}