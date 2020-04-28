#ifndef FS_HIGH
#define FS_HIGH

int startFileSystem(char * volName, uint64_t * volSize, uint64_t * blockSize,int format);
void * vinfo();
void * getBitMap();
void * freeBuffers();


void * fsWriteFile(char * fileName);
void * readFile();
void * moveFile();
void * copyFile();

void * ls();
void * mkdir(char * dirName);
void * pwd();
void * cdRoot();
void * cd(char * dirName);
void * rmdir(char * dirName);
void * fsCopyFromLinux();

#endif