//Yixuan Qin, 
//Yongru Pan, 261001758


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h> 
//#include <sys/stat.h> // these could be useful?
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "shellmemory.h"
#include "shell.h"

int MAX_ARGS_SIZE = 20;

int badcommand(){
	printf("%s\n", "Unknown Command");
	return 1;
}
int badcommandSpecific(char* command){
	printf("Bad command: %s\n", command);
	return 1;
}

// For run command only
int badcommandFileDoesNotExist(){
	printf("%s\n", "Bad command: File not found");
	return 3;
}

int help();
int quit();
int set(char* arguments[], int numberOfArguments);
int print(char* var);
int run(char* script);
int badcommandFileDoesNotExist();
int my_ls();
int my_touch(char* filename);
int my_cat(char* filename);
int echo(char* var);
int my_mkdir(char* dir);
int my_cd(char* dirname);
int mkdir(const char *pathname, mode_t mode);
int chdir(const char *path);



// Interpret commands and their arguments
int interpreter(char* command_args[], int args_size){
	int i;

	if ( args_size < 1 || args_size > MAX_ARGS_SIZE){
		return badcommand();
	}

	for ( i=0; i<args_size; i++){ //strip spaces new line etc
		command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	}

	if (strcmp(command_args[0], "help")==0){
	    //help
	    if (args_size != 1) return badcommand();
	    return help();
	
	} else if (strcmp(command_args[0], "quit")==0) {
		//quit
		if (args_size != 1) return badcommand();
		return quit();

	} else if (strcmp(command_args[0], "set")==0) {
		//set
		if (args_size < 3 || args_size > 7) {
			return badcommandSpecific("set");
		}
		return set(command_args, args_size);
	
	} else if (strcmp(command_args[0], "print")==0) {
		if (args_size != 2) return badcommand();
		return print(command_args[1]);
	
	} else if (strcmp(command_args[0], "run")==0) {
		if (args_size != 2) return badcommand();
		return run(command_args[1]);
	
	} else if (strcmp(command_args[0], "my_ls")==0){
		if (args_size != 1) return badcommand();
		return my_ls();
	} else if (strcmp(command_args[0], "my_touch") == 0){
		if (args_size != 2) return badcommand();
		return my_touch(command_args[1]);
	}
	else if (strcmp(command_args[0], "echo") == 0) {
    if (args_size != 2) return badcommand();
    return echo(command_args[1]);
	}
	else if (strcmp(command_args[0], "my_mkdir") == 0) {
    if (args_size != 2) return badcommand();
    return my_mkdir(command_args[1]);
	}
	else if (strcmp(command_args[0], "my_cd") == 0) {
    if (args_size != 2) return badcommand();
    return my_cd(command_args[1]);
	}
	else if (strcmp(command_args[0], "my_cat") == 0) {
    if (args_size != 2) return badcommand();
    return my_cd(command_args[1]);
	}
	else return badcommand();
}

int help(){

	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n ";
	printf("%s\n", help_string);
	return 0;
}

int quit(){
	printf("%s\n", "Bye!");
	exit(0);
}

/*The set command helps linking a char and a value together in a java-hashmap-like way*/
int set(char* arguments[], int numberOfArguments){
	char* var = arguments[1];
	char value[504];
	strcpy(value, arguments[2]);
	for (int i = 3; i < numberOfArguments; i++){
		strcat(value, " ");
		strcat(value, arguments[i]);
	}
	mem_set_value(var, value);

	return 0;
}

/*echo is a shell command that prints its arguments
echo $var prints the value of the variable var*/
int echo(char* var){
	if (var[0] == '$'){
		//search for the corresponding value in the memory
		char* val = mem_get_value(var+1);
		if (val == NULL){
			printf("%s\n", "Variable Does Not Exist");
		}
		else {
			printf("%s\n", val);
		}
	}
	else{
		//print the string
		printf("%s\n", var);
	}

	return 0;
}

/*list all the files presented in the current directory*/
int my_ls(){
	char *command = "ls";
	system(command);
	return 0;
}

/*create a new directory called dirname in the
current directory.*/
int my_mkdir(char* dir){
	if (strlen(dir) > 100){
        printf("Directory name is too long.\n");
        return 1; // Return 1 for error
    }
	//int status = mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	int status = mkdir(dir, 0777);
	if (status == -1){
		printf("%s\n", "Directory already exists");
		return 1;
	}
	return 0; //
}

/* change the current 
directory to the specified directory.*/
int my_cd(char* dirname) {
    if(chdir(dirname) != 0) {
        return badcommandSpecific("my_cd");
    }
    return 0;
}

/*creats a new empty file inside the current directory*/
int my_touch(char *filename){
	printf("Filename: '%s'\n", filename);
	printf("Length: %lu\n", strlen(filename));
	//check the length of the filename
	if (strlen(filename) > 100){
		printf("File name is too long.\n");
		return 1; // Return 1 for error
	}
	FILE *file;
	file = fopen(filename, "w");
	fclose(file);
	return 0;
}


int print(char* var){
	printf("%s\n", mem_get_value(var)); 
	return 0;
}

int run(char* script){
	int errCode = 0;
	char line[1000];
	FILE *p = fopen(script,"rt");  // the program is in a file

	if(p == NULL){
		return badcommandFileDoesNotExist();
	}

	fgets(line,999,p);
	while(1){
		errCode = parseInput(line);	// which calls interpreter()
		memset(line, 0, sizeof(line));

		if(feof(p)){
			break;
		}
		fgets(line,999,p);
	}

    fclose(p);

	return errCode;
}



int my_cat(char *filename) {
	FILE *file;
	file = fopen(filename, "r");
	if (file == NULL) return badcommandSpecific("my_cat");
	char c = fgetc(file); 
    while (c != EOF) 
    { 
        printf ("%c", c); 
        c = fgetc(file); 
    } 
  
	fclose(file);
	return 0;
}
