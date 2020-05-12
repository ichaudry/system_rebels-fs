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
	//File name
	char fileName [32];

	//This is 1 for directory and 0 for file
	uint64_t typeOfFile;
	
	//Number of directory entries in a directory
	uint64_t directorySize;

	//blocks for file 
	uint64_t lba_blocks;

	//Stores the head of the file parent directory if it exists.
	uint64_t parentDirectory;

	//LBAs of inodes for files in a directory
	uint64_t filesMeta [32];

    //Stores the head LBA of a file or directory
	uint64_t memoryLocation;	
    

	//Everything below here is priority 2
	char fileAuthor [32];

    //9 bit values represent read - write - execute permission for user - group - root
    //e.g 000 000 111 means only the root user has permissions to read write and execute
    //101 111 111 means user has read and execute permission and group and root has all all permissions 
	uint64_t filePermissions; 

    //The format of the date is month.day.year
    //e.g 04271996 is april 27 1996
	uint64_t dateCreated;
	uint64_t dateModified;

			
} Dir_Entry;


//Start here 
//Set the first LBA

/**
 * This structure defines the volume information of this filesystem 
 * 
 */
typedef struct {
	
	//Name for a volume in the filesystem 
	char volumeName[32];

	//The size of a file system volume 
	uint64_t volumeSize;
	
	//Blokc size of each LBA
	uint64_t LBA_SIZE;

	//THis will be an lba block
	//Use unint 64
	uint64_t bitMapStart;

	//Size of bitmap
	uint64_t bitMapSize;

	//LBA of the rootDirectory
	uint64_t rootDir;

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
