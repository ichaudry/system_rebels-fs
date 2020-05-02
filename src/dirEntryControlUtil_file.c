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


//Haven't really used yet. 
//May be more applicable when doing mutlithreaded file processing 
//Decide later
typedef struct{
    
    //Need a boolean to check if whole file has been processed?

    uint64_t pointer;   //Current postion of pointer in memory either for read or write
    uint64_t fileSize;
    char * fileBuffer;

    //Need to keep track of last LBA we wrote a chunk of file to?

} openFileEntry;

void * copyFromLinux(Dir_Entry * currentDirectory,int * bitMap,uint64_t noOfBlocks){
    FILE * filePointer;

    //Total File Size
    uint64_t fileSize;

    //Number of blocks needed
    uint64_t blocksNeeded;

    //Final block space
    uint64_t finalBlockSpace;

    //Figure out where to start LBA write from
    uint64_t lbaStart;

    //Open file to initialize file size
    filePointer = fopen ("test2.txt", "r");

    //Check if file was successfully opened
    if (filePointer) {
        //Calculate file size and how many blocks are need to store file
        fseek(filePointer, 0, SEEK_END);
        fileSize = ftell(filePointer);
        blocksNeeded=(fileSize/512)+1;

        //Amount of space needed in the final block
        finalBlockSpace=fileSize%512;

        //Find memory 
        lbaStart= findFreeMemory(bitMap,noOfBlocks,blocksNeeded);

        printf("This is the size of the file opened:%lu\n",fileSize);

        printf("The size of char in c is %lu\n",sizeof(char));

        printf("The number of block that this file would need is %lu\n",blocksNeeded);

        printf("The last block will have this much data %lu\n",finalBlockSpace);

        printf("This is where the lba start for file writing exists %lu\n",lbaStart);


        for(long i=0;i<blocksNeeded;i++){
            long start=512*i;
            uint64_t chunkSize;

            if(i==(blocksNeeded-1)){
                printf("Setting read length to final block space needed on final iteration.\n");
                chunkSize=finalBlockSpace;
            }
            else{
                chunkSize=512;
            }

            printf("The start of loop %lu is %lu\n",i,start);

            printf("The chunk size being read is %lu\n",chunkSize);

            //Set file pointer to start of chunk thread needs to read
            fseek (filePointer, start, SEEK_SET);

            //Allocate buffer to store file content
            char * fileContent = malloc (512);
            
            if (fileContent)
                {
            //Load the buffer up with threads desginated portion of file
            fread (fileContent, 1, chunkSize, filePointer);

            //LBA write
            printf("This is the lba position being written %lu\n",lbaStart+i);
            LBAwrite(fileContent,1,lbaStart+i);

            //Occupy bitmaps to reflect written data
            occupyMemoryBits(bitMap,noOfBlocks,lbaStart+i,1);

            }
            else{
            perror("Error");
            }
            free(fileContent);
        }
        fclose(filePointer);
    }
    else{
        perror("Error");
    }

    printf("Write completed without error.\n");

    //Creating directory entry for the file
    Dir_Entry * fileEntry= malloc(512);
    strcpy(fileEntry->fileName,"test2big");
    fileEntry->typeOfFile=0;
    fileEntry->lba_blocks=blocksNeeded;
    fileEntry->parentDirectory=currentDirectory->memoryLocation;
    fileEntry->memoryLocation=lbaStart;

    //Find free blocks to store file directory entry
    uint64_t lbaPosition=findFreeMemory(bitMap,noOfBlocks,1);

    uint64_t temp= currentDirectory->directorySize;

    uint64_t * metaData=currentDirectory->filesMeta;

    int canWrite=0;

    //Check if a directory is full
    if(currentDirectory->directorySize==32){
        printf("No space to write in the directory, try creating in another directory.\n");
        free(fileEntry);
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
     
    printf("Overwriting current directories meta data with new updaed fields.\n");

    //Overwrite the parent directory with the new meta data
    LBAwrite(currentDirectory,1,currentDirectory->memoryLocation);

    //Write directory entry to disk
    LBAwrite(fileEntry,1,lbaPosition);

    //Modify bit map 
    occupyMemoryBits(bitMap,noOfBlocks,lbaPosition,1);

    LBAwrite(bitMap,5,1);

    free(fileEntry);

}



void * removeFile(char * fileName,Dir_Entry * currentDirectory,int * bitMap,uint64_t bitMapSize,uint64_t blockSize,uint64_t noOfBlocks){
    uint64_t * metaData= currentDirectory->filesMeta;

    //Parent ID of directory being removed
    uint64_t pid=currentDirectory->memoryLocation;

    /*
    *Delete all the blocks associated with the file
    *-You know the starting block 
    *-Do the same math to figure out how many blocks will be occupied by the file
    *-Call the free bit map function to free the corresponding blocks
    *
    * Delete the directory entry for the file
    * DONE
    */

    //checking find file function
    Dir_Entry * file=findFile(fileName,currentDirectory,blockSize);
    if(file){
        //Total File Size
        uint64_t fileSize=file->lba_blocks;
        printf("The name of the file is: %s\n",file->fileName);
        printf("The size of the file is: %lu\n",file->lba_blocks);

        //Number of blocks allocated to file
        uint64_t blocksNeeded=(fileSize/512)+1;

        //The starting lba of file
        uint64_t lbaStart=file->memoryLocation;

        //Free the lba blocks in the bitmap
        freeMemoryBits(bitMap,noOfBlocks,lbaStart,blocksNeeded);







    }
    else{
    printf("No file was found or your function is fucked up. Either of the two.\n");
    }
}

Dir_Entry * findFile(char * fileName,Dir_Entry * currentDirectory,uint64_t blockSize){
    
    uint64_t * metaData=currentDirectory->filesMeta;


    //Find directory to remove in current directory
    for(int i=0;i<32;i++){
        uint64_t memoryLocation=metaData[i];
        if(memoryLocation==0){
            continue;
        }

        //Allocate a directory entry to read into 
        Dir_Entry * tempDir= malloc(blockSize);
        LBAread(tempDir,1,memoryLocation);

        //check if the names are a match
        if(strcmp(tempDir->fileName,fileName)==0){
            //Check if its a directory
            if(tempDir->typeOfFile==1){
            printf("%s is a directory and not a file. Please enter a valid file name to remove.\n",fileName);
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




void * writeFile(char * fileName,Dir_Entry * currentDirectory,int * bitMap,uint64_t bitMapSize,uint64_t blockSize,uint64_t noOfBlocks){
    if(duplicateChecker(fileName,currentDirectory,blockSize)==0){
        char *buffer;
        size_t bufsize = 0;
        size_t characters;
        printf("Begin writing to file \"%s\":\n",fileName);

        while((characters = getline(&buffer,&bufsize,stdin))!=-1){
            /**
             * THIS IS THE REASON WE NEED A FILE DESCRIPTOR STRUCTURE THAT CAN KEEP A BUFFER SET TO WHERE YOU ARE WRITING 
             * THE ABOVE FUNCTION IS GOING TO HAND YOU A BUFFER OF SOME SIZE ON EACH CALL. 
             * HAND IT TO FUNCTION THAT IS ALLOCATED AT A BLOCK SIZE BOUNDARY 
             * DO A MEMCOPY SO NOW THAT BUFFER HAS IT 
             * AFTER END OF FILE MAKE SURE TO WRITE THAT BUFFER 
             */

        // printf("%zu characters were read.\n",characters);
        // printf("You typed: '%s'\n",buffer);
        // printf("The size of the input is %lu\n",sizeof(buffer));
        // printf("The size of the input checked from bufsize is %lu\n",bufsize);
        }
        free(buffer);
        printf("Flushed all buffers\n");

    }
}



