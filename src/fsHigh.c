/**
 * fsHigh.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include "fsLow.h"
#include "fileSystem.h"
#include "bitMapUtil.h"
#include "fsHigh.h"
#include "dirEntryControlUtil.h"



/**
 * Starts the partition system
 * Formats the partition if not already formatted by writing the free list and volume information
 * Allocates space for root directory 
 * @param volname
 * @param volSize
 * @param blockSize
 * @param format 0= don't format , 1= format
 * @return
 */
int startFileSystem(char * volName, uint64_t * volSize, uint64_t * blockSize, int format){
    volumeSize=* volSize;
    blckSize=* blockSize;
    noOfBlocks=volumeSize/blckSize;
    arraySize= noOfBlocks/(uint64_t)(sizeof(int)*8)+1;
    bitMapSize= ((arraySize * sizeof(int))/(uint64_t)blckSize)+1;

    printf("The number of blocks in our volume are:%lu\n",noOfBlocks);
    printf("The arraySize would be:%lu\n",arraySize);
    printf("The number of bitMapSize needed are:%lu\n",bitMapSize);


    //Initialize partition system and catch errors
    int ret= startPartitionSystem(volName,volSize, blockSize);

    if(ret==0){
        printf("Success\n");

        //Format the partition with VCB, bit map, and root directory
        if(format==1){
            printf("Formatting the partition system.\n");
            
            //Initialize volume information block to write to partion
            Volume_Information * volInfo=malloc(*blockSize);

            strcpy(volInfo->volumeName,volName);
            volInfo->volumeSize=* volSize;
            volInfo->LBA_SIZE= * blockSize;

            //Initialize bitMap to write to partition
            int * bitMap=malloc(bitMapSize*blckSize);
     
            //Initialize bitMap to all zeroes 
            for (int i = 0; i < arraySize; i++ ){
                bitMap[i] = 0;  
            }

            volInfo->rootDir=6;

            Dir_Entry * rootDirectory=malloc(blckSize);

            char * rootName= "root";
            strcpy(rootDirectory->fileName,"root");
            rootDirectory->typeOfFile=1;
            rootDirectory->directorySize=0;
            rootDirectory->parentDirectory=0;

            static const uint64_t tempMeta[32]={0};
            memcpy(rootDirectory->filesMeta,tempMeta,sizeof tempMeta);
            rootDirectory->memoryLocation=6;


            //Mark dirty bits for Root Dir
            occupyMemoryBits(bitMap,noOfBlocks,0,7); 
            
            //Write the volume information block
            LBAwrite(volInfo,1,0);

            //Write the bitmap
            LBAwrite(bitMap,bitMapSize,1);

            //Write the root directory
            LBAwrite(rootDirectory,1,6);

            free(bitMap);
            free(volInfo);
            free(rootDirectory);
        }

        else{
            printf("File is already formatted\n");
        } 
        //Load volume information block into memory
        vInfo=malloc(blckSize);
        LBAread(vInfo,1,0);

        //Load bitmap into memory
        bitMap=malloc(bitMapSize*blckSize);
        LBAread(bitMap,bitMapSize,1);

        //Load root directory into memory
        currentDirectory=malloc(blckSize);
        LBAread(currentDirectory,1,6);

    }
    else if(ret==-1){
        printf("File exists but can not open for write\n");
    }
    else if(ret==-2){
        printf("Insufficient space for the volume");
    }

    return 0;
}



void * mkdir(char * dirName){
    writeDirectory(dirName);
    
}

void * rmdir(char * dirName){
    removeDirectory(dirName);
}

void * rm(char * fileName){
    removeFile(fileName);
}

void * fsCopyFromLinux(char * linuxFileName, char * srfsFileName){
    copyFromLinux(linuxFileName,srfsFileName);
}

void * fsReadFile(char * fileName,uint64_t length){
    char * buffer;
    openFileEntry * fd= fileOpen(fileName,"r");
    if(fd){
    buffer=(char *)readFile(fd,buffer,length);
    printf("The sizeof read  buffer is %lu\nHere are the contents read:\n\n%s\n\n",strlen(buffer),buffer);
    free(buffer);
    fileClose(fd);
    }
}

void * fsMove(char * fileName, char * dirName){
    moveFile(fileName,dirName);
}


void * cdRoot(){
    Dir_Entry *directory=changeDirectoryRoot();
    
     //Catch null pointer returns
    if(directory){
    free(currentDirectory);
    currentDirectory=directory;
    }
}

void * cd(char * dirName){
    Dir_Entry *directory=changeDirectory(dirName);

    //Catch null pointer returns
    if(directory){
    free(currentDirectory);
    currentDirectory=directory;
    }
}

void * pwd(){
    printCurrentDirectory();
}

void * vinfo(){  
    printVolInfo(vInfo);
}

void * ls(){
    listFiles();
}

void * getBitMap(){
        count(bitMap,0,noOfBlocks);  
}

void * freeBuffers(){
    free(vInfo);
    free(bitMap);
    free(currentDirectory);
}

