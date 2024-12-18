//Yixuan Qin, 261010963 Yongru Pan, 261001758

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
#include <stdbool.h>

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

  FILE *f = fopen(fname, "rb");
  if (f == NULL){
    return FILE_DOES_NOT_EXIST;
  }
  fread(buffer, size + 1, 1, f);
  fclose(f);


  int shell_space = fsutil_freespace();
  if (shell_space == 0) {
    printf("No space left on the shell hard drive\n");
    return NO_MEM_SPACE;
  }
  if (size + 1 > shell_space) {
    fsutil_create(fname, shell_space);
    fsutil_write(fname, buffer, shell_space);
    free(buffer); 
    printf("Warning: could only write %d out of %d bytes (reached end of file)", shell_space, size);
    return 0; //partial write
  }
  else{
    fsutil_create(fname, size + 1);
    fsutil_write(fname, buffer, size + 1);
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
    return FILE_DOES_NOT_EXIST;
  }
  fputs(buffer, f);
  fclose(f);
  return 0; 
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
    char *buffer = malloc((fsutil_size(name)) * sizeof(char));
    fsutil_read(name, buffer, fsutil_size(name));
    if (strstr(buffer, pattern) != NULL) {
      printf("%s\n", name);
    }
    free(buffer);
  }

  dir_close(dir);
  return;
}

//helper function to check if a file is fragmented
//returns boolean value
bool is_file_fragmented(block_sector_t* mySectors, size_t size){
  for (int i = 0; i < size - 1; i++){
    if (abs(mySectors[i+1] - mySectors[i]) >= 6 ){
      return true;
    }
  }
  return false;
}

void fragmentation_degree() {
  struct dir *dir;
  char name[NAME_MAX + 1];
  int fragmentable_counter = 0;
  int fragmented_counter = 0;
  float fragmentation_degree = 0;

  dir = dir_open_root();
  if (dir == NULL){
    printf("Directory not found\n");
    return; // Directory not found
  }
  while (dir_readdir(dir, name)){
    if (fsutil_size(name) > 512){
      struct file *f = filesys_open(name);
      struct inode *fileNode = file_get_inode(f); //fileNode contains the inode of the file f
      block_sector_t* mySectors = get_inode_data_sectors(fileNode); //sector indecies of the file f
      offset_t fileSize = inode_length(fileNode);
      if (is_file_fragmented(mySectors, bytes_to_sectors(fileSize))){
        fragmented_counter++;
      }
      fragmentable_counter++;
    }
  }
  fragmentation_degree = (float) fragmented_counter / fragmentable_counter;
  dir_close(dir);
  printf("Num fragmentable files: %d\n", fragmentable_counter);
  printf("Num fragmented files: %d\n", fragmented_counter);
  printf("Fragmentation pct: %f\n", fragmentation_degree);
  return; 
}

//This function can be partly implemented by reading all files into memory (not necessarily the shell
// memory, just a buffer);
int defragment() {
  struct dir *dir;
  char name[NAME_MAX + 1];
  size_t size_of_all_files = bitmap_size(free_map) - num_free_sectors();
  char *buffer = malloc(size_of_all_files * 512);
  int fileNumber = 0;

  //read all file content into a buffer
  //count the number of files
  dir = dir_open_root();
  if (dir == NULL){
    return FILE_DOES_NOT_EXIST;
  }
  while (dir_readdir(dir, name)){
    fileNumber++;
    char *temp_buffer = malloc(fsutil_size(name));
    fsutil_read(name, temp_buffer, fsutil_size(name));
    strcat(buffer, temp_buffer);
    free(temp_buffer);
  }
  dir_close(dir);

  int file_sizes[fileNumber];
  char file_names[fileNumber][NAME_MAX + 1];
  int file_idx = 0;

  //record each file's size into file_sizes
  //record each file's name into file_names
  dir = dir_open_root();
  if (dir == NULL){
    return FILE_DOES_NOT_EXIST;
  }
  while (dir_readdir(dir, name)){
    file_sizes[file_idx] = fsutil_size(name);
    strcpy(file_names[file_idx], name);
    // printf("file name: %s\n", file_names[file_idx]);
    file_idx++;
  }
  dir_close(dir);

  dir = dir_open_root();
  if (dir == NULL){
    return FILE_DOES_NOT_EXIST;
  }

  //remove all files
  for (int i = 0; i < fileNumber; i++){
    fsutil_rm(file_names[i]);
  }

  //recreate all the files
  int offset = 0;
  for (int i = 0; i < fileNumber; i++){
    char *tmp = malloc(file_sizes[i]);
    strncpy(tmp, buffer + offset, file_sizes[i]);
    fsutil_create(file_names[i], file_sizes[i]);
    fsutil_write(file_names[i], tmp, file_sizes[i]);
    free(tmp);
    offset += file_sizes[i];
  }
  dir_close(dir);
  free(buffer);
  return 0;
}

void remove_nulls(char *buf, size_t *len) {
    char *null_pos;
    size_t shift_len;

    while ((null_pos = memchr(buf, '\0', *len)) != NULL) {
        // Calculate the number of bytes to shift
        shift_len = *len - (null_pos - buf) - 1;

        // Shift the memory to overwrite the null byte
        memmove(null_pos, null_pos + 1, shift_len);

        // Decrease the buffer length
        (*len)--;
    }
}

void remove_first_n_chars(char *str, int n) {
    int len = strlen(str);
    if (n > len) n = len; 

    for (int i = 0; i <= len - n; i++) {
        str[i] = str[i + n];
    }
}

void recover(int flag) {
  size_t block_sector_size = 512;

  if (flag == 0) { // recover deleted inodes
    //search for all empty sectors
    for (size_t i = 0; i < bitmap_size(free_map); i++) {
      //sector i is free - potential removed inode
      if (!bitmap_test(free_map, i)) {
        //checks if it's associated to a file inode
        struct inode *inode = inode_open(i);
        if (inode != NULL && inode->data.magic == INODE_MAGIC){
          if (inode_is_directory(inode)){
            continue;
          }
          //recover
          offset_t size = inode_length(inode);
          char *buffer = malloc(size);
          char fname[100];
          sprintf(fname, "recovered0-%d", (int) i);
          inode_read_at(inode, buffer, size, 0);
          struct dir* dir = dir_open_root();
          dir_add(dir, fname, i, false);
          dir_close(dir);
          free(buffer);
        }
      }
    }
  } else if (flag == 1) { // recover all non-empty sectors
    size_t start = 4; // Begin at sector 4, skipping reserved sectors
    for (size_t i = start; i < bitmap_size(free_map); i++) {
      if (bitmap_test(free_map, i)) { // Sector is not free, potential data remnants
        char *buffer = malloc(BLOCK_SECTOR_SIZE);
        if (buffer == NULL) {
            break; // Insufficient memory to proceed
        }
        block_read(fs_device, i, buffer);
        remove_nulls(buffer, &block_sector_size);
        size_t actual_length = strlen(buffer);
        char filename[32];
        sprintf(filename, "recovered1-%d.txt", (int) i);
        FILE *file = fopen(filename, "w");
        fwrite(buffer, actual_length, 1, file);
        fclose(file);
        free(buffer);
      }
    }
  } else if (flag == 2) { // data past end of file.
    //find data pass the end of the file then save it to recovered2-%s.txt
    struct dir *dir;
    char name[NAME_MAX + 1];
    dir = dir_open_root();
    if (dir == NULL){
      printf("Directory not found\n");
      return;
    }
    while (dir_readdir(dir, name)){
      struct file *file = filesys_open(name);
      int fileSize = fsutil_size(name);
      if(fileSize <= 0 || fileSize % 512 == 0){
        continue; //not possible for this file to have hidden data
      }
      int num_overflow_chars = fileSize % 512;
      //num_overflow_chars = 56
      struct inode *inode = file_get_inode(file);
      size_t numBlocks = bytes_to_sectors(inode_length(inode));

      // find the data in its last block sector
      block_sector_t* sectors = get_inode_data_sectors(inode);
      block_sector_t last_block = sectors[numBlocks - 1];
      char *buffer = malloc(BLOCK_SECTOR_SIZE);
      buffer_cache_read(last_block, buffer); //read sector data into buffer

      char result_buffer[BLOCK_SECTOR_SIZE];
      for(int i = num_overflow_chars; i < BLOCK_SECTOR_SIZE; i++){
        if (buffer[i] == '\0'){
          continue;
        }
        result_buffer[strlen(result_buffer)] = buffer[i];
      }
      result_buffer[strlen(result_buffer)] = '\0';
      char fname[100];
      sprintf(fname, "recovered2-%s.txt", name);
      FILE *f = fopen(fname, "wb");
      if (f == NULL){
        return;
      }
      fputs(result_buffer, f);
      fclose(f);
      free(buffer);
    }
    dir_close(dir);
  }
}

