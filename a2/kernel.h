#ifndef KERNEL
#define KERNEL
#include "pcb.h"
int process_initialize(char *filename, int pid);
int schedule_by_policy(char* policy); //, bool mt);
int shell_process_initialize();
void ready_queue_destory();
int load_page_to_memory(FILE *fp, int pid, PAGE** page_table, PCB* pcb);
#endif