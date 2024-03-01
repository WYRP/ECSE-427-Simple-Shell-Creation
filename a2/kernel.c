// Yixuan Qin, 261010963
// Yongru Pan, 261001758 
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "pcb.h"
#include "kernel.h"
#include "shell.h"
#include "shellmemory.h"
#include "interpreter.h"
#include "ready_queue.h"
#include "interpreter.h"

bool active = false;
bool debug = false;
bool in_background = false;

#define BUF_SIZE 65536

int count_lines(FILE* file)
{
    char buf[BUF_SIZE];
    int counter = 0;
    for(;;)
    {
        size_t res = fread(buf, 1, BUF_SIZE, file);
        if (ferror(file))
            return -1;

        int i;
        for(i = 0; i < res; i++)
            if (buf[i] == '\n')
                counter++;

        if (feof(file))
            break;
    }

    return counter;
}

int process_initialize(char *filename, int pid){
    //copy files to backing store
    copy_to_backing_store(filename);
    char buffer[100];
    sprintf(buffer, "%s/%s", "./backing_store", filename);
    //load from backing store to frame store
    FILE *fp;

    fp = fopen(buffer, "r");
    int file_length = count_lines(fp);
    rewind(fp);
    
    int page_size_by_file_length = file_length / 3 + 1;

    if(fp == NULL){
		return FILE_DOES_NOT_EXIST;
    }
    //load page to shell memory
    PAGE** page_table = malloc(sizeof(PAGE*) * page_size_by_file_length);
    page_table_init(page_table, page_size_by_file_length);
    PCB* newPCB = makePCB(page_table, buffer);

    load_pages_to_memory(fp, pid, page_table, newPCB);
    QueueNode *node = malloc(sizeof(QueueNode));
    node->pcb = newPCB;
    ready_queue_add_to_tail(node);
    fclose(fp);

    return 0;
}




// int shell_process_initialize(){
//     //Note that "You can assume that the # option will only be used in batch mode."
//     //So we know that the input is a file, we can directly load the file into ram
//     int* start = (int*)malloc(sizeof(int));
//     int* end = (int*)malloc(sizeof(int));
//     int error_code = 0;
//     error_code = load_file(stdin, start, end, "_SHELL");
//     if(error_code != 0){
//         return error_code;
//     }
//     PCB* newPCB = makePCB(*start,*end);
//     newPCB->priority = true;
//     QueueNode *node = malloc(sizeof(QueueNode));
//     node->pcb = newPCB;

//     ready_queue_add_to_head(node);

//     freopen("/dev/tty", "r", stdin);
//     return 0;
// }

bool execute_process(QueueNode *node, int quanta){
    char* line = NULL;
    PCB *pcb = node->pcb;
    
    for(int i=0; i<quanta; i++){
        PAGE** page_table = pcb->page_table;
        //get current line
        PAGE* cur_page = page_table[pcb->PC[0]];
        if(cur_page == NULL){
            load_missing_page_to_mem(pcb);
            in_background = false; //? not sure what does in_background do
            return false;
        }

        //if executing a new page add it to the LRU queue
        if(pcb->PC[1] == 0){
            LruQueueNode *LRUnode = malloc(sizeof(LruQueueNode));
            LRUnode->page = cur_page;
            LRU_queue_add_to_tail(LRUnode);
        }

        int cur_line_index = cur_page->index[pcb->PC[1]];
        line = mem_get_value_at_line(cur_line_index);
        in_background = true;
        if(pcb->priority) {
            pcb->priority = false;
        }
        // this means that the process has reached the last line 
        // and it is about to terminate
        if(cur_line_index == pcb->last_line_number){
            parseInput(line);
            terminate_process(node);
            in_background = false;
            return true;
        }
        parseInput(line);
        in_background = false;
        //update pc
        updatePC(pcb);
    }
    return false;
}

void *scheduler_FCFS(){
    QueueNode *cur;
    while(true){
        if(is_ready_empty()) {
            if(active) continue;
            else break;   
        }
        cur = ready_queue_pop_head();
        if(!execute_process(cur, MAX_INT)) {
            ready_queue_add_to_head(cur);
        }   
    }
    return 0;
}

void *scheduler_SJF(){
    QueueNode *cur;
    while(true){
        if(is_ready_empty()) {
            if(active) continue;
            else break;
        }
        cur = ready_queue_pop_shortest_job();
        execute_process(cur, MAX_INT);
    }
    return 0;
}

void *scheduler_AGING_alternative(){
    QueueNode *cur;
    while(true){
        if(is_ready_empty()) {
            if(active) continue;
            else break;
        }
        cur = ready_queue_pop_shortest_job();
        ready_queue_decrement_job_length_score();
        if(!execute_process(cur, 1)) {
            ready_queue_add_to_head(cur);
        }   
    }
    return 0;
}

