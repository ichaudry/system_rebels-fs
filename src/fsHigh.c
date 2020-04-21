#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include "fsLow.h"
#include "fileSystem.h"
#include "bitMapUtil.h"
#include "fsHigh.h"


uint64_t vol_Size;
uint64_t block_Size;
uint64_t noOfBlocks;
uint64_t bitBlocks;
uint64_t arraySize;
Volume_Information * v_Info;
int * bitMap;

//Current active directory
Dir_Entry * currentDirectory;


/**
 * Starts the partition system
 * Formats the partition if not already formatted by writing the free list and volume information
 * Allocate space for root directory 
 * @param volname
 * @param volSize
 * @param blockSize
 * @param format 0= don't format , 1= format
 * @return
 */
int startFileSystem(char * volName, uint64_t * volSize, uint64_t * blockSize, int format){
    vol_Size=* volSize;
    block_Size=* blockSize;
    noOfBlocks=vol_Size/block_Size;
    arraySize= noOfBlocks/(uint64_t)(sizeof(int)*8)+1;
    bitBlocks= ((arraySize * sizeof(int))/(uint64_t)block_Size)+1;

    printf("The number of blocks in our volume are:%lu\n",noOfBlocks);
    printf("The arraySize would be:%lu\n",arraySize);
    printf("The number of bitblocks needed are:%lu\n",bitBlocks);


    //Initialize partition system and catch errors
    int ret= startPartitionSystem(volName,volSize, blockSize);

    if(ret==0){
        printf("Success\n");

        //Format the partition with VCB, bit map, and root directory
        if(format==1){
            printf("Formatting the partition system.\n");
            
            //Initialize volume information block to write to partion
            Volume_Information * v_Info=malloc(*blockSize);

            strcpy(v_Info->volumeName,volName);
            v_Info->volumeSize=* volSize;
            v_Info->LBA_SIZE= * blockSize;

            //Initialize bitMap to write to partition
            int * bitMap=malloc(bitBlocks*block_Size);
     
            //Initialize bitMap to all zeroes 
            for (int i = 0; i < arraySize; i++ ){
                bitMap[i] = 0;  
            }


            v_Info->rootDir=6;

            Dir_Entry * rootDirectory=malloc(block_Size);

            char * rootName= "root";
            strcpy(rootDirectory->fileName,"root");
            rootDirectory->typeOfFile=1;
            rootDirectory->directorySize=0;
            rootDirectory->parentDirectory=0;

            static const uint64_t tempMeta[32]={0};
            memcpy(rootDirectory->filesMeta,tempMeta,sizeof tempMeta);
            rootDirectory->memoryLocation=6;


            //Mark dirty bits for Root Dir
            occupyMemoryBits(bitMap,0,6); 
            
            //Write the volume information block
            LBAwrite(v_Info,1,0);

            //Write the bitmap
            LBAwrite(bitMap,bitBlocks,1);

            //Write the root directory
            LBAwrite(rootDirectory,1,6);

            free(bitMap);
            free(v_Info);
            free(rootDirectory);
        }

        else{
            printf("File is already formatted\n");
        } 
        //Load volume information block into memory
        v_Info=malloc(block_Size);
        LBAread(v_Info,1,0);

        //Load bitmap into memory
        bitMap=malloc(bitBlocks*block_Size);
        LBAread(bitMap,bitBlocks,1);

        //Load root directory into memory
        currentDirectory=malloc(block_Size);
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


void * writeDirectory(char * dirName){

    Dir_Entry * directory= malloc(block_Size);

    uint64_t pid=currentDirectory->memoryLocation;
    
    strcpy(directory->fileName,dirName);

    //Set type 1 for directory
    directory->typeOfFile=1;

    directory->directorySize=0;

    static const uint64_t tempMeta[32]={0};
    
    memcpy(directory->filesMeta,tempMeta,sizeof tempMeta);

    //Set the parent directory address to be the address of current working directory
    directory->parentDirectory=pid;

    uint64_t lbaPosition=findFreeMemory(bitMap,noOfBlocks,1);

    directory->memoryLocation=lbaPosition;

    uint64_t temp= currentDirectory->directorySize;

    currentDirectory->directorySize=temp+1;

    currentDirectory->filesMeta[temp]=lbaPosition;

    //Overwrite the parent directory with the new meta data
    LBAwrite(currentDirectory,1,pid);

    //Write directory entry to disk
    LBAwrite(directory,1,lbaPosition);

    // printf("This is the LBA position found by the find memory function for making the directory....:%lu\n",lbaPosition);

    //TODO Modify bit map 
    occupyMemoryBits(bitMap,1,lbaPosition);

    LBAwrite(bitMap,bitBlocks,1);

    free(directory);
    
}


void * changeDirectoryRoot(){
    
    uint64_t parentAd= currentDirectory->parentDirectory;
    if(parentAd==0){
        printf("The current directory is the root of the file system and there are no root nodes.\n");
        return 0;
    }

    Dir_Entry * tempDir= malloc(block_Size);
    LBAread(tempDir,1,parentAd);

    //free current directory and point it to root
    free(currentDirectory);

    currentDirectory=tempDir;

    tempDir=NULL;
}

void * changeDirectory(char * dirName){
    
    uint64_t * metaData=currentDirectory->filesMeta;

    for(int i=0;i<32;i++){
        uint64_t memoryLocation=metaData[i];
        // printf("This is the memory location returned %lu\n",memoryLocation);
        if(memoryLocation==0){
            // printf("No more files to display in this directory.\n");
            break;
        }
        Dir_Entry * tempDir= malloc(block_Size);
        LBAread(tempDir,1,memoryLocation);

        if(tempDir->typeOfFile==0){
            printf("%s is not a directory. Please enter a valid directory name.\n",dirName);
        }
        
        //check if the names are a match
        if(strcmp(tempDir->fileName,dirName)==0){
            //free the current directory buffer and realloc
            free(currentDirectory);
            
            //Current Directory now points to the new mathced directory
            currentDirectory= tempDir;

            tempDir=NULL;
            
            break;
        }

        free(tempDir);
    }


}

//TODO later to ensure that directories with the same name are not created
void * duplicateChecker(){

}


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




void * printVolInfo(){  
        printf("This is what is read from the read volume information\n The volume name: %s\n",v_Info->volumeName);
        printf("The volume size is: ");
        printf("%"PRIu64"\n", v_Info->volumeSize);
        printf("The LBA size is: ");
        printf("%"PRIu64"\n", v_Info->LBA_SIZE);
        printf("The root directoy is at: ");
        printf("%"PRIu64"\n", v_Info->rootDir);
        printf("The reading of vol_info is over...\n");    
}


void * listFiles(){
    uint64_t * metaData=currentDirectory->filesMeta;

    for(int i=0;i<32;i++){
        uint64_t memoryLocation=metaData[i];
        // printf("This is the memory location returned %lu\n",memoryLocation);
        if(memoryLocation==0){
            // printf("No more files to display in this directory.\n");
            break;
        }
        Dir_Entry * tempDir= malloc(block_Size);
        LBAread(tempDir,1,memoryLocation);
        
        //Printing file name
        printf("%s\n",tempDir->fileName);

        free(tempDir);
    }
}


void * getBitMap(){
        count(bitMap,0,noOfBlocks);  
}

void * freeBuffers(){
    free(v_Info);
    free(bitMap);
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
