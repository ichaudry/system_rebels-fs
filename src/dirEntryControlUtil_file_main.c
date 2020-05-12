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



void * copyFromLinux(char * linuxFileName, char * srfsFileName){

    if(duplicateChecker(srfsFileName)==1){
        printf("Try with a different fileName\n");
    }

    FILE * filePointer;

    //Total File Size
    uint64_t fileSize;

    //Number of blocks needed
    uint64_t blocksNeeded;

    //Final block space
    uint64_t finalBlockSpace;

    //Figure out where to start LBA write from....This becomes fd->memory location
    uint64_t lbaStart;

    //Open file to initialize file size
    filePointer = fopen (linuxFileName, "r");

    //Check if file was successfully opened
    if (filePointer) {
        //Calculate file size and how many blocks are need to store file
        fseek(filePointer, 0, SEEK_END);
        fileSize = ftell(filePointer);
        fseek(filePointer,0,SEEK_SET);
        
        
        blocksNeeded=(fileSize +(blckSize-1))/blckSize;

        //Amount of space needed in the final block
        if(fileSize%blckSize==0){
        finalBlockSpace=blckSize;
        }
        else{
        finalBlockSpace=fileSize%blckSize;
        }

        //Find memory 
        lbaStart= findFreeMemory(bitMap,noOfBlocks,blocksNeeded);

        if(!lbaStart){
            printf("Error finding space for file on file System\n");
        }

        printf("This is the size of the file opened:%lu\n",fileSize);

        printf("The size of char in c is %lu\n",sizeof(char));

        printf("The number of block that this file would need is %lu\n",blocksNeeded);

        printf("The last block will have this much data %lu\n",finalBlockSpace);

        printf("This is where the lba start for file writing exists %lu\n",lbaStart);


        for(unsigned long i=0;i<blocksNeeded;i++){
            long start=blckSize*i;
            uint64_t chunkSize;

            if(i==(blocksNeeded-1)){
                printf("Setting read length to final block space needed on final iteration.\n");
                chunkSize=finalBlockSpace;
            }
            else{
                chunkSize=blckSize;
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

        writeFileDirectoryEntry(srfsFileName,currentDirectory,blocksNeeded,lbaStart);

        printf("Write completed without error.\n");

        fclose(filePointer);
    }
    else{
        perror("Error");
        return NULL;
    }
}



void * removeFile(char * fileName){
    uint64_t * metaData= currentDirectory->filesMeta;

    //Parent ID of directory being removed
    uint64_t pid=currentDirectory->memoryLocation;

    //checking find file function
    Dir_Entry * file=findFile(fileName);
    if(file){
        //Total File Size
        uint64_t fileSize=file->lba_blocks;
        printf("The name of the file is: %s\n",file->fileName);
        printf("The size of the file is: %lu\n",file->lba_blocks);

        //Number of blocks allocated to file
        // uint64_t blocksNeeded=(fileSize/512)+1;

        //The starting lba of file
        uint64_t lbaStart=file->memoryLocation;

        //Free the lba blocks in the bitmap
        freeMemoryBits(bitMap,noOfBlocks,lbaStart,fileSize);

        //free the directory entry
        removeDirectoryEntry(fileName);

        //overwrite bitmap
        LBAwrite(bitMap,bitMapSize,1);

        //freeing file pointer
        free(file);

        //End of fucntion
        printf("End of the remove file function was reached\n");
    }
    else{
    printf("No file was found. Either of the two.\n");
    }
}


Dir_Entry * findFile(char * fileName){
    
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
        if(strcmp(tempDir->fileName,fileName)==0){
            //Check if its a directory
            if(tempDir->typeOfFile==1){
            printf("%s is a directory and not a file. Please enter a valid file name.\n",fileName);
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

openFileEntry * fileOpen(char * fileName,char * condition){
    
    //If opening for read
    if(strcmp(condition,"r\n")){
        //check if file exists and get the Directory entry
        Dir_Entry * file=findFile(fileName);

    
        //If file not found return error
        if(!file){
            printf("Error encountered while reading file.\n");
            return NULL;
        }

        //Create a openFile structure
        openFileEntry * fd=malloc(sizeof(openFileEntry));

        //Set file name
        strcpy(fd->fileName,fileName);

        //The file size
        fd->fileSize=file->lba_blocks * blckSize;

        //Set pointer to start of file
        fd->pointer=0;

        //malloc a file buffer to read/write into
        fd->fileBuffer=malloc(blckSize);

        //the starting memory location of file
        fd->memoryLocation=file->memoryLocation;

        free(file);

        return fd;
    }

    if(strcmp(condition,"w\n")){
        //Check if file already exists
        if(duplicateChecker(fileName)==1){
            return NULL;
        }

        //Create a openFile structure
        openFileEntry * fd=malloc(sizeof(openFileEntry));


        //Set file name
        strcpy(fd->fileName,fileName);

        fd->pointer=0;

        //malloc a file buffer to read/write into
        fd->fileBuffer=malloc(blckSize);

        return fd;
    }
}

//For file close after write create directory entry
void * fileClose(openFileEntry * fd){
    //Free file Buffer
    free(fd->fileBuffer);

    //Free file descriptor
    free(fd);
}

void * writeFile(openFileEntry * fd, char * buffer,uint64_t length){

    printf("The number of bytes to write to file are %lu\n",length);

    //No of blocks to read
    unsigned long noOfBlocks= (length + (blckSize-1))/blckSize;

    
    uint64_t currentBlock=fd->pointer/blckSize;
    uint64_t currentOffset=fd->pointer % blckSize;
    
    for(unsigned long i= 0;i<noOfBlocks;i++){

    }



}

void * readFile(openFileEntry * fd, char * buffer,uint64_t length){

    printf("The number of bytes to read from file are %lu\n",length);

    //Allocate the buffer on heap to read file into
    buffer=malloc(length);

    //check for malloc errors
    if(!buffer){
        printf("Malloc of read buffer failed.\n");
        return NULL;
    }

    //Can't read beyond file size
    if(length > fd->fileSize){
        printf("Trying to do a read of %lu bytes on a file that is %lu bytes\n",length,fd->fileSize);
        return NULL;
    }

    //No of blocks to read
    unsigned long noOfBlocks= (length + (blckSize-1))/blckSize;

    for(unsigned long i= 0;i<noOfBlocks;i++){
        LBAread(fd->fileBuffer,1,fd->memoryLocation+i);
        
        //size to memcpy into buffer
        size_t n;
        
        fd->pointer=blckSize * i;
        
        if(i==(noOfBlocks-1)){
            n= length%blckSize;
        }
        else
        {
            n=blckSize;
        }
    
        memcpy(buffer + fd->pointer,fd->fileBuffer,n);

    }

    printf("Read function finished\n");
    return (void *)buffer;
}


#define MYSEEK_CUR 1
#define MYSEEK_POS 2
#define MYSEEK_END 3

uint64_t fileSeek(openFileEntry * fd,uint64_t position, int method){

    switch(method)
    {
        case MYSEEK_CUR:
            fd->pointer += position;
            break;
        
        case MYSEEK_POS:
            fd->pointer = position;

        case MYSEEK_END:
            fd->pointer= fd->fileSize+position;
            break;

        default:
            break;
    }

    return (fd->pointer);

}


// if(duplicateChecker(fileName,currentDirectory,blckSize)==0){
//         char *buffer;
//         size_t buffersize = 0;
//         size_t characters;
//         printf("Begin writing to file \"%s\":\n",fileName);
//         size_t nread;

//          while ((nread = getline(&buffer, &buffersize,stdin)) != -1) {
//                printf("Retrieved line of length %zu:\n", nread);
//             //    fwrite(buffer, nread, 1, stdout);
//            }


//         printf("\n\n\nThis is the data being written: %s\n",buffer); 

//         // printf("%zu characters were read.\n",characters);
//         // printf("You typed: '%s'\n",buffer);
//         // printf("The size of the input is %lu\n",sizeof(buffer));
//         // printf("The size of the input checked from bufsize is %lu\n",bufsize);
   

//     free(buffer);
//     printf("Flushed all buffers\n");




    // uint64_t currentBlock=fd->pointer/blckSize;
    // uint64_t currentOffset=fd->pointer % blckSize;

    // if(length + currentOffset < blckSize){
    //     memcpy(fd->fileBuffer + currentOffset,buffer,length);
    // }

    // else if(length + currentOffset < (blckSize*2)){
    //     memcpy(fd->fileBuffer + currentOffset , buffer, length);
    //     LBAwrite(fd->fileBuffer,2,currentBlock + fd->memoryLocation);
    //     memcpy(fd->fileBuffer,fd->fileBuffer+blckSize,blckSize);  
    // }
    // else{

    // }


    // fd->pointer=fd->pointer+length;
    // uint64_t currentBlock=fd->pointer/blckSize;
    // uint64_t currentOffset=fd->pointer % blckSize;
