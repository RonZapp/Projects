/*
 * File:	virMemMan.c
 * Author:	Ron Zappardino
 * Purpose: Simulates a Virtual Memory Manager for demand-paging with a TLB for quick lookup
 * Use:		Use with PMM.c, PMM.h, TLB.c, TLB.h, addresses.txt, BACKING_STORE.bin
 * 			Detailed instructions in virMemMan_design_doc.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include "TLB.h"
#include "PMM.h"

#define numAddresses 1000 // Number of addresses to be read from addresses.txt
#define totalNumberPages 256 // Number of pages in virtual memory


/* Function declarations */
int checkTLB(int pageNum);
void updateTLB(int pageNum, int frameNum);
int getTotalTLBHits();

int fixPageFault(int pageNum);
int readPhysicalMemory(int address);
int getTotalPageFaults();
int checkValid(int pageNum);


/* Global variables */
int pageTable[totalNumberPages];
int totalPageFaults;

/*
 * Reads addresses.txt and populates array with logical addresses from file
 * Params: int argc and char* argv[] are the commandline parameters from the main function
 * Param: nested array of size numAddresses * 2 for storing addresses
 * At the end of the function, each entry in the array will be a page number / page offset pair
 * stored in logicalAddresses[][0] and logicaAddresses[][1] respectively
 */
void txtParser(int argc, char* argv[], int logicalAddresses[][2]) {
	if (argc != 2) {
		printf("Error: Invalid number of arguments\n");
		exit(EXIT_FAILURE); 
	}

	char* addressesFile = argv[1]; // Read path from first commandline parameter

	// Open file at path
	FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(addressesFile, "r");
    if (fp == NULL) {
    	printf("Error: Could not read file at parameter\n");
        exit(EXIT_FAILURE); 
    }

    int i = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
    	int fullAddress = atoi(line); // Convert line to int

    		/* Calculate and store page number */
    	int pageNumMask = 0x0000FF00;
    	int pageNum = fullAddress & pageNumMask;
    	pageNum = pageNum >> 8;
    	logicalAddresses[i][0] = pageNum;

    		/* Calculate and store page offset */
    	int pageOffMask = 0x000000FF;
    	int pageOff = fullAddress & pageOffMask;
    	logicalAddresses[i][1] = pageOff;


    	i++;
    }

    if (i != numAddresses) {
    	printf("Error: Could only read %d of %d lines in addresses.txt\n", i, numAddresses);
    	exit(EXIT_FAILURE); 
    }

    fclose(fp);
    if (line) {
        free(line);
    }
}


/*
 * Finds and returns frame number from physical memory
 * input: int pageNum page number from virtual memory
 */
int findFrameNum(int pageNum) {
	int frameNum = checkTLB(pageNum);
	if (frameNum != -1) { // Was able to find frame number in TLB
		return frameNum;
	}


	int valid = checkValid(pageNum); // Check if page has a frame loaded in physical memory
	if (!valid) { // page fault
		pageTable[pageNum] = fixPageFault(pageNum);
	}

	frameNum = pageTable[pageNum];
	updateTLB(pageNum, frameNum);

	return frameNum;
}



int main(int argc, char* argv[]) {
		/* Initialize page table */
	for (int i = 0; i < totalNumberPages; i++) {
		pageTable[i] = -1; 
	}

	int logicalAddresses[numAddresses][2]; // For each address, [0] is page number and [1] is offset
	txtParser(argc, argv, logicalAddresses);

	FILE *fp;
	fp = fopen("results.txt", "w");

	for (int i = 0; i < numAddresses; i++) {
		
		int pageNum = logicalAddresses[i][0];
		int pageOff = logicalAddresses[i][1];
		int frameNum = findFrameNum(pageNum);
		int physAddress = (256 * frameNum) + pageOff;
		int logicAddress = (256 * pageNum) + pageOff;

		int value = readPhysicalMemory(physAddress);

		char str[80];
		sprintf(str, "Virtual address: %d Physical address: %d Value: %d\n", 
			logicAddress, physAddress, value);
		fputs(str, fp);
	}

	fclose(fp);

	int totalTLBHits = getTotalTLBHits();
	int totalPageFaults = getTotalPageFaults();

	printf("Out of 1000 memory requests, there were %d TLB hits and %d page faults.\n",
		totalTLBHits, totalPageFaults);

    exit(EXIT_SUCCESS);
}