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
#include <time.h>

bool active = false;
bool debug = false;
bool in_background = false;

int process_initialize(char *filename, int pid){
    //copy files to backing store
    copy_to_backing_store(filename);
    char buffer[100];
    sprintf(buffer, "%s/%s", "./backing_store", filename);
    //load from backing store to frame store
    FILE *fp;
    fp = fopen(buffer, "r");
    
    if(fp == NULL){
		return FILE_DOES_NOT_EXIST;
    }
    //load page to shell memory
    PAGE** page_table = malloc(sizeof(PAGE*) * FRAME_STORE_SIZE);
    page_table_init(page_table);
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
    char *line = NULL;
    PCB *pcb = node->pcb;
    for(int i=0; i<quanta; i++){
        PAGE** page_table = pcb->page_table;
        //get current line
        PAGE* cur_page = page_table[pcb->PC[0]];
    
        // if the next line of code that resides in a page 
        //which is not yet in memory
        // this if statement will be true
        if(cur_page == NULL){
            // then we handle the page fault
 
            //first interrupt the current process (P)
            // by adding it to the tail of the ready queue
            ready_queue_add_to_tail(node);
            //after we added the P to the tail of the ready queue
            //we need to execute the first process from the ready queue
            ready_queue_pop_head();

            //while the P is in the background, we need to load the missing page to memory
            //then we go to the allocate_frame function to load the page to frame store
            //and update the page table
            allocate_frame(pcb->filename, NULL);
            //load page to frame store and update table
            load_missing_page_to_mem(pcb);
            in_background = false; //? not sure what does in_background do
            return false;
        }
        cur_page->last_accessed = time(NULL); // Correctly update the timestamp of the accessed page

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
            //we would like to terminate the process
            // by calling the terminate_process function
            // however, we do not want to clean up the process's corresponding 
            // pages in the frame store
            // something is being done in the terminate_process function
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
        execute_process(cur, MAX_INT);
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
        //some local var setup
        page_index = lineCount / 3;

        //load only 2 pages
        if(page_index == 2) {
            lineCount++;
            break;
        }

        line_index_in_page = lineCount % 3;

        //we want to stop loading after the first two pages are loaded
        if (page_index > 1){
            break;
        }

        //convert pid id to string
        //change to 2 because the the pid is one 
        //and then the null terminator is also 1
        char pid_string[2];
        sprintf(pid_string, "%d", pid);

        //when a new page starts, create a new page
        if (line_index_in_page == 0){
            page = makePAGE(page_index, pid);
            page_table[page_index] = page;
        }

        //find a space in frame store and keep a record of the index
        fgets(command, commandLength, fp);
        line_location = allocate_frame(pid_string, command);

        set_page_index(page, line_index_in_page, line_location);
        set_page_valid_bits(page, line_index_in_page, 1);
        page->last_accessed = time(NULL); // Update to current time

        
        lineCount++;
    }
    set_pcb_last_line_index(pcb, line_location);
    set_pcb_last_page_index(pcb, page_index);
    set_pcb_line_executed(pcb, lineCount);
    //if current page is not fully occupied, fill it up
    while (line_index_in_page < 2){
        line_index_in_page++;
        set_page_index(page,line_index_in_page, -1);
        set_page_valid_bits(page, line_index_in_page, 0);
        page->last_accessed = time(NULL); // Update to current time

    }
    
    return;
}

void load_missing_page_to_mem(PCB* pcb){
    int commandLength = 100;
    char command[commandLength];
    int index[3];
    int lineCount = pcb->line_executed;
    int page_index = lineCount / 3;
    int line_index_in_page = 0;
    PAGE* page;
    int line_location = 0;
    FILE * fp = fopen(pcb->filename, "r");

    for(int i=0; i < 3; i++){
        line_index_in_page = lineCount % 3;
        //convert pid id to string
        // need to be 2 because we want to acount for the null terminator
        char pid_string[2];
        sprintf(pid_string, "%d", pcb->pid);

        //when a new page starts, create a new page
        if (line_index_in_page == 0){
            page = makePAGE(page_index, pcb->pid);
            pcb->page_table[page_index] = page;
        }

        //find a space in frame store and keep a record of the index
        fgets(command, commandLength, fp);
        line_location = allocate_frame(pid_string, command);
        
        set_page_index(page, line_index_in_page, line_location);
        set_page_valid_bits(page, line_index_in_page, 1);
        page->last_accessed = time(NULL); // Update to current time

        
        lineCount++;
    }
    if (feof(fp)){    
        set_pcb_last_line_index(pcb, line_location);
        set_pcb_last_page_index(pcb, page_index);
    }

    set_pcb_line_executed(pcb, lineCount);
    //if current page is not fully occupied, fill it up
    while (line_index_in_page < 2){
        line_index_in_page++;
        set_page_index(page,line_index_in_page, -1);
        set_page_valid_bits(page, line_index_in_page, 0);
        page->last_accessed = time(NULL); // Update to current time

    }
    return;
}



