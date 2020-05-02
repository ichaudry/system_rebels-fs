/**
 * fsdriver3.c
 * 
 * 
 * 
 *Commands needed: list directories, create directories, add and remove files, copy files, move files,
 *setting meta data, and two “special commands” one to copy from the normal filesystem to your filesystem and the other
 *from your filesystem to the normal filesystem
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <inttypes.h>
#include "fsHigh.h"

#define ARGUMENTS_BUFFER 64
#define DELIMETER " \t\r\n\a"

//Get input from stdin
char *getInputLine();

//String tokenizer
char **getArguments(char * inputLine);

//Convert char to int64_t
int64_t S64(const char *s);

//Check if file exists
int cfileexists(const char * filename);

int main(int argc, char const *argv[])
{
    //Variables storing data to format/start file system
    char * filename;
    uint64_t volSize;
    uint64_t blockSize;
    

    //Get data to format/start file system
    if (argc == 4) {
        filename = (char *)argv[1];
        volSize= S64(argv[2]);
        blockSize =  S64(argv[3]);
    }
    else{
        printf("Invalid input. Please run with the following format: ./<executable> <fileName> <volSize> <blockSize>\n");
        exit(0);
    }

    //Start the file system and check if formatting needs to be done
    if(cfileexists(filename)==0){
        startFileSystem(filename,&volSize,&blockSize,1);
    }
    else{
        startFileSystem(filename,&volSize,&blockSize,0);
    }

    //Start command line utility to interact with the file system
    while(1){
        printf("\n>>>");

        //Get stdin
        char * inputLine= getInputLine(); 

        //test
        // printf("This is the line inputted: %s\n",inputLine);

        //If no input report error and get another input
        if(*inputLine == '\n') {
            printf("You did not enter any input. Please try again.\n");
            free(inputLine);
            continue;
        }   

        //if user enters the exit command quit the shell
        if(strcasecmp(inputLine, "exit\n")==0){
        printf("Exiting program thank you for using SRFS.\n");
        free(inputLine);
        break;
        }

        //Split the input line into arguments using delimiters 
        char ** arguments= getArguments(inputLine);

        // printf("This is the first argument %s\nThis is the second argument %s\nThis is the third argument %s\n",arguments[0],arguments[1],arguments[2]);

        if(strcmp(arguments[0],"vinfo\0")==0){
            // printf("Control has reached the print vol info function\n");
            vinfo();
        }

        if(strcmp(arguments[0],"bmap\0")==0){
            // printf("Control has reached the get bmap count function\n");
            getBitMap();
        }

        if(strcmp(arguments[0],"ls\0")==0){
            // printf("Control has reached the free buffers function\n");
            ls();
        }

        if(strcmp(arguments[0],"mkdir\0")==0){
            if(arguments[1]==NULL){
                printf("You need to enter a name for the new directory please try again with mkdir <directoryName>\n");
                free(inputLine);
                continue;
            }
            printf("Writing directory with the name : %s\n",arguments[1]);
            mkdir(arguments[1]);
        }

        if(strcmp(arguments[0],"write\0")==0){
            if(arguments[1]==NULL){
                printf("You need to enter a name for the new file please try again with write <fileName>\n");
                free(inputLine);
                continue;
            }
            fsWriteFile(arguments[1]); 
        }

        if(strcmp(arguments[0],"rmdir\0")==0){
            if(arguments[1]==NULL){
                printf("You need to enter a name for the directory to remove please try again with rmdir <directoryName>\n");
                free(inputLine);
                continue;
            }
            printf("Removing directory with the name : %s\n",arguments[1]);
            rmdir(arguments[1]);
        }

        //Remove file function call
        if(strcmp(arguments[0],"rm\0")==0){
            if(arguments[1]==NULL){
                printf("You need to enter a name for the file to be removed please try again with rmdir <directoryName>\n");
                free(inputLine);
                continue;
            }
            printf("Removing file with the name : %s\n",arguments[1]);
            rm(arguments[1]);
        }



        if(strcmp(arguments[0],"pwd\0")==0){
            // printf("Control has reached the free buffers function\n");
            pwd();
        }

        if(strcmp(arguments[0],"cpfl\0")==0){
            // printf("Control has reached the free buffers function\n");
            fsCopyFromLinux();
        }

        if(strcmp(arguments[0],"cd\0")==0){
            // printf("Control has reached the change directory function\n");
            if(arguments[1]==NULL){
                printf("You need to enter a name of the directory to change to, please try again with cd <directoryName>\nNote: To change to parent directory type cd ..");
                free(inputLine);
                continue;
            }
            // printf("The second argument to the change directory function is %s\n", arguments[1]);
            if(strcmp(arguments[1],"..\0")==0){
                cdRoot();
            }
            else{
                cd(arguments[1]);
            }
        }

        // printf("Clearing the driver buffers.\n");
        free(inputLine);
        free(arguments);
    }


    return 0;
}


//Converts string to uint64 type
//Source: https://stackoverflow.com/a/17003732
int64_t S64(const char *s) {
  int64_t i;
  char c ;
  int scanned = sscanf(s, "%" SCNd64 "%c", &i, &c);
  if (scanned == 1) return i;
  if (scanned > 1) {
    // TBD about extra data found
    return i;
    }
  // TBD failed to scan;  
  return 0;  
}


//Checks if file already exists
//Source: https://www.zentut.com/c-tutorial/c-file-exists/
int cfileexists(const char * filename){
    /* try to open file to read */
    FILE *file;
    if ((file = fopen(filename, "r"))){
        fclose(file);
        return 1;
    }
    return 0;
}


//Gets STDIN
char *getInputLine(){
    char *inputLine = NULL;
    size_t bufsize = 0; // have getline allocate a buffer for us

    if (getline(&inputLine, &bufsize, stdin) == -1){
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);  // We recieved an EOF
        } else {
            perror("readline");
            exit(EXIT_FAILURE);
        }
    }


    return inputLine;

}


char **getArguments(char * inputLine)
{
    int index = 0;

    //Allocate arguments array on the heap
    char **arguments = malloc(ARGUMENTS_BUFFER* sizeof(char*));

    //Pointer to the tokens of inputline used for strtok splitting function
    char *token;

    if (!arguments) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(inputLine, DELIMETER);
    while (token != NULL) {
        arguments[index] = token;
        // printf("this the token %s\n",token);
        index++;
        token = strtok(NULL, DELIMETER);
    }

    //To ensure that the array is null terminated
    arguments[index] = NULL;

    return arguments;
}
