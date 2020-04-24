//
// Created by ibraheem on 4/20/20.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include "fsLow.h"
#include "bitMapUtil.h"
#include "fileSystem.h"
#include "fsHigh.h"
#include "directoryControlUtil.h"
#include "fileControlUtil.h"




void * writeFile(char * fileName,Dir_Entry * currentDirectory,int * bitMap,uint64_t bitMapSize,uint64_t blockSize,uint64_t noOfBlocks){

    if(duplicateChecker(fileName,currentDirectory,blockSize)==0){
        char *buffer;
        size_t bufsize = 0;
        size_t characters;
        printf("Begin writing to file \"%s\": ",fileName);
        characters = getline(&buffer,&bufsize,stdin);
        printf("%zu characters were read.\n",characters);
        printf("You typed: '%s'\n",buffer);
        printf("The size of the input is %lu\n",sizeof(buffer));
        printf("The size of the input checked from bufsize is %lu\n",bufsize);

        free(buffer);
        

    }
}