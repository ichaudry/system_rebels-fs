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


            free(v_Info);
        }
        else{
            printf("File is already formatted\n"); 
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
     
       
void * occupyMemoryBits(void * bitMap,unsigned long long startPosition, unsigned long long count){
      
      int * pBitMap= (int *) bitMap;

      for (unsigned long long i = startPosition; i <= count; i++){
                pBitMap[i]=1;
            }

        return NULL;

}

void * freeMemoryBits(void * bitMap,unsigned long long startPosition, unsigned long long count ){
        int * pBitMap= (int *) bitMap;
       for (unsigned long long i = startPosition; i <= count; i++){
                pBitMap[i]=0;
            }

            return NULL;
}

void * count(void * bitMap,unsigned long long startPosition, unsigned long long count ){
        int * pBitMap= (int *) bitMap;
        int zeroes=0;
        int ones=0;
       for (unsigned long long i = startPosition; i <= count; i++){
                // printf("%d",pBitMap[i]);
                if(pBitMap[i]==0){
                    zeroes++;
                }
                 if(pBitMap[i]==1){
                   ones++;
                }
            }
        printf("The number of zero bit blocks are %d \nThe number of one bit blocks are %d\n",zeroes,ones);
            return NULL;
}

