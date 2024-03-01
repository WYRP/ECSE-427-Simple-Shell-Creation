// Yixuan Qin, 261010963
// Yongru Pan, 261001758 
#ifndef KERNEL
#define KERNEL
#include "pcb.h"
int count_lines(FILE* file);
int process_initialize(char *filename, int pid);
int schedule_by_policy(char* policy); //, bool mt);
int shell_process_initialize();
void ready_queue_destory();
void load_pages_to_memory(FILE *fp, int pid, PAGE** page_table, PCB* pcb);
void load_missing_page_to_mem(PCB* pcb);
#endif