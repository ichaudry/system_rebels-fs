#ifndef FS_HIGH
#define FS_HIGH

#include "fileSystem.h"

uint64_t volumeSize;
uint64_t blckSize;
uint64_t noOfBlocks;
uint64_t bitMapSize;
uint64_t arraySize;
Volume_Information * vInfo;
int * bitMap;

//Current active directory
Dir_Entry * currentDirectory;


int startFileSystem(char * volName, uint64_t * volSize, uint64_t * blockSize,int format);
void * vinfo();
void * getBitMap();
void * freeBuffers();


void * fsReadFile(char * fileName,uint64_t length);
void * readFile();
void * moveFile();
void * copyFile();

void * ls();
void * mkdir(char * dirName);
void * pwd();
void * cdRoot();
void * cd(char * dirName);
void * rmdir(char * dirName);
void * rm(char * fileName);
void * fsCopyFromLinux(char * linuxFileName, char * srfsFileName);

#endif