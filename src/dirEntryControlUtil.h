//
// Created by ibraheem on 4/20/20.
//

#ifndef SYSTEM_REBELS_FS_DIRENTRYCONTROLUTIL_H
#define SYSTEM_REBELS_FS_DIRENTRYCONTROLUTIL_H


typedef struct{
    
    //Need a boolean to check if whole file has been processed?

    uint64_t pointer;   //Current postion of pointer in memory either for read or write
    uint64_t fileSize;
    uint64_t memoryLocation;
    uint64_t dirEntryLocation;
    
    char * fileBuffer;

    //Need to keep track of last LBA we wrote a chunk of file to?

} openFileEntry;


void * printMeta(uint64_t * metaData);
void * writeDirectory(char * dirName);
void * writeFileDirectoryEntry(char * fileName,uint64_t blocksNeeded,uint64_t lbaStart);
void * removeDirectory(char * dirName);

openFileEntry * fileOpen(char * fileName,char * condition);
void * fileClose(openFileEntry * fd);
void * readFile(openFileEntry * fd, char * buffer, uint64_t length);



Dir_Entry  * changeDirectoryRoot();
Dir_Entry * changeDirectory(char * dirName);
int duplicateChecker();
void * printCurrentDirectory();
void * printDirectory(Dir_Entry * directory);
void * printVolInfo();
void * listFiles();
void * copyFromLinux(char * linuxFileName, char * srfsFileName);
void * removeFile(char * fileName);
void * removeDirectoryEntry(char * dirName);
Dir_Entry * findFile(char * fileName);




#endif //SYSTEM_REBELS_FS_DIRENTRYCONTROLUTIL_H
