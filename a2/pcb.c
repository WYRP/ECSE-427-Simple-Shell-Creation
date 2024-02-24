#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "pcb.h"

int pid_counter = 1;
const int FRAME_STORE_SIZE = 2;


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
}