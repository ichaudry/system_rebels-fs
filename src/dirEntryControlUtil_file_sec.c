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


void * copyFile(char * fileName,char * newFileName)
{
    Dir_Entry * file= findFile(fileName);

    if(!file){
        return NULL;
    }

    if(currentDirectory->directorySize==32){
        printf("No space in current directory. Please move file in new directory and create copy there.\n");
        free(file);
        return NULL;
    }

    //read file to be copied into buffer
    char * buffer= malloc(file->lba_blocks *blckSize);
    LBAread(buffer,file->lba_blocks,file->memoryLocation);

    //LBA for new file
    uint64_t lbaStart= findFreeMemory(bitMap,noOfBlocks,file->lba_blocks);
   
    //Write the new file
    LBAwrite(buffer,file->lba_blocks,file->memoryLocation);

    //Occupying memory in bitmap
    occupyMemoryBits(bitMap,noOfBlocks,lbaStart,file->lba_blocks);

    writeFileDirectoryEntry(newFileName,currentDirectory,file->lba_blocks,lbaStart);

    free(file);

    printf("Copy complete\n");
}



void * renameFile(char * fileName, char * newFileName){
   uint64_t * metaData=currentDirectory->filesMeta;

    //Parent ID of directory being renamed
    uint64_t pid=currentDirectory->memoryLocation;

    //Find directory entry to renamed in current directory
    for(int i=0;i<32;i++){
        uint64_t memoryLocation=metaData[i];
        if(memoryLocation==0){
            continue;
        }
        Dir_Entry * tempDir= malloc(blckSize);
        LBAread(tempDir,1,memoryLocation); 
        
        //check if the names are a match
        if(strcmp(tempDir->fileName,fileName)==0){
            strcpy(tempDir->fileName,newFileName);
            
            //Overwrite the directory entry with new name
            LBAwrite(tempDir,1,memoryLocation);
    
            free(tempDir);
            
            return NULL;
        }
        free(tempDir);
    }
    printf("The file with that name was not found. Please enter a valid file name.\n");
}   


void * moveFile(char * fileName,char * dirName)
{
    //Get directory entry of file to be moved
    Dir_Entry * file= findFile(fileName);

    if(!file){
        return NULL;
    }

    //Get directory entry of directory where file needs to moved.
    Dir_Entry * directory= findDirectory(dirName);

    if(!directory){
        return NULL;
    }

    writeFileDirectoryEntry(file->fileName,directory,file->lba_blocks,file->memoryLocation);

    removeDirectoryEntry(fileName);

    free(file);
    free(directory);

    printf("Move Complete.\n");

}
