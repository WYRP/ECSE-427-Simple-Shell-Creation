#ifndef SHELLMEMORY_H
#define SHELLMEMORY_H
#include <stdio.h>
#include "pcb.h"


// Use macros for frame store and variable store sizes
#define FRAME_STORE_SIZE framesize
#define VARIABLE_STORE_SIZE varmemsize

struct memory_struct {
    char *var;
    char *value;
};

typedef struct LruQueueNode {
    PAGE* page;
    struct LruQueueNode* next;
}LruQueueNode;

void mem_init();
char *mem_get_value(char *var);
void mem_set_value(char *var_in, char *value_in);
void LRU_queue_add_to_tail(LruQueueNode* node);
LruQueueNode *LRU_queue_pop_head();
int allocate_frame(char *var_in, char *value_in, PCB* pcb);
int load_file(FILE* fp, int* pStart, int* pEnd, char* fileID);
void copy_to_backing_store(char* script);
char* mem_get_value_at_line(int index);
void mem_free_lines_between(int start, int end);
void mem_free_line(int i);
void printShellMemory();
void clear_variable_store();
#endif
