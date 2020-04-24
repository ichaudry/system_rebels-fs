//
// Created by ibraheem on 4/23/20.
//

#ifndef SYSTEM_REBELS_FS_FILECONTROLUTIL_H
#define SYSTEM_REBELS_FS_FILECONTROLUTIL_H


void * writeFile(char * fileName,Dir_Entry * currentDirectory,int * bitMap,uint64_t bitMapSize,uint64_t blockSize,uint64_t noOfBlocks);

#endif //SYSTEM_REBELS_FS_FILECONTROLUTIL_H
