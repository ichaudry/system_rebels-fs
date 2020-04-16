#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include "fsLow.h"
#include "fileSystem.h"
#include "fsHigh.h"



void * occupyMemoryBits(void * bitMap,unsigned long long startPosition, unsigned long long count);
void * freeMemoryBits(void * bitMap,unsigned long long startPosition, unsigned long long count );
void * count(void * bitMap,unsigned long long startPosition, unsigned long long count );


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
int startFileSystem(const char * volname, uint64_t * volSize, uint64_t * blockSize, int format){

    Volume_Information v_Info;

//    v_Info.volumeName=volname;
    strcpy(v_Info.volumeName,volname);
    v_Info.volumeSize=* volSize;
    v_Info.LBA_SIZE= * blockSize;



    //Initialize partition system and catch errors
    int ret= startPartitionSystem(v_Info.volumeName, &v_Info.volumeSize, &v_Info.LBA_SIZE);

    if(ret==0){
        printf("Success\n");

        //Write the volume information block and initialize the free list
        if(format==1){
            printf("Formatting the partition system.\n");

            uint64_t numberOfBlocks=v_Info.volumeSize/v_Info.LBA_SIZE;

            // printf("This is the number of blocks for the bitmap%"PRIu64"\n", numberOfBlocks);

            int bitMap[numberOfBlocks];

            // printf("The size of the freelist would be: %lu\n",sizeof(bitMap)/sizeof(int));

            //TO-DO Figure out how to use celing function and not hard code 1
            uint64_t bitMapSize= ((sizeof(bitMap)/sizeof(int))/v_Info.LBA_SIZE)+1;

            // printf("The lba size of the freelist would be: %llu\n",bitMapSize);

            v_Info.bitMapStart=1;

            v_Info.bitMapSize= bitMapSize;

            v_Info.rootDirectory=40;

            occupyMemoryBits(&bitMap,0,bitMapSize);

            freeMemoryBits(&bitMap,bitMapSize+1,numberOfBlocks);

            count(&bitMap,0,numberOfBlocks);

            LBAwrite(&v_Info,1,0);

            LBAwrite(&bitMap,bitMapSize,1);
        }

        else{
            printf("File is already formatted");

            //TODO Figure out why malloc is not working
            Volume_Information temp;

            LBAread(&temp,1,0);
            printf("This is what is read from the read volume information\n The volume name: %s\n",temp.volumeName);
            printf("The volume size is: ");
            printf("%"PRIu64"\n", temp.volumeSize);
            printf("The LBA size is: ");
            printf("%"PRIu64"\n", temp.LBA_SIZE);

            uint64_t numberOfBlocks=(*volSize)/(*blockSize);

            int bitMap[numberOfBlocks];

            uint64_t bitMapSize= ((sizeof(bitMap)/sizeof(int))/(*blockSize))+1;

            LBAread(&bitMap,bitMapSize,1);

            printf("This is a count check after reading back bit map\n" );

            count(&bitMap,0,numberOfBlocks);
            
        }




    }
    else if(ret==-1){
        printf("File exists but can not open for write\n");
    }
    else if(ret==-2){
        printf("Insufficient space for the volume");
    }


   

   
   

    closePartitionSystem();


    return 0;
}


// Test read functions
//  Volume_Information temp;
//         LBAread((void *)&temp,1,0);

//         printf("This is what is read from the read volume information\n The volume name: %s\n",temp.volumeName);
//         printf("The volume size is: ");
//         printf("%"PRIu64"\n", temp.volumeSize);
//         printf("The LBA size is: ");
//         printf("%"PRIu64"\n", temp.LBA_SIZE);
       
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

