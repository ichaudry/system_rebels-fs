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
#include "dirEntryControlUtil.h"




void * copyFile(char * fileName,char * newFileName)
{
    /**
     * The copy file function takes the following
     * 1: The file to be moved
     * 2: Name of the new copied version of file
     * 3: The new directory where to move the file / This could be the current directory of the root
     *
     * Steps:
     * -Check if there is space in the directory to copy a file into it
     * -Find memory in the bitmap on where to copy file to
     * -
     * -
     */

}

void * moveFile(char * fileName,char * dirName)
{
    //Get directory entry of file to be moved
    Dir_Entry * file= findFile(fileName);

    if(!file){
        return NULL;
    }

    //Get directory entry of directory where file needs to moved.
    Dir_Entry * directory= findDirectory(dirName);

    if(!directory){
        return NULL;
    }

    writeFileDirectoryEntry(file->fileName,directory,file->lba_blocks,file->memoryLocation);

    removeDirectoryEntry(fileName);

    printf("Move Complete.\n");

}
