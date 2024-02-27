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
PCB* makePCB(int start, int end){
    PCB* newPCB = malloc(sizeof(PCB));
    PAGE** page_table = malloc(sizeof(PAGE*) * FRAME_STORE_SIZE);
    newPCB->pid = generatePID();
    newPCB->PC = start;
    newPCB->start  = start;
    newPCB->end = end;
    newPCB->job_length_score = 1+end-start;
    newPCB->priority = false;
    newPCB->number_of_pages = FRAME_STORE_SIZE;
    newPCB->page_table = page_table;
    return newPCB;

    //when a new process comes in, we need to create a page table for it
    //it goes through each frame in the frame store and prepares them for the new process
    //each page in the page table has 3 index and 3 valid bits
    for (int i = 0; i < FRAME_STORE_SIZE; i++){
        //in each slots in the page table, we need to create a page
        //this is where the information of the frame is stored
        newPCB->page_table[i] = malloc(sizeof(PAGE));
        // this is like labelling the page
        newPCB->page_table[i]->page_index = i;
        // this is like marking the page with the pid of the process
        newPCB->page_table[i]->page_pid = newPCB->pid;
        // inside each page, we need to store information up to 3 index
        // and 3 valid bits
        for (int j = 0; j < 3; j++){
            // but since we assume the new process has not brought in any page yet
            // we mark the index as empty
            newPCB->page_table[i]->index[j] = -1;
            // and the valid bits as 0, which is invalid
            newPCB->page_table[i]->valid_bits[j] = 0;
        }
    }
}