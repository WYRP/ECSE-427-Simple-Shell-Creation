#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<stdbool.h>
#include<unistd.h>
#include<time.h>
#include "interpreter.h"
#include "shellmemory.h"
#include "pcb.h"

#define SHELL_MEM_LENGTH 1000


//where the FRAME STORE and the VARIABLE store are;
//we would need a hard line to seperate the two
//EG

const int THRESHOLD = FRAME_STORE_SIZE;
LruQueueNode* LRU_head = NULL;

//frame store and variable store
struct memory_struct shellmemory[SHELL_MEM_LENGTH];
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
	for (i=0; i<1000; i++){		
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}
}

void clear_variable_store(){
	int i;
	for (i=THRESHOLD; i<1000; i++){		
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}
}

// Set key value pair in the variable store
void mem_set_value(char *var_in, char *value_in) {
	int i;
	for (i=THRESHOLD; i<1000; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	//Value does not exist, need to find a free spot.
	for (i=THRESHOLD; i<1000; i++){
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

int allocate_frame(char *var_in, char *value_in, PCB* pcb){
	for (int i=0; i<THRESHOLD; i++){
		if (strcmp(shellmemory[i].var, "none") == 0){
			shellmemory[i].var = strdup(var_in);
			shellmemory[i].value = strdup(value_in);
			return i;
		} 
	}
	//remove page at pcb->LRU_page_number
	// PAGE* victim_page = pcb->page_table[pcb->LRU_page_number++];
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
	shellmemory[new_index].var = strdup(var_in);
	shellmemory[new_index].value = strdup(value_in);
	return new_index;
}


//get value based on input key
char *mem_get_value(char *var_in) {
	int i;
	for (i=0; i<1000; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			return strdup(shellmemory[i].value);
		} 
	}
	return NULL;

}


void printShellMemory(){
	int count_empty = 0;
	for (int i = 0; i < SHELL_MEM_LENGTH; i++){
		if(strcmp(shellmemory[i].var,"none") == 0){
			count_empty++;
		}
		else{
			printf("\nline %d: key: %s\t\tvalue: %s\n", i, shellmemory[i].var, shellmemory[i].value);
		}
    }
	printf("\n\t%d lines in total, %d lines in use, %d lines free\n\n", SHELL_MEM_LENGTH, SHELL_MEM_LENGTH-count_empty, count_empty);
}


/*
 * Function:  addFileToMem 
 * 	Added in A2
 * --------------------
 * Load the source code of the file fp into the shell memory:
 * 		Loading format - var stores fileID, value stores a line
 *		Note that the first 100 lines are for set command, the rests are for run and exec command
 *
 *  pStart: This function will store the first line of the loaded file 
 * 			in shell memory in here
 *	pEnd: This function will store the last line of the loaded file 
 			in shell memory in here
 *  fileID: Input that need to provide when calling the function, 
 			stores the ID of the file
 * 
 * returns: error code, 21: no space left
 */
int load_file(FILE* fp, int* pStart, int* pEnd, char* filename)
{
	char *line;
    size_t i;
    int error_code = 0;
	bool hasSpaceLeft = false;
	bool flag = true;
	i=101;
	size_t candidate;
	while(flag){
		flag = false;
		for (i; i < SHELL_MEM_LENGTH; i++){
			if(strcmp(shellmemory[i].var,"none") == 0){
				*pStart = (int)i;
				hasSpaceLeft = true;
				break;
			}
		}
		candidate = i;
		for(i; i < SHELL_MEM_LENGTH; i++){
			if(strcmp(shellmemory[i].var,"none") != 0){
				flag = true;
				break;
			}
		}
	}
	i = candidate;
	//shell memory is full
	if(hasSpaceLeft == 0){
		error_code = 21;
		return error_code;
	}
    
    for (size_t j = i; j < SHELL_MEM_LENGTH; j++){
        if(feof(fp))
        {
            *pEnd = (int)j-1;
            break;
        }else{
			line = calloc(1, SHELL_MEM_LENGTH);
			if (fgets(line, SHELL_MEM_LENGTH, fp) == NULL)
			{
				continue;
			}
			shellmemory[j].var = strdup(filename);
            shellmemory[j].value = strndup(line, strlen(line));
			free(line);
        }
    }

	//no space left to load the entire file into shell memory
	if(!feof(fp)){
		error_code = 21;
		//clean up the file in memory
		for(int j = 1; i <= SHELL_MEM_LENGTH; i ++){
			shellmemory[j].var = "none";
			shellmemory[j].value = "none";
    	}
		return error_code;
	}
	//printShellMemory();
    return error_code;
}

void copy_to_backing_store(char* filename){
	char command[100];
	sprintf(command, "cp %s ./backing_store", filename);
	system(command);
}

char * mem_get_value_at_line(int index){
	if(index<0 || index > SHELL_MEM_LENGTH) return NULL; 
	return shellmemory[index].value;
}

void mem_free_lines_between(int start, int end){
	for (int i=start; i<=end && i<SHELL_MEM_LENGTH; i++){
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

