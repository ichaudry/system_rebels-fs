#ifndef FS_HIGH
#define FS_HIGH


int startFileSystem(char * volName, uint64_t * volSize, uint64_t * blockSize,int format);
void * printVolInfo();
void * occupyMemoryBits(void * bitMap,unsigned long long startPosition, unsigned long long count);
void * freeMemoryBits(void * bitMap,unsigned long long startPosition, unsigned long long count );
void * count(void * bitMap,unsigned long long startPosition, unsigned long long count );


#endif