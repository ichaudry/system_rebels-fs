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
#include "directoryControlUtil.h"
#include "fsHigh.h"





void * writeFile(char * fileName,Dir_Entry * currentDirectory,int * bitMap,uint64_t bitMapSize,uint64_t blockSize,uint64_t noOfBlocks){

    if(duplicateChecker(fileName,currentDirectory,blockSize)==0){
        char *buffer;
        size_t bufsize = 0;
        size_t characters;
        printf("Begin writing to file \"%s\": ",fileName);
        characters = getline(&buffer,&bufsize,stdin);
        printf("%zu characters were read.\n",characters);
        printf("You typed: '%s'\n",buffer);
        printf("The size of the input is %lu\n",sizeof(buffer));
        printf("The size of the input checked from bufsize is %lu\n",bufsize);

        free(buffer);
        

    }
}


/**
 * Creates a new directory under the current working directory
 * @param dirName
 * @param currentDirectory
 * @param bitMap
 * @param bitMapSize
 * @param blockSize
 * @param noOfBlocks
 * @return
 */
void * writeDirectory(char * dirName,Dir_Entry * currentDirectory,int * bitMap,uint64_t bitMapSize,uint64_t blockSize,uint64_t noOfBlocks){

    if(duplicateChecker(dirName,currentDirectory,blockSize)==0){
    
    Dir_Entry * directory= malloc(blockSize);

    //Parent ID for new directory
    uint64_t pid=currentDirectory->memoryLocation;
    
    strcpy(directory->fileName,dirName);

    //Set type 1 for directory
    directory->typeOfFile=1;

    directory->directorySize=0;

    static const uint64_t tempMeta[32]={0};
    
    memcpy(directory->filesMeta,tempMeta,sizeof tempMeta);

    //Set the parent directory address to be the address of current working directory
    directory->parentDirectory=pid;

    //Find free blocks to store file
    uint64_t lbaPosition=findFreeMemory(bitMap,noOfBlocks,1);

    directory->memoryLocation=lbaPosition;

    uint64_t temp= currentDirectory->directorySize;

    currentDirectory->directorySize=temp+1;

    uint64_t * metaData=currentDirectory->filesMeta;

    int canWrite=0;

    for(int i=0;i<32;i++){
        uint64_t memoryLocation=metaData[i];
        if(memoryLocation==0){
            metaData[i]=lbaPosition;
            canWrite=1;
            break;
        }
    }
    if(canWrite==0){
        printf("No space to write in the directory, try creating in another directory.\n");
    }


    //Store the LBA of new directory in parent directories meta data array
    // currentDirectory->filesMeta[temp]=lbaPosition;

    //Overwrite the parent directory with the new meta data
    LBAwrite(currentDirectory,1,pid);

    //Write directory entry to disk
    LBAwrite(directory,1,lbaPosition);

    //Modify bit map 
    occupyMemoryBits(bitMap,noOfBlocks,lbaPosition,1);

    LBAwrite(bitMap,bitMapSize,1);

    free(directory);
    }
}



void * removeDirectory(char * dirName,Dir_Entry * currentDirectory,int * bitMap,uint64_t bitMapSize,uint64_t blockSize,uint64_t noOfBlocks){
    uint64_t * metaData=currentDirectory->filesMeta;

    //Parent ID of directory being removed
    uint64_t pid=currentDirectory->memoryLocation;

    //Find directory to remove in current directory
    for(int i=0;i<32;i++){
        uint64_t memoryLocation=metaData[i];
        if(memoryLocation==0){
            continue;
        }
        Dir_Entry * tempDir= malloc(blockSize);
        LBAread(tempDir,1,memoryLocation); 
        
        //check if the names are a match
        if(strcmp(tempDir->fileName,dirName)==0){
            //Check not directory
            if(tempDir->typeOfFile==0){
            printf("%s is not a directory. Please enter a valid directory name to remove.\n",dirName);
            free(tempDir);
            break;
            }

            //Set the bit map at that locaton as free
            freeMemoryBits(bitMap, noOfBlocks, memoryLocation,1);

            //Set the meta data in the roots as 0 indicating the place is free
            metaData[i]=0;

            uint64_t temp= currentDirectory->directorySize;

            currentDirectory->directorySize=temp-1;

            //Overwrite the parent directory with the new meta data
            LBAwrite(currentDirectory,1,pid);

            //overwrite bitmap
            LBAwrite(bitMap,bitMapSize,1);
    
            free(tempDir);
            
            break;
        }
        free(tempDir);
    }
    printf("The directory with that name was not found. Please enter a valid direcotry name.\n");
    
}

/**
 * Go to root directory
 * @param currentDirectory
 * @param blockSize
 * @return
 */
Dir_Entry * changeDirectoryRoot(Dir_Entry * currentDirectory,uint64_t blockSize){
    uint64_t parentAd= currentDirectory->parentDirectory;
    
    //Check if root
    if(parentAd==0){
        printf("The current directory is the root of the file system and there are no root nodes.\n");
        return NULL;
    }

    Dir_Entry * tempDir= malloc(blockSize);
    LBAread(tempDir,1,parentAd);

    return tempDir;
}

/**
 * Change directory to a valid directory within the current working directory
 * @param dirName
 * @param currentDirectory
 * @param blockSize
 * @return
 */
Dir_Entry * changeDirectory(char * dirName,Dir_Entry * currentDirectory, uint64_t blockSize){
    
    uint64_t * metaData=currentDirectory->filesMeta;

    //Find directory in current directory
    for(int i=0;i<32;i++){
        uint64_t memoryLocation=metaData[i];
        if(memoryLocation==0){
            continue;
        }
        Dir_Entry * tempDir= malloc(blockSize);
        LBAread(tempDir,1,memoryLocation); 
        
        //check if the names are a match
        if(strcmp(tempDir->fileName,dirName)==0){
            //Check not directory
            if(tempDir->typeOfFile==0){
            printf("%s is not a directory. Please enter a valid directory name.\n",dirName);
            return NULL;
            }
            return tempDir;
        }
        free(tempDir);
    }
    printf("The directory with that name was not found. Please enter a valid direcotry name.\n");
    return NULL;
}

/**
 * Check if a file with name already exists
 * @param fileName
 * @param currentDirectory
 * @param blockSize
 * @return
 */
int duplicateChecker(char * fileName,Dir_Entry * currentDirectory, uint64_t blockSize){
    uint64_t * metaData=currentDirectory->filesMeta;

    //Check if file exists in current directory
    for(int i=0;i<32;i++){
        uint64_t memoryLocation=metaData[i];
        if(memoryLocation==0){
            continue;
        }
        Dir_Entry * tempDir= malloc(blockSize);
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

/**
 * prints current working directory
 * @param currentDirectory
 * @return
 */
void * printCurrentDirectory(Dir_Entry * currentDirectory){
    printf("This is what is read from the current directory\nThe directory name is: %s\n",currentDirectory->fileName);
    printf("The type of file is: ");
    printf("%"PRIu64"\n",currentDirectory->typeOfFile);
    printf("The size of the directory is: %lu\n", currentDirectory->directorySize);
    printf("The meta data contained by file is: \n");
    printMeta(currentDirectory->filesMeta);
    printf("The parent directory address is: %lu\n",currentDirectory->parentDirectory);
    printf("The memory location of file is: %lu\n", currentDirectory->memoryLocation);
}

void * printDirectory(Dir_Entry * directory){  
        printf("This is what is read from the current directory\n The directory name is: %s\n",directory->fileName);
        printf("The type of file is: ");
        printf("%"PRIu64"\n",directory->typeOfFile);
        printf("The size of the directory is: %lu\n", directory->directorySize);
        printf("The meta data contained by file is: \n");
        // printMeta();
        printf("The memory location of file is: %lu\n", directory->memoryLocation);

}

void * printMeta(uint64_t * metaData){
    for(int i=0;i<32;i++){
        printf("The index %d holds: %lu\n",i,metaData[i]);
    }
}


void * printVolInfo(Volume_Information * vInfo){  
        printf("This is what is read from the read volume information\n The volume name: %s\n",vInfo->volumeName);
        printf("The volume size is: ");
        printf("%"PRIu64"\n", vInfo->volumeSize);
        printf("The LBA size is: ");
        printf("%"PRIu64"\n", vInfo->LBA_SIZE);
        printf("The root directoy is at: ");
        printf("%"PRIu64"\n", vInfo->rootDir);
        printf("The reading of vol_info is over...\n");    
}


void * listFiles(Dir_Entry * currentDirectory, uint64_t blockSize){
    uint64_t * metaData=currentDirectory->filesMeta;

    for(int i=0;i<32;i++){
        uint64_t memoryLocation=metaData[i];
        if(memoryLocation==0){
            continue;
        }
        Dir_Entry * tempDir= malloc(blockSize);
        LBAread(tempDir,1,memoryLocation);
        
        //Printing file name
        printf("%s\n",tempDir->fileName);
        free(tempDir);
    }
}