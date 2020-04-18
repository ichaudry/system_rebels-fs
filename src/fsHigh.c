#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include "fsLow.h"
#include "fileSystem.h"
#include "fsHigh.h"


uint64_t vol_Size;
uint64_t block_Size;
uint64_t noOfBlocks;

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


    //Initialize partition system and catch errors
    int ret= startPartitionSystem(volName,volSize, blockSize);

    if(ret==0){
        printf("Success\n");

        //Format the partition with VCB, bit map, and root directory
        if(format==1){
            printf("Formatting the partition system.\n");

            Volume_Information * v_Info=malloc(*blockSize);

            strcpy(v_Info->volumeName,volName);
            v_Info->volumeSize=* volSize;
            v_Info->LBA_SIZE= * blockSize;

            //Write the volume information block
            LBAwrite(v_Info,1,0);


            
            printf("The number of blocks in our volume are:%lu\n",noOfBlocks);
            
            uint64_t arraySize= noOfBlocks/(uint64_t)(sizeof(int)*8)+1;
            
            printf("The arraySize would be:%lu\n",arraySize);

            uint64_t bitBlocks= ((arraySize * sizeof(int))/(uint64_t)block_Size)+1;

            printf("The number of bitblocks needed are:%lu\n",bitBlocks);

            int * bitMap=malloc(bitBlocks*block_Size);
     
            //Initialize bitMap to all zeroes 
            for (int i = 0; i < arraySize; i++ ){
                bitMap[i] = 0;  
            }

            //Mark dirty bits for VCB and bit map
            occupyMemoryBits(bitMap,0,bitBlocks); 
        
            LBAwrite(bitMap,bitBlocks,1);


            free(bitMap);
            free(v_Info);
        }
        else{
            printf("File is already formatted\n"); 
            getBitMap();
            // writeDirectory();

            writeFile();
            readFile();
            moveFile();
            copyFile();

            

        } 

        //testing find free memory block function
        int tBitMap[20];

        for (int i = 0; i < 20; i++ ){
                tBitMap[i] = 0;  
            }

        occupyMemoryBits(tBitMap,0,7);

        for(int i= 0; i < 20 ; i++){
            if(TestBit(tBitMap,i)){
                printf("The bit %d is set to one.\n",i);
            }
        }
        uint64_t lbaPosition;
        unsigned long long counter=0;
        unsigned long long count=5;


        for(unsigned long long i =0;i<20;i++){
            lbaPosition=i;
            printf("The current LBA position being tested for free space head is: %lu\n",lbaPosition);
            for(unsigned long long j= i; j<20;j++){
                if(TestBit(tBitMap,i)){
                    break;
                }
                else{
                    counter++;
                }
                if(counter==count){
                printf("The memory of length %llu exists and the starting lba block is %lu\n",count,lbaPosition);
                break;
            }
            
        }
        // findFreeMemory(tBitMap,5);


        if(counter==count){
                printf("The memory of length %llu exists and the starting lba block is %lu\n",count,lbaPosition);
                break;
            }





    }
    }
    else if(ret==-1){
        printf("File exists but can not open for write\n");
    }
    else if(ret==-2){
        printf("Insufficient space for the volume");
    }

    return 0;
}



void * writeRootDirectory(){
    printf("The size of the directory entry is: %lu\n",sizeof(Dir_Entry));
    Dir_Entry * rootDirectory= malloc(block_Size);

   
    strcpy(rootDirectory->fileName,"root");
    rootDirectory->typeOfFile=1;

    // rootDirectory->

  
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

void * printVolInfo(){
        Volume_Information * v_Info=malloc(block_Size);

        LBAread(v_Info,1,0);
        printf("This is what is read from the read volume information\n The volume name: %s\n",v_Info->volumeName);
        printf("The volume size is: ");
        printf("%"PRIu64"\n", v_Info->volumeSize);
        printf("The LBA size is: ");
        printf("%"PRIu64"\n", v_Info->LBA_SIZE);

        printf("The reading of vol_info is over...\n");

        free(v_Info);
        
}

void * getBitMap(){
       uint64_t noOfBlocks=vol_Size/block_Size;

        printf("The number of blocks in our volume are:%lu\n",noOfBlocks);
        
        uint64_t arraySize= noOfBlocks/(uint64_t)(sizeof(int)*8)+1;
        
        printf("The arraySize would be:%lu\n",arraySize);

        uint64_t bitBlocks= ((arraySize * sizeof(int))/(uint64_t)block_Size)+1;

        printf("The number of bitblocks needed are:%lu\n",bitBlocks);

        int * bitMap=malloc(bitBlocks*block_Size);
        

        LBAread(bitMap,bitBlocks,1);

        count(bitMap,0,noOfBlocks);
        
        
        free(bitMap);
        
}


uint64_t findFreeMemory(int * bitMap, unsigned long long count){
    uint64_t lbaPosition;
    unsigned long long counter=0;



    for(unsigned long long i =0;i<noOfBlocks;i++){
        lbaPosition=i;
        for(unsigned long long j= i; j<count;j++){
            if(TestBit(bitMap,i)){
                break;
            }
            else{
                counter++;
            }
        }
        if(counter==count){
        printf("The memory of length %llu exists and the starting lba block is %lu",count,lbaPosition);
        break;
        }
    }

    return lbaPosition;
}


void * occupyMemoryBits(int * bitMap,unsigned long long startPosition, unsigned long long count){

      for (unsigned long long i = startPosition; i <= count; i++){
                SetBit(bitMap, i);
            }

        return NULL;

}

void * freeMemoryBits(int * bitMap,unsigned long long startPosition, unsigned long long count ){
  
       for (unsigned long long i = startPosition; i <= count; i++){
                ClearBit(bitMap,i);
            }

            return NULL;
}

void * count(int * bitMap,unsigned long long startPosition, unsigned long long count ){
        int zeroes=0;
        int ones=0;
       for (unsigned long long i = startPosition; i <= count; i++){

                if(TestBit(bitMap,i)){
                    ones++;
                }

                if(! TestBit(bitMap,i)){
                    zeroes++;
                }
                
            }
        printf("The number of zero bit blocks are %d \nThe number of one bit blocks are %d\n",zeroes,ones);
            return NULL;
}

