#ifndef FS_HIGH
#define FS_HIGH

//A=array
//k=element in array
#define SetBit(A,k)     ( A[(k/32)] |= (1 << (k%32)) )
#define ClearBit(A,k)   ( A[(k/32)] &= ~(1 << (k%32)) )
#define TestBit(A,k)    ( A[(k/32)] & (1 << (k%32)) )

int startFileSystem(char * volName, uint64_t * volSize, uint64_t * blockSize,int format);
void * printVolInfo();
void * getBitMap();
void * occupyMemoryBits(int * bitMap,unsigned long long startPosition, unsigned long long count);
void * freeMemoryBits(int * bitMap,unsigned long long startPosition, unsigned long long count );
void * count(int * bitMap,unsigned long long startPosition, unsigned long long count );
uint64_t findFreeMemory(int * bitMap, unsigned long long count);

void * writeFile();
void * readFile();
void * moveFile();
void * copyFile();
void * writeDirectory();

#endif