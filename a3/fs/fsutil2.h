//Yixuan Qin, 261010963 Yongru Pan, 261001758
#ifndef FILESYS_FSUTIL2_H
#define FILESYS_FSUTIL2_H

#include <stdbool.h>
#include "block.h"

int copy_in(char *fname);
int copy_out(char *fname);
void find_file(char *pattern);
void fragmentation_degree();
int defragment();
void recover(int flag);


#endif /* fs/fsutil2.h */
