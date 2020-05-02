/**
 *bitMapUtil.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include "bitMapUtil.h"

uint64_t findFreeMemory(int * bitMap,uint64_t noOfBlocks ,unsigned long long count){
    uint64_t lbaPosition;
    unsigned long long counter=0;
    //Sets to 1 if memory is found. 
    int memoryFound=0;

    for(unsigned long long i =0;i<noOfBlocks;i++){
        lbaPosition=i;
        // printf("The current LBA position being tested for free space head is: %lu\n",lbaPosition);
        for(unsigned long long j= i; j<noOfBlocks;j++){
            if(TestBit(bitMap,i)){
                break;
            }
            else{
                counter++;
            }
            if(counter==count){
                // printf("The memory of length %llu exists and the starting lba block is %lu\n",count,lbaPosition);
                memoryFound=1;
                break;
            } 
        }
        if(counter==count){
            printf("The memory of length %llu exists and the starting lba block is %lu\n",count,lbaPosition);
            break;
        }
    }

    if(memoryFound==1){
        return lbaPosition;
    }
    else if(memoryFound==0){
        printf("There is not enough space to do the write please free up some space.\n");
        return (unsigned long long )NULL;
    }
}


void * occupyMemoryBits(int * bitMap,uint64_t noOfBlocks,unsigned long long startPosition, unsigned long long count){
    unsigned long long counter=0;

    //TODO make this a while loop
    for (unsigned long long i = startPosition; i <= noOfBlocks; i++){
        SetBit(bitMap, i);
        counter+=1;
        if(counter==count){
            printf("%llu blocks occupied in bitmap starting from position %llu.\n",count,startPosition);
            break;
        }
    }
    return NULL;
}

void * freeMemoryBits(int * bitMap,uint64_t noOfBlocks, unsigned long long startPosition, unsigned long long count ){
    unsigned long long counter=0;

    //TODO make this while loops
    for (unsigned long long i = startPosition; i <=noOfBlocks ; i++){
        ClearBit(bitMap,i);
        counter+=1;
        if(counter==count){
            printf("%llu blocks cleared in bitmap starting from position %llu.\n",count,startPosition);
            break;
        }
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