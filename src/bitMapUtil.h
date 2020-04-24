#ifndef BIT_MAP_UTIL
#define BIT_MAP_UTIL

//A=array
//k=element in array
#define SetBit(A,k)     ( A[(k/32)] |= (1 << (k%32)) )
#define ClearBit(A,k)   ( A[(k/32)] &= ~(1 << (k%32)) )
#define TestBit(A,k)    ( A[(k/32)] & (1 << (k%32)) )


void * occupyMemoryBits(int * bitMap,uint64_t noOfBlocks,unsigned long long startPosition, unsigned long long count);
void * freeMemoryBits(int * bitMap,uint64_t noOfBlocks,unsigned long long startPosition, unsigned long long count );
void * count(int * bitMap,unsigned long long startPosition, unsigned long long count );
uint64_t findFreeMemory(int * bitMap,uint64_t noOfBlocks, unsigned long long count);

#endif