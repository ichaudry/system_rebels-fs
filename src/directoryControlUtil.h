//
// Created by ibraheem on 4/20/20.
//

#ifndef SYSTEM_REBELS_FS_DIRECTORYCONTROLUTIL_H
#define SYSTEM_REBELS_FS_DIRECTORYCONTROLUTIL_H

void * printMeta(uint64_t * metaData);
void * writeDirectory(char * dirName,Dir_Entry * currentDirectory,int * bitMap,uint64_t bitMapSize,uint64_t blockSize,uint64_t noOfBlocks);
void * removeDirectory(char * dirName,Dir_Entry * currentDirectory,int * bitMap,uint64_t bitMapSize,uint64_t blockSize,uint64_t noOfBlocks);
void * writeFile(char * fileName,Dir_Entry * currentDirectory,int * bitMap,uint64_t bitMapSize,uint64_t blockSize,uint64_t noOfBlocks)
Dir_Entry  * changeDirectoryRoot(Dir_Entry * currentDirectory,uint64_t blockSize);
Dir_Entry * changeDirectory(char * dirName,Dir_Entry * currentDirectory, uint64_t blockSize);
int duplicateChecker();
void * printCurrentDirectory(Dir_Entry * currentDirectory);
void * printDirectory(Dir_Entry * directory);
void * printVolInfo(Volume_Information * vInfo);
void * listFiles(Dir_Entry * currentDirectory, uint64_t blockSize);



#endif //SYSTEM_REBELS_FS_DIRECTORYCONTROLUTIL_H
