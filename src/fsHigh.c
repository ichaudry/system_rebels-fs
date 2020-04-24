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
#include "directoryControlUtil.h"
#include "fsHigh.h"


uint64_t volumeSize;
uint64_t blckSize;
uint64_t noOfBlocks;
uint64_t bitMapSize;
uint64_t arraySize;
Volume_Information * vInfo;
int * bitMap;

//Current active directory
Dir_Entry * currentDirectory;


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


void * writeFile(){
    //Get user input into a buffer that realloatable
    //Keep track of how many bytes of char data
    //Calculate how many 512 byte blocks you will need to write the data
    //Malloc a buffer on a 512 boundary rounding up
    //do a LBAwrite() using the buffer that holds the user input, the number of blocks needed and a starting point that can be anything
}

void * readFile(){
    //To test keep track of where you wrote the file from above 
    //Basically a reverse process where you do a LBA read into a malloced buffer of same size as above 
    //Printf the buffer to check integrity of data

}

void * moveFile(){
    //Write some data on a number of blocks 
    //REMEMBER to use malloc for reading and writing
    //Keep track of how many blocks 
    //read that data into a buffer 
    //write it to another random place 
    //delete the old data
}
            
            
void * copyFile(){
    //Write some data on a number of blocks 
    //REMEMBER to use malloc for reading and writing
    //Keep track of how many blocks 
    //read that data into a buffer 
    //write it to another random place 
}



void * mkdir(char * dirName){
    writeDirectory(dirName,currentDirectory,bitMap,bitMapSize,blckSize,noOfBlocks);
    
}

void * rmdir(char * dirName){
    removeDirectory(dirName,currentDirectory,bitMap,bitMapSize,blckSize,noOfBlocks);
}


void * cdRoot(){
    Dir_Entry *directory=changeDirectoryRoot(currentDirectory,blckSize);
    
     //Catch null pointer returns
    if(directory){
    free(currentDirectory);
    currentDirectory=directory;
    }
}

void * cd(char * dirName){
    Dir_Entry *directory=changeDirectory(dirName,currentDirectory,blckSize);

    //Catch null pointer returns
    if(directory){
    free(currentDirectory);
    currentDirectory=directory;
    }
}

void * pwd(){
    printCurrentDirectory(currentDirectory);
}

void * vinfo(){  
    printVolInfo(vInfo);
}

void * ls(){
    listFiles(currentDirectory,blckSize);
}

void * getBitMap(){
        count(bitMap,0,noOfBlocks);  
}

void * freeBuffers(){
    free(vInfo);
    free(bitMap);
    free(currentDirectory);
}

