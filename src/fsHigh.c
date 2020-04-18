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


            uint64_t noOfBlocks=vol_Size/block_Size;

            printf("The number of blocks in our volume are:%lu\n",noOfBlocks);
            
            uint64_t arraySize= noOfBlocks/(uint64_t)(sizeof(int)*8)+1;
            
            printf("The arraySize would be:%lu\n",arraySize);

            uint64_t bitBlocks= ((arraySize * sizeof(int))/(uint64_t)block_Size)+1;

            printf("The number of bitblocks needed are:%lu\n",bitBlocks);

            int * bitMap=malloc(bitBlocks*block_Size);
            // int bitMap[arraySize];


            //Initialize bitMap to all zeroes 
            for (int i = 0; i < arraySize; i++ ){
                bitMap[i] = 0;  
            }

            // occupyMemoryBits(bitMap,0,50); 
            SetBit( bitMap, 0 ); 
            SetBit( bitMap, 1 );
            SetBit( bitMap, 2 );
            SetBit( bitMap, 3 );
            SetBit( bitMap, 4 );               // Set 3 bits
            SetBit( bitMap, 5 );
            SetBit( bitMap, 6 );
            SetBit( bitMap, 7 );               // Set 3 bits
            SetBit( bitMap, 8 );
            SetBit( bitMap, 9 );
            SetBit( bitMap, 10 );               // Set 3 bits
            SetBit( bitMap, 11 );
            SetBit( bitMap, 12 );
            
            LBAwrite(bitMap,bitBlocks,1);


            free(bitMap);
            free(v_Info);
        }
        else{
            printf("File is already formatted\n"); 
            getBitMap();
            

            writeFile();
            readFile();
            moveFile();
            copyFile();

            

        } 

        //








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

        for (int i = 0; i <= 2000; i++ ) {
            if (TestBit(bitMap, i) ){
                printf("Bit %d was set !\n", i);
            }
        }
        
        free(bitMap);
        
}


void * occupyMemoryBits(int * bitMap,unsigned long long startPosition, unsigned long long count){
      
      int * pBitMap= bitMap;

      for (unsigned long long i = startPosition; i <= count; i++){
                SetBit(pBitMap, i);
            }

        return NULL;

}

void * freeMemoryBits(int * bitMap,unsigned long long startPosition, unsigned long long count ){
        int * pBitMap= (int *) bitMap;
        
       for (unsigned long long i = startPosition; i <= count; i++){
                ClearBit(pBitMap,i);
            }

            return NULL;
}

void * count(int * bitMap,unsigned long long startPosition, unsigned long long count ){
        int * pBitMap= (int *) bitMap;
        int zeroes=0;
        int ones=0;
       for (unsigned long long i = startPosition; i <= count; i++){
                // printf("%d",pBitMap[i]);

                if(TestBit(pBitMap,i)==1){
                    ones++;
                }

                if(TestBit(pBitMap,i)==0){
                    zeroes++;
                }
                
            }
        printf("The number of zero bit blocks are %d \nThe number of one bit blocks are %d\n",zeroes,ones);
            return NULL;
}

