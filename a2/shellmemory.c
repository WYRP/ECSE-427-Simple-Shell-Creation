// Yixuan Qin, 261010963
// Yongru Pan, 261001758 
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<stdbool.h>
#include<unistd.h>
#include "interpreter.h"
#include "shellmemory.h"
#include "pcb.h"


//where the FRAME STORE and the VARIABLE store are;
//we would need a hard line to seperate the two
//EG

const int THRESHOLD = FRAME_STORE_SIZE;
LruQueueNode* LRU_head = NULL;

//frame store and variable store
struct memory_struct shellmemory[FRAME_STORE_SIZE + VARIABLE_STORE_SIZE];
// int LRU_page_number = 0;

// Helper functions
int match(char *model, char *var) {
	int i, len=strlen(var), matchCount=0;
	for(i=0;i<len;i++)
		if (*(model+i) == *(var+i)) matchCount++;
	if (matchCount == len)
		return 1;
	else
		return 0;
}


char *extract(char *model) {
	char token='=';    // look for this to find value
	char value[1000];  // stores the extract value
	int i,j, len=strlen(model);
	for(i=0;i<len && *(model+i)!=token;i++); // loop till we get there
	// extract the value
	for(i=i+1,j=0;i<len;i++,j++) value[j]=*(model+i);
	value[j]='\0';
	return strdup(value);
}


// Shell memory functions

void mem_init(){
	int i;
	for (i=0; i<FRAME_STORE_SIZE + VARIABLE_STORE_SIZE; i++){		
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}
}

void clear_variable_store(){
	int i;
	for (i=THRESHOLD; i<FRAME_STORE_SIZE + VARIABLE_STORE_SIZE; i++){		
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}
}

// Set key value pair in the variable store
void mem_set_value(char *var_in, char *value_in) {
	int i;
	for (i=THRESHOLD; i< FRAME_STORE_SIZE + VARIABLE_STORE_SIZE; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}
	
	//Value does not exist, need to find a free spot.
	for (i=THRESHOLD; i< FRAME_STORE_SIZE + VARIABLE_STORE_SIZE; i++){
		if (strcmp(shellmemory[i].var, "none") == 0){
			shellmemory[i].var = strdup(var_in);
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	return;

}
void LRU_queue_add_to_tail(LruQueueNode* node)
{
    if(!LRU_head){
        LRU_head = node;
        LRU_head->next = NULL;
    }
    else{
        LruQueueNode* cur = LRU_head;
        while(cur->next!=NULL) cur = cur->next;
        cur->next = node;
        cur->next->next = NULL;
    }
}

LruQueueNode *LRU_queue_pop_head(){
    LruQueueNode *tmp = LRU_head;
    if(LRU_head!=NULL) LRU_head = LRU_head->next;
    return tmp;
}

int allocate_frame(char *value_in, PCB* pcb){
	char pid_string[1];
	sprintf(pid_string, "%d", pcb->pid);
	for (int i=0; i<THRESHOLD; i++){
		if (strcmp(shellmemory[i].var, "none") == 0){
			shellmemory[i].var = strdup(pid_string);
			shellmemory[i].value = strdup(value_in);
			return i;
		} 
	}

	LruQueueNode* victim_page_node = LRU_queue_pop_head();
	PAGE* victim_page = victim_page_node->page;
	printf("Page fault! Victim page contents:\n");
	for (int i=0; i < 3; i++){
		printf("%s", shellmemory[victim_page->index[i]].value);
		mem_free_line(victim_page->index[i]);
	}
	printf("End of victim page contents.\n");
	int new_index = victim_page->index[0];
	//allocate new line to this free spot (index 0 of the victim page)
	shellmemory[new_index].var = strdup(pid_string);
	shellmemory[new_index].value = strdup(value_in);
	// printShellMemory();
	return new_index;
}


//get value based on input key
char *mem_get_value(char *var_in) {
	int i;
	for (i=THRESHOLD; i<FRAME_STORE_SIZE + VARIABLE_STORE_SIZE; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			return strdup(shellmemory[i].value);
		} 
	}
	return NULL;

}


void printShellMemory(){
	int count_empty = 0;
	for (int i = 0; i < FRAME_STORE_SIZE + VARIABLE_STORE_SIZE; i++){
		if(strcmp(shellmemory[i].var,"none") == 0){
			count_empty++;
		}
		else{
			printf("\nline %d: key: %s\t\tvalue: %s\n", i, shellmemory[i].var, shellmemory[i].value);
		}
    }
	printf("\n\t%d lines in total, %d lines in use, %d lines free\n\n", FRAME_STORE_SIZE + VARIABLE_STORE_SIZE, FRAME_STORE_SIZE + VARIABLE_STORE_SIZE-count_empty, count_empty);
}

void copy_to_backing_store(char* filename){
	char command[100];
	sprintf(command, "cp %s ./backing_store", filename);
	system(command);
}

char * mem_get_value_at_line(int index){
	if(index<0 || index > FRAME_STORE_SIZE + VARIABLE_STORE_SIZE) return NULL; 
	return shellmemory[index].value;
}

void mem_free_lines_between(int start, int end){
	for (int i=start; i<=end && i<FRAME_STORE_SIZE + VARIABLE_STORE_SIZE; i++){
		mem_free_line(i);
	}
}

void mem_free_line(int i){
	if(shellmemory[i].var != NULL){
		free(shellmemory[i].var);
	}	
	if(shellmemory[i].value != NULL){
		free(shellmemory[i].value);
	}	
	shellmemory[i].var = "none";
	shellmemory[i].value = "none";
}