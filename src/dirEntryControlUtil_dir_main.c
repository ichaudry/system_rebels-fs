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
 * Creates a new directory under the current working directory
 * @param dirName
 * @param currentDirectory
 * @param bitMap
 * @param bitMapSize
 * @param blckSize
 * @param noOfBlocks
 * @return
 */
void * writeDirectory(char * dirName){

    if(duplicateChecker(dirName)==0){
    
    Dir_Entry * directory= malloc(blckSize);

    //Parent ID for new directory
    uint64_t pid=currentDirectory->memoryLocation;
    
    strcpy(directory->fileName,dirName);

    //Set type 1 for directory
    directory->typeOfFile=1;

    directory->directorySize=0;

    directory->lba_blocks=1;

    static const uint64_t tempMeta[32]={0};
    
    memcpy(directory->filesMeta,tempMeta,sizeof tempMeta);

    //Set the parent directory address to be the address of current working directory
    directory->parentDirectory=pid;

    //Find free blocks to store file
    uint64_t lbaPosition=findFreeMemory(bitMap,noOfBlocks,1);

    directory->memoryLocation=lbaPosition;

    uint64_t temp= currentDirectory->directorySize;

    uint64_t * metaData=currentDirectory->filesMeta;

    //Check if a directory is full
    if(currentDirectory->directorySize==32){
        printf("No space to write in the directory, try creating in another directory.\n");
        free(directory);
        return NULL;
    }

    //Find a free block to create a directory entry in current directory
    for(int i=0;i<32;i++){
        uint64_t memoryLocation=metaData[i];
        if(memoryLocation==0){
            metaData[i]=lbaPosition;
            currentDirectory->directorySize=temp+1;
            break;
        }
    }

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

void * writeFileDirectoryEntry(char * fileName,Dir_Entry * parentDir,uint64_t blocksNeeded,uint64_t lbaStart){
    
    
    
    //Creating directory entry for the file
    Dir_Entry * fileEntry= malloc(512);
    strcpy(fileEntry->fileName,fileName);
    fileEntry->typeOfFile=0;
    fileEntry->lba_blocks=blocksNeeded;
    fileEntry->parentDirectory=parentDir->memoryLocation;
    fileEntry->memoryLocation=lbaStart;

    //Find free blocks to store file directory entry
    uint64_t lbaPosition=findFreeMemory(bitMap,noOfBlocks,1);

    uint64_t temp= parentDir->directorySize;

    uint64_t * metaData=parentDir->filesMeta;


    //Check if a directory is full
    if(parentDir->directorySize==32){
        printf("No space to write in the directory, try creating in another directory.\n");
        free(fileEntry);
        return NULL;
    }

    //Find a free block to create a directory entry in current directory
    for(int i=0;i<32;i++){
        uint64_t memoryLocation=metaData[i];
        if(memoryLocation==0){
            metaData[i]=lbaPosition;
            parentDir->directorySize=temp+1;
            break;
        }
    }
     
    // printf("Overwriting current directories meta data with new updaed fields.\n");

    //Overwrite the parent directory with the new meta data
    LBAwrite(parentDir,1,parentDir->memoryLocation);

    //Write directory entry to disk
    LBAwrite(fileEntry,1,lbaPosition);

    //Modify bit map 
    occupyMemoryBits(bitMap,noOfBlocks,lbaPosition,1);

    LBAwrite(bitMap,bitMapSize,1);

    free(fileEntry);
}

/**
 * Remove a directory and all of its files
 * @param dirName
 * @param currentDirectory
 * @param bitMap
 * @param bitMapSize
 * @param blckSize
 * @param noOfBlocks
 * @return
 */
void * removeDirectory(char * dirName){
    uint64_t * metaData=currentDirectory->filesMeta;

    //Parent ID of directory being removed
    uint64_t pid=currentDirectory->memoryLocation;

    //Find directory to remove in current directory
    for(int i=0;i<32;i++){
        uint64_t memoryLocation=metaData[i];
        if(memoryLocation==0){
            continue;
        }
        Dir_Entry * tempDir= malloc(blckSize);
        LBAread(tempDir,1,memoryLocation); 
        
        //check if the names are a match
        if(strcmp(tempDir->fileName,dirName)==0){
            //Check not directory
            if(tempDir->typeOfFile==0){
                printf("%s is not a directory. Please enter a valid directory name to remove.\n",dirName);
                free(tempDir);
                return NULL;
            }

            if(tempDir->directorySize > 0){
                printf("Cannot delete directory because there are files inside it. Try removing all file inside and then removing directory. Or wait for the next release of file system which shall have a recursive delete function.\n");
                return NULL;
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
            
            return NULL;
        }
        free(tempDir);
    }
    printf("The directory with that name was not found. Please enter a valid direcotry name.\n");
    
}



/**
 * Remove a directory entry
 * @param dirName
 * @param currentDirectory
 * @param bitMap
 * @param bitMapSize
 * @param blckSize
 * @param noOfBlocks
 * @return
 */
void * removeDirectoryEntry(char * dirEntryName){
    uint64_t * metaData=currentDirectory->filesMeta;

    //Parent ID of directory being removed
    uint64_t pid=currentDirectory->memoryLocation;

    //Find directory entry to remove in current directory
    for(int i=0;i<32;i++){
        uint64_t memoryLocation=metaData[i];
        if(memoryLocation==0){
            continue;
        }
        Dir_Entry * tempDir= malloc(blckSize);
        LBAread(tempDir,1,memoryLocation); 
        
        //check if the names are a match
        if(strcmp(tempDir->fileName,dirEntryName)==0){
            //Check if directory
            if(tempDir->typeOfFile==1){
            printf("%s is a directory. Please use the rmdir command to remove a directory and all of its contents.\n",dirEntryName);
            free(tempDir);
            break;
            }

            //Set the bit map at that location as free
            freeMemoryBits(bitMap, noOfBlocks, memoryLocation,1);

            //Set the meta data in the roots as 0 indicating the place is free
            metaData[i]=0;

            uint64_t temp= currentDirectory->directorySize;

            currentDirectory->directorySize=temp-1;

            //Overwrite the parent directory with the new meta data
            LBAwrite(currentDirectory,1,pid);

            // //overwrite bitmap
            // LBAwrite(bitMap,bitMapSize,1);
    
            free(tempDir);
            
            return NULL;
        }
        free(tempDir);
    }
    printf("The directory entry with that name was not found. Please enter a valid direcotry name.\n");
    
}



/**
 * Go to root directory
 * @param currentDirectory
 * @param blckSize
 * @return
 */
Dir_Entry * changeDirectoryRoot(){
    uint64_t parentAd= currentDirectory->parentDirectory;
    
    //Check if root
    if(parentAd==0){
        printf("The current directory is the root of the file system and there are no root nodes.\n");
        return NULL;
    }

    Dir_Entry * tempDir= malloc(blckSize);
    LBAread(tempDir,1,parentAd);

    return tempDir;
}

/**
 * Change directory to a valid directory within the current working directory
 * @param dirName
 * @return
 */
Dir_Entry * changeDirectory(char * dirName){
    
    uint64_t * metaData=currentDirectory->filesMeta;

    //Find directory in current directory
    for(int i=0;i<32;i++){
        uint64_t memoryLocation=metaData[i];
        if(memoryLocation==0){
            continue;
        }
        Dir_Entry * tempDir= malloc(blckSize);
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
