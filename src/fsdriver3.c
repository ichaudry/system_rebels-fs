#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "fsLow.h"
#include "fileSystem.h"
#include "fsHigh.h"

//Convert char to int64_t
int64_t S64(const char *s);

//Check if file exists
int cfileexists(const char * filename);

int main(int argc, char const *argv[])
{
    const char * filename;
    uint64_t volSize;
    uint64_t blockSize;
    
    if (argc == 4) {
        filename = argv[1];
        volSize= S64(argv[2]);
        blockSize =  S64(argv[3]);

        printf("These are the vlaues from stdin\n Vol name is: %s\n",filename);
        printf("vol size is %"PRIu64"\n",volSize);
        printf("block size is %"PRIu64"\n",blockSize);
    }
    else{
        printf("Invalid input. Please run with the following format: ./<executable> <fileName> <volSize> <blockSize>");
    }

    if(cfileexists(filename)==0){
        printf("The file doesn't exist format the file system.");
        startFileSystem(filename,&volSize,&blockSize,1);
    }
    else{
        printf("The file does exist no formating done\n");
        startFileSystem(filename,&volSize,&blockSize,0);
    }

    return 0;
}



/**
 * Helper Functions
 */

//https://stackoverflow.com/a/17003732
int64_t S64(const char *s) {
  int64_t i;
  char c ;
  int scanned = sscanf(s, "%" SCNd64 "%c", &i, &c);
  if (scanned == 1) return i;
  if (scanned > 1) {
    // TBD about extra data found
    return i;
    }
  // TBD failed to scan;  
  return 0;  
}


//https://www.zentut.com/c-tutorial/c-file-exists/
int cfileexists(const char * filename){
    /* try to open file to read */
    FILE *file;
    if ((file = fopen(filename, "r"))){
        fclose(file);
        return 1;
    }
    return 0;
}