#ifndef PCB_H
#define PCB_H
#include <stdbool.h>
<<<<<<< HEAD
#define MAX_LINE 1000
=======
#include <time.h>
>>>>>>> parent of 015540e (fix q9)

typedef struct
{   
    int index[3];
    int valid_bits[3];
    int page_index;
    int page_pid;
}PAGE;
/*
 * Struct:  PCB 
 * --------------------
 * pid: process(task) id
 * PC: program counter, stores the index of line that the task is executing
 * start: the first line in shell memory that belongs to this task
 * end: the last line in shell memory that belongs to this task
 * job_length_score: for EXEC AGING use only, stores the job length score
 */

typedef struct
{
    bool priority;
    char* filename;
    int pid;
    int PC[2];
    int job_length_score;
    int last_line_number;
    int last_page_number;
    int line_loaded;
    int LRU_page_number;
    PAGE** page_table;
}PCB;

int generatePID();
PCB* makePCB(PAGE** page_table, char* filename);
PAGE* makePAGE(int page_index,int page_pid);
void set_page_index(PAGE* page, int i, int value);
void set_page_valid_bits(PAGE* page, int i, int value);
void set_pcb_last_line_index(PCB* pcb, int last_page_number);
void set_pcb_last_page_index(PCB* pcb, int n);
void updatePC(PCB* pcb);
void page_table_init(PAGE** page_table);
void set_pcb_line_executed(PCB* pcb, int line_loaded);
#endif