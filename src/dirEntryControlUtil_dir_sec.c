//
// Created by ibraheem on 4/20/20.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include "fsLow.h"
#include "bitMapUtil.h"
#include "fileSystem.h"
#include "fsHigh.h"
#include "dirEntryControlUtil.h"


/**
 * Check if a file with name already exists
 * @param fileName
 * @return
 */
int duplicateChecker(char * fileName){
    uint64_t * metaData=currentDirectory->filesMeta;

    //Check if file exists in current directory
    for(int i=0;i<32;i++){
        uint64_t memoryLocation=metaData[i];
        if(memoryLocation==0){
            continue;
        }
        Dir_Entry * tempDir= malloc(blckSize);
        LBAread(tempDir,1,memoryLocation);

        //check if the names are a match
        if(strcmp(tempDir->fileName,fileName)==0){
            printf("A file with this name already exists please choose a new name.\n");
            return 1;
        }
        free(tempDir);
    }
    //No duplicates
    return 0;
}


Dir_Entry * findDirectory(char * dirName){
    
    uint64_t * metaData=currentDirectory->filesMeta;


    //Find directory to remove in current directory
    for(int i=0;i<32;i++){
        uint64_t memoryLocation=metaData[i];
        if(memoryLocation==0){
            continue;
        }

        //Allocate a directory entry to read into 
        Dir_Entry * tempDir= malloc(blckSize);
        LBAread(tempDir,1,memoryLocation);

        //check if the names are a match
        if(strcmp(tempDir->fileName,dirName)==0){
            //Check if its a directory
            if(tempDir->typeOfFile==0){
            printf("%s is a file and not a directory. Please enter a valid directory name.\n",dirName);
            free(tempDir);
            return NULL;
            }
            //return a pointer to file directory entry
            return tempDir;
        }
        //Free tempDir at the end of for loop
        free(tempDir);
    }

    printf("No file with that name was found. Please check the name you entered and try again.\n");
    return NULL;
}



/**
 * Print current working directory information
 * @return
 */
void * printCurrentDirectory(){
    printf("This is what is read from the current directory\nThe directory name is: %s\n",currentDirectory->fileName);
    printf("The type of file is: ");
    printf("%"PRIu64"\n",currentDirectory->typeOfFile);
    printf("The size of the directory is: %lu\n", currentDirectory->directorySize);
    printf("The meta data contained by file is: \n");
    printMeta(currentDirectory->filesMeta);
    printf("The parent directory address is: %lu\n",currentDirectory->parentDirectory);
    printf("The memory location of file is: %lu\n", currentDirectory->memoryLocation);
}


/**
 * Print directory information
 * @param directory
 * @return
 */
void * printDirectory(Dir_Entry * directory){  
        printf("This is what is read from the current directory\n The directory name is: %s\n",directory->fileName);
        printf("The type of file is: ");
        printf("%"PRIu64"\n",directory->typeOfFile);
        printf("The size of the directory is: %lu\n", directory->directorySize);
        printf("The meta data contained by file is: \n");
        // printMeta();
        printf("The memory location of file is: %lu\n", directory->memoryLocation);

}

/**
 * Print file metadata
 * @param metaData
 * @return
 */
void * printMeta(uint64_t * metaData){
    for(int i=0;i<32;i++){
        printf("The index %d holds: %lu\n",i,metaData[i]);
    }
}

/**
 * Print information of current volume
 * @param vInfo
 * @return
 */
void * printVolInfo(){  
        printf("This is what is read from the read volume information\n The volume name: %s\n",vInfo->volumeName);
        printf("The volume size is: ");
        printf("%"PRIu64"\n", vInfo->volumeSize);
        printf("The LBA size is: ");
        printf("%"PRIu64"\n", vInfo->LBA_SIZE);
        printf("The root directoy is at: ");
        printf("%"PRIu64"\n", vInfo->rootDir);
        printf("The reading of vol_info is over...\n");    
}


/**
 * List files in the current directory
 * @param currentDirectory
 * @param blockSize
 * @return
 */
void * listFiles(){
    //Get metadata of current directory
    uint64_t * metaData=currentDirectory->filesMeta;

    for(int i=0;i<32;i++){
        uint64_t memoryLocation=metaData[i];
        if(memoryLocation==0){
            continue;
        }
        //Read directory entry
        Dir_Entry * tempDir= malloc(blckSize);
        LBAread(tempDir,1,memoryLocation);
        
        char fileType [32];

        //Check if directory or file
        if(tempDir->typeOfFile==0){
            strcpy(fileType,"file");
        }
        else{
            strcpy(fileType,"Directory");
        }

        //Printing file name
        printf("%s\t\tType: %s\n",tempDir->fileName,fileType);
        free(tempDir);
    }
}

