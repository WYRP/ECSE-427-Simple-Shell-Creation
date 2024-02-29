#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "pcb.h"
#include "shellmemory.h"

int pid_counter = 1;

int generatePID(){
    return pid_counter++;
}

//In this implementation, Pid is the same as file ID 
PCB* makePCB(PAGE** page_table, char* filename){
    char* file_name = malloc(sizeof(filename));
    file_name = filename;
    PCB* newPCB = malloc(sizeof(PCB));
    newPCB->pid = generatePID();
    newPCB->filename = file_name;
    newPCB->PC[0] = 0;
    newPCB->PC[1] = 0;
    newPCB->priority = false;
    newPCB->page_table = page_table;
    newPCB->last_line_number = -1;
    newPCB->last_page_number = -1;
    return newPCB;
}

PAGE* makePAGE(int page_index,int page_pid){
    PAGE* newPAGE = malloc(sizeof(PAGE));
    newPAGE->page_index = page_index;
    newPAGE->page_pid = page_pid;
    newPAGE->last_accessed = time(NULL); // Set the current time
    return newPAGE;
}

void set_page_index(PAGE* page, int i, int value){
    page->index[i] = value;
}

void set_page_valid_bits(PAGE* page, int i, int value){
    page->valid_bits[i] = value;
}

void set_pcb_last_line_index(PCB* pcb, int last_page_number){
    pcb->last_line_number = last_page_number;
}

void set_pcb_last_page_index(PCB* pcb, int n){
    pcb->last_page_number = n;
}

void updatePC(PCB* pcb){
    //if not yet reach the last page
    if (pcb->PC[1] < 2){
        pcb->PC[1]++;
    }
    else if(pcb->last_page_number == -1 || pcb->PC[0] < pcb->last_page_number){
        pcb->PC[0]++;
        pcb->PC[1] = 0;
    }
}

void page_table_init(PAGE** page_table){
	int i;
    for (i=0; i<FRAME_STORE_SIZE; i++){		
        page_table[i] = NULL;
    }
}

void set_pcb_line_executed(PCB* pcb, int line_executed){
    pcb->line_executed = line_executed;
}