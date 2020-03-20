#include <stdio.h>
#include <stdlib.h>
#include "fileSystem.h"


#define BUFFER_SIZE 1000000000

int main(int argc, char const *argv[])
{
    long * LBA_POINTER= malloc(sizeof(BUFFER_SIZE));


    Dir_Entry *Dir_Entry= malloc(sizeof(Dir_Entry));
    Dir_Entry->fileName="systemRebels";

    Volume_Information *Volume_Information= malloc(sizeof(Volume_Information));
    Volume_Information->volumeSize= (long long ) BUFFER_SIZE;
    Volume_Information->rootDirectory=(void *)LBA_POINTER[0];
    Volume_Information->LBA_SIZE=32*sizeof(char);

    Free_Space_Tracker Free_Space_Tracker;
    Free_Space_Tracker.freeMemory_Head=malloc(sizeof(Node));
    Node * head= Free_Space_Tracker.freeMemory_Head;
    Free_Space_Tracker.freeMemory_Head->LBA_ID=1232342315;
    Free_Space_Tracker.freeMemory_Head->LBA_chunk=2;
    Free_Space_Tracker.freeMemory=131254321;
    Free_Space_Tracker.freeMemory_Tail=malloc(sizeof(Node));
    Node * tail=Free_Space_Tracker.freeMemory_Tail;
    Free_Space_Tracker.freeMemory_Tail->LBA_chunk=12;
    Free_Space_Tracker.freeMemory_Tail->LBA_ID=213451235;
    Node * test= malloc(sizeof(Node));
    test->LBA_ID=120;
    head->next=test;


    printf("The file name is %s\n",Dir_Entry->fileName);

    printf("The volume size is %lld\n",Volume_Information->volumeSize);

    printf("The LBA size is %lu bytes\n",Volume_Information->LBA_SIZE);
    
    printf("The ID of this chunk is %ld\n",Free_Space_Tracker.freeMemory_Head->LBA_ID);

    printf("The size of this chunk is %ld\n",Free_Space_Tracker.freeMemory_Head->LBA_chunk);

    printf("The next from the head has this id %ld",head->next->LBA_ID);


    free(LBA_POINTER);
    free(Dir_Entry);
    free(Volume_Information);
    free(head);
    free(tail);
    free(test);


    return 0;
}
