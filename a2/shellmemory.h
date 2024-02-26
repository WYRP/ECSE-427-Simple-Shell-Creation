#ifndef SHELLMEMORY_H
#define SHELLMEMORY_H
static const int FRAME_STORE_SIZE = 2;
void mem_init();
char *mem_get_value(char *var);
void mem_set_value(char *var_in, char *value_in);
void allocate_frame(char* command);
int load_file(FILE* fp, int* pStart, int* pEnd, char* fileID);
char* copy_to_backing_store(char* script);
char* mem_get_value_at_line(int index);
void mem_free_lines_between(int start, int end);
void printShellMemory();
void clear_variable_store();
#endif