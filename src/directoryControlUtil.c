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


typedef struct{
    
    //Need a boolean to check if whole file has been processed?

    uint64_t pointer;   //Current postion of pointer in memory either for read or write
    uint64_t fileSize;
    char * fileBuffer;

    //Need to keep track of last LBA we wrote a chunk of file to?

} openFileEntry;




void * copyFromLinux(Dir_Entry * currentDirectory,int * bitMap,uint64_t noOfBlocks){

    printf("The copy function being successfully reached.\n");

    //Malloc an openFileEntry structure to track the metas of opened file in memory
    // openFileEntry * openFileEntry= malloc(sizeof(openFileEntry));


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
        fseek(filePointer, 0, SEEK_END);
        fileSize = ftell(filePointer);
        blocksNeeded=(fileSize/512)+1;
        finalBlockSpace=fileSize%512;

        lbaStart= findFreeMemory(bitMap,noOfBlocks,blocksNeeded);

        //Set open file entry meta
        // openFileEntry->fileSize=fileSize;
        // openFileEntry->pointer=0;
        
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

    for(int i=0;i<32;i++){
        uint64_t memoryLocation=metaData[i];
        if(memoryLocation==0){
            metaData[i]=lbaPosition;
            currentDirectory->directorySize=temp+1;
            canWrite=1;
            break;
        }
    }
    if(canWrite==0){
        printf("No space to write in the directory, try creating in another directory.\n");
        // free(openFileEntry);
        free(fileEntry);
        return NULL;
    }

    //Store the LBA of new directory in parent directories meta data array
    // currentDirectory->filesMeta[temp]=lbaPosition;

    printf("Overwriting current directories meta data with new updaed fields.\n");

    //Overwrite the parent directory with the new meta data
    LBAwrite(currentDirectory,1,currentDirectory->memoryLocation);

 //Write directory entry to disk
    LBAwrite(fileEntry,1,lbaPosition);

    //Modify bit map 
    occupyMemoryBits(bitMap,noOfBlocks,lbaPosition,1);

    LBAwrite(bitMap,5,1);

    free(fileEntry);

    // free(openFileEntry);
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

    uint64_t * metaData=currentDirectory->filesMeta;

    int canWrite=0;

    for(int i=0;i<32;i++){
        uint64_t memoryLocation=metaData[i];
        if(memoryLocation==0){
            metaData[i]=lbaPosition;
            currentDirectory->directorySize=temp+1;
            canWrite=1;
            break;
        }
    }
    if(canWrite==0){
        printf("No space to write in the directory, try creating in another directory.\n");
        free(directory);
        return NULL;
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