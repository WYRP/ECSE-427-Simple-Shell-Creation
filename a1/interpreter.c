//Yixuan Qin, 261010963
//Yongru Pan, 261001758


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h> 
//#include <sys/stat.h> // these could be useful?
#include <ctype.h>
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
int my_cp(char *srcFile, char *destFile);



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
    return my_cat(command_args[1]);
	}
	else if (strcmp(command_args[0], "my_cp") == 0) {
    if (args_size != 3) return badcommand();
    return my_cp(command_args[1], command_args[2]);
	}
	else if (strcmp(command_args[0], "if") == 0) {
	if (args_size != 9) return badcommand();
	return ifStatement(command_args);
	}
	else return badcommand();
}

int help(){

	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n";

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

/*Helper function to determine whether the filename is alphanumeric*/
int is_alphanumeric(char *str) {
    while (*str) {
        // If the current character is not alphanumeric
		// return 0
        if (!isalnum((unsigned char)*str)) {
            return 0;
        }
        str++;
    }
    // reached the end without finding 
	// a non-alphanumeric character, return 1
    return 1;
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
	//check the length of the filename
	if (strlen(filename) > 100){
		printf("File name is too long.\n");
		return 1; // Return 1 for error
	}
	//check if the filename is alphanumeric
	if (is_alphanumeric(filename)== 0){
		printf("File name is not alphanumeric.\n");
		return 1; // Return 1 for error
	}
	//create the file
	FILE *file;
	file = fopen(filename, "w");
	fclose(file);
	return 0;
}


int my_cat(char *filename) {

	FILE *file;
	file = fopen(filename, "r");
	if (file == NULL) return badcommandSpecific("my_cat");
	int c = fgetc(file); 
    while (c != EOF) 
    { 
        printf ("%c", c); 
        c = fgetc(file); 
    } 
  
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

/*Optional: The following part is not worth any marks and will not be graded. However, if you pass
the tests for this part, you will be awarded 1,000 COMP310COIN when you submit your code and pass
the corresponding public test. Add support for if conditionals. An if conditional will have the
following syntax:
if identifier op identifier then command1 else command2 fi
The identifier's can be either single alphanumeric tokens (of no more than 100 characters) or
a variable beginning with a $ (in which case you should use the value of the variable stored in
memory, like you did with the echo command). The op can be either == (equals) or != (not equals).
If the condition (identifier op identifier) is true, then command1 should be executed; otherwise,
command2 should be executed.
Commands can be any series of alphanumeric tokens until the else or newline.*/
int ifStatement(char *input){
    char *command_args[100];
    int args_size = 0;
    char *token = strtok(input, " ");
    while (token != NULL){
        command_args[args_size++] = token;
        token = strtok(NULL, " ");
    }

    // Fetch the identifiers and operator
    char *identifier1 = command_args[1];
    char *op = command_args[2];
    char *identifier2 = command_args[3];
    char *then = command_args[4];
    char *command1 = command_args[5];
    char *else_ = command_args[6];
    char *command2 = command_args[7];
    char *fi = command_args[8];

    // Check for correct syntax with "then", "else", and "fi"
    if (strcmp(then, "then") != 0 || strcmp(else_, "else") != 0 || strcmp(fi, "fi") != 0) {
        return badcommandSpecific("ifStatement");
    }

    // Variable substitution (if identifier starts with $, use the variable value instead)
    char value1[101] = {0}, value2[101] = {0};
    if (identifier1[0] == '$') {
        strcpy(value1, echo(identifier1 + 1)); // Assuming this function exists
        identifier1 = value1;
    }
    if (identifier2[0] == '$') {
        strcpy(value2, echo(identifier2 + 1)); // Assuming this function exists
        identifier2 = value2;
    }

    // Perform the comparison and execute the corresponding command
    if ((strcmp(op, "==") == 0 && strcmp(identifier1, identifier2) == 0) ||
        (strcmp(op, "!=") == 0 && strcmp(identifier1, identifier2) != 0)) {
        return interpreter(command1); // Assuming command1 is just one token, otherwise need to parse fully
    } else {
        return interpreter(command2); // Assuming command2 is just one token, otherwise need to parse fully
    }
}






/*
	Question 9: implemet cp with following usage: 
	cp source_fileName destination_fileName
	copies the concent from the source file to the destination file, if destination file
	doesn't exist, create the file in the current directory.
	return bad command if srcfile is null
*/
int my_cp(char *srcFile, char *destFile){
	FILE *src;
	FILE *dest;
	src = fopen(srcFile, "r");
	if (src == NULL) return badcommandSpecific("my_cp");
	dest = fopen(destFile, "w");
	int c = fgetc(src); 
	while (c != EOF) 
    { 
        fprintf (dest, "%c", c); 
        c = fgetc(src); 
    } 
	fclose(src);
	fclose(dest);
	return 0;

}
