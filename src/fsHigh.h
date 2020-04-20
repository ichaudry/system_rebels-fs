#ifndef FS_HIGH
#define FS_HIGH

int startFileSystem(char * volName, uint64_t * volSize, uint64_t * blockSize,int format);
void * printVolInfo();
void * getBitMap();
void * freeBuffers();

void * writeFile();
void * readFile();
void * moveFile();
void * copyFile();
void * writeDirectory();
void * printDirectory();

#endif