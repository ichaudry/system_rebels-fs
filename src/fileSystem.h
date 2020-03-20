//System Rebels File System Directory
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#ifndef SYSTEM_REBELS_DIRECTORY_STRUCT_HEADER_FILE
#define SYSTEM_REBELS_DIRECTORY_STRUCT_HEADER_FILE

/**
 * This structure defines the componenets of a directory entry in the system rebels filesystem 
 */
typedef struct {
	char *fileName;
	char *fileAuthor;
	char *typeOfFile;
    
    //9 bit values represent read - write - execute permission for user - group - root
    //e.g 000 000 111 means only the root user has permissions to read write and execute
    //101 111 111 means user has read and execute permission and group and root has all all permissions 
	long filePermissions; 

    //The format of the date is month.day.year
    //e.g 04271996 is april 27 1996
	long dateCreated;
	long dateModified;

	unsigned long int directorySize;

    //Stores the head LBA of a file or directory
	unsigned long int memoryLocation;			
} Dir_Entry;

/**
 * This structure defines the volume information of this filesystem 
 */
typedef struct {
	//The size of a file system volume 
	long long volumeSize;

	//Pointer to the first free memory block the free memory linked list
	void * freeMemory;

	//Pointer to the root directory location 
	void * rootDirectory;

	//Unique ID to identify a particular volume 
	long volumeID;

	//Name for a volume in the filesystem 
	char * volumeName;

	//Dedicated LBA_SIZE for the filesystem
	size_t LBA_SIZE;	

} Volume_Information;


/**
 * This structure is a linked list that is initialized by our file system to keep track of memory that shares some property.
 * It can be used to create a linked list for free memory
 * It can also be used to create a linked list for used memory
 */
typedef struct Node{
	//The ID of the memory block
	long LBA_ID;

	//how many consecutive LBAs are free. If none this value is 1
	long LBA_chunk; 

	//Linking to next free memory block
	struct Node * next; 

}Node;


/**
 * This structure is initializes and stores references to our free memory linked list
 */
typedef struct{
	//head for the free memory linkedlist 
	Node * freeMemory_Head;

	//tail for the free memory linkedlist
	Node * freeMemory_Tail;

	//how much free memory is present in our filesystem
	long long freeMemory;
}Free_Space_Tracker;


#endif



/**
 * This is a main function that you can put in main.c to test this .h file 
 * to ensure that it compiles without errors. 
 */
// #include <stdio.h>
// #include <stdlib.h>
// #include "fileSystem.h"


// #define BUFFER_SIZE 1000000000

// int main(int argc, char const *argv[])
// {
//     long * LBA_POINTER= malloc(sizeof(BUFFER_SIZE));


//     Dir_Entry *Dir_Entry= malloc(sizeof(Dir_Entry));
//     Dir_Entry->fileName="systemRebels";

//     Volume_Information *Volume_Information= malloc(sizeof(Volume_Information));
//     Volume_Information->volumeSize= (long long ) BUFFER_SIZE;
//     Volume_Information->rootDirectory=(void *)LBA_POINTER[0];
//     Volume_Information->LBA_SIZE=32*sizeof(char);

//     Free_Space_Tracker Free_Space_Tracker;
//     Free_Space_Tracker.freeMemory_Head=malloc(sizeof(Node));
//     Node * head= Free_Space_Tracker.freeMemory_Head;
//     Free_Space_Tracker.freeMemory_Head->LBA_ID=1232342315;
//     Free_Space_Tracker.freeMemory_Head->LBA_chunk=2;
//     Free_Space_Tracker.freeMemory=131254321;
//     Free_Space_Tracker.freeMemory_Tail=malloc(sizeof(Node));
//     Node * tail=Free_Space_Tracker.freeMemory_Tail;
//     Free_Space_Tracker.freeMemory_Tail->LBA_chunk=12;
//     Free_Space_Tracker.freeMemory_Tail->LBA_ID=213451235;
//     Node * test= malloc(sizeof(Node));
//     test->LBA_ID=120;
//     head->next=test;


//     printf("The file name is %s\n",Dir_Entry->fileName);

//     printf("The volume size is %lld\n",Volume_Information->volumeSize);

//     printf("The LBA size is %lu bytes\n",Volume_Information->LBA_SIZE);
    
//     printf("The ID of this chunk is %ld\n",Free_Space_Tracker.freeMemory_Head->LBA_ID);

//     printf("The size of this chunk is %ld\n",Free_Space_Tracker.freeMemory_Head->LBA_chunk);

//     printf("The next from the head has this id %ld",head->next->LBA_ID);


//     free(LBA_POINTER);
//     free(Dir_Entry);
//     free(Volume_Information);
//     free(head);
//     free(tail);
//     free(test);


//     return 0;
// }
