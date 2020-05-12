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

        else if(strcmp(arguments[0],"bmap\0")==0){
            // printf("Control has reached the get bmap count function\n");
            getBitMap();
        }

        else if(strcmp(arguments[0],"ls\0")==0){
            // printf("Control has reached the free buffers function\n");
            ls();
        }

        else if(strcmp(arguments[0],"mkdir\0")==0){
            if(arguments[1]==NULL){
                printf("You need to enter a name for the new directory please try again with mkdir <directoryName>\n");
                free(inputLine);
                continue;
            }
            printf("Writing directory with the name : %s\n",arguments[1]);
            mkdir(arguments[1]);
        }

        else if(strcmp(arguments[0],"read\0")==0){
            if(arguments[1]==NULL || arguments[2]==NULL){
                printf("You need to enter a name for the file and number of bytes to read please try again with read <fileName> <noOfBytes>\n");
                free(inputLine);
                continue;
            }
            uint64_t noOfBytes= S64(arguments[2]);

            fsReadFile(arguments[1],noOfBytes);
        }

        else if(strcmp(arguments[0],"rmdir\0")==0){
            if(arguments[1]==NULL){
                printf("You need to enter a name for the directory to remove please try again with rmdir <directoryName>\n");
                free(inputLine);
                continue;
            }
            printf("Removing directory with the name : %s\n",arguments[1]);
            rmdir(arguments[1]);
        }

        //Remove file function call
        else if(strcmp(arguments[0],"rm\0")==0){
            if(arguments[1]==NULL){
                printf("You need to enter a name for the file to be removed please try again with rmdir <directoryName>\n");
                free(inputLine);
                continue;
            }
            printf("Removing file with the name : %s\n",arguments[1]);
            rm(arguments[1]);
        }



        else if(strcmp(arguments[0],"pwd\0")==0){
            // printf("Control has reached the free buffers function\n");
            pwd();
        }

        else if(strcmp(arguments[0],"cpfl\0")==0){
            // printf("Control has reached the free buffers function\n");
            if(arguments[1]==NULL || arguments[2]==NULL){
                printf("You need to enter the name of file on linux and name of file created on srfs. Type your command in the following format cpfl <linuxFileName> <newFileName>\n");
                free(inputLine);
                continue;  
            }
        
            
            fsCopyFromLinux(arguments[1],arguments[2]);
        }

        else if(strcmp(arguments[0],"move\0")==0){
            // printf("Control has reached the free buffers function\n");
            if(arguments[1]==NULL || arguments[2]==NULL){
                printf("You need to enter the name of file to move and the directory to move the file to. Type your command in the following format move <fileName> <dirName>\n");
                free(inputLine);
                continue;  
            }
            fsMove(arguments[1],arguments[2]);
        }

        else if(strcmp(arguments[0],"rename\0")==0){
            // printf("Control has reached the free buffers function\n");
            if(arguments[1]==NULL || arguments[2]==NULL){
                printf("You need to enter the name of file to rename and the new name for the file. Type your command in the following format rename <fileName> <newFileName>\n");
                free(inputLine);
                continue;  
            }
            fsRenameFile(arguments[1],arguments[2]);
        }


        else if(strcmp(arguments[0],"copy\0")==0){
            // printf("Control has reached the free buffers function\n");
            if(arguments[1]==NULL || arguments[2]==NULL){
                printf("You need to enter the name of file to and new name of copied file. Optionally you can also specify a directry to copy to as a third argument. Type your command in the following format copy <fileName> <newFileName> <dirName>(optional)\n");
                free(inputLine);
                continue;  
            }
            if(arguments[3]==NULL){
                fsCopy(arguments[1],arguments[2],NULL);
            }
            else{
                fsCopy(arguments[1],arguments[2],arguments[3]);
            }
        }


       else if(strcmp(arguments[0],"cd\0")==0){
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
        else if(strcmp(arguments[0],"man\0")==0){
            printf("List of commands:\nexit\nvinfo\nbmap\nls\nmkdir\nread\nrmdir\nrm\npwd\ncpfl\nmove\nrename\ncopy\ncd\n");
        }
        else{
            printf("Command not recognized. Type command man for a list of functional commands\n");
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
            printf("EOF on stdin reached.\n");
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