void *scheduler_AGING(){
    QueueNode *cur;
    int shortest;
    sort_ready_queue();
    while(true){
        if(is_ready_empty()) {
            if(active) continue;
            else break;
        }
        cur = ready_queue_pop_head();
        shortest = ready_queue_get_shortest_job_score();
        if(shortest < cur->pcb->job_length_score){
            ready_queue_promote(shortest);
            ready_queue_add_to_tail(cur);
            cur = ready_queue_pop_head();
        }
        ready_queue_decrement_job_length_score();
        if(!execute_process(cur, 1)) {
            ready_queue_add_to_head(cur);
        }
    }
    return 0;
}

void *scheduler_RR(void *arg){
    int quanta = ((int *) arg)[0];
    QueueNode *cur;
    while(true){
        if(is_ready_empty()){
            if(active) continue;
            else break;
        }
        cur = ready_queue_pop_head();
        if(!execute_process(cur, quanta)) {
            ready_queue_add_to_tail(cur);
        }
    }
    return 0;
}

int schedule_by_policy(char* policy){ //, bool mt){
    if(strcmp(policy, "FCFS")!=0 && strcmp(policy, "SJF")!=0 && 
        strcmp(policy, "RR")!=0 && strcmp(policy, "AGING")!=0 && strcmp(policy, "RR30")!=0){
            return SCHEDULING_ERROR;
    }
    if(active) return 0;
    if(in_background) return 0;
    int arg[1];
    if(strcmp("FCFS",policy)==0){
        scheduler_FCFS();
    }else if(strcmp("SJF",policy)==0){
        scheduler_SJF();
    }else if(strcmp("RR",policy)==0){
        arg[0] = 2;
        scheduler_RR((void *) arg);
    }else if(strcmp("AGING",policy)==0){
        scheduler_AGING();
    }else if(strcmp("RR30", policy)==0){
        arg[0] = 30;
        scheduler_RR((void *) arg);
    }
    return 0;
}

void load_pages_to_memory(FILE *fp, int pid, PAGE** page_table, PCB* pcb){
    int commandLength = 100;
    char command[commandLength];
    int index[3];
    int lineCount = 0;
    int page_index = 0;
    int line_index_in_page = 0;
    PAGE* page;
    int line_location = 0;
    //load file line by line
    while(!feof(fp)) {
        page_index = lineCount / 3;
        if(page_index == 2) {
            break;
        }

        line_index_in_page = lineCount % 3;

        if (line_index_in_page == 0){
            page = makePAGE(page_index, pid);
            page_table[page_index] = page;
        }

        //find a space in frame store and keep a record of the index
        fgets(command, commandLength, fp);
        // line_location is the location (index) of the line
        // that we loaded into the frame store 
        // it is the index of the frame store. 
        line_location = allocate_frame(command, pcb);
        set_page_index(page, line_index_in_page, line_location);
        set_page_valid_bits(page, line_index_in_page, 1);
        
        lineCount++;
    }

    if (feof(fp)){    
        set_pcb_last_line_index(pcb, line_location);
        set_pcb_last_page_index(pcb, page_index);
    }
    set_pcb_line_executed(pcb, lineCount);
    //if current page is not fully occupied
    //we put some place holders. 
    while (line_index_in_page < 2){
        line_index_in_page++;
        line_location = allocate_frame(command, pcb);
        set_page_index(page, line_index_in_page, line_location);
        set_page_index(page,line_index_in_page, -1);
        set_page_valid_bits(page, line_index_in_page, 0);
    }
    
    return;
}

void load_missing_page_to_mem(PCB* pcb){
    int commandLength = 100;
    char command[commandLength];
    int lineCount = pcb->line_loaded;
    int page_index = lineCount / 3;
    int line_index_in_page = 0;
    PAGE* page;
    int line_location = 0;
    int counter=0;
    FILE * fp = fopen(pcb->filename, "r");

    for(int i=0; i < lineCount;i++){
        fgets(command, commandLength, fp);
    }

    while(!feof(fp) && counter < 3) {
        line_index_in_page = lineCount % 3;
        if (line_index_in_page == 0){
            page = makePAGE(page_index, pcb->pid);
            pcb->page_table[page_index] = page;
        }
        //find a space in frame store and keep a record of the index
        fgets(command, commandLength, fp);
        line_location = allocate_frame(command, pcb);
        set_page_index(page, line_index_in_page, line_location);
        set_page_valid_bits(page, line_index_in_page, 1);
        lineCount++;
        counter++;
    }
    if (feof(fp)){    
        set_pcb_last_line_index(pcb, line_location);
        set_pcb_last_page_index(pcb, page_index);
    }

    set_pcb_line_executed(pcb, lineCount);
    //if current page is not fully occupied, fill it up
    while (line_index_in_page < 2){
        line_index_in_page++;
        line_location = allocate_frame("", pcb);
        set_page_index(page, line_index_in_page, line_location);
        set_page_valid_bits(page, line_index_in_page, 0);
    }
    fclose(fp);
    return;
}


