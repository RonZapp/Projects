/*
 * File:	physMem.c
 * Author:	Ron Zappardino
 * Purpose:	Simulates Physical memory and manager with FIFO replacement
 * Use:		Use with PMM.h, VMM.c, TLB.c, TLB.h, addresses.txt, and BACKING_STORE.h
 *			Detailed instructions in virMemMan_design_doc.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include "TLB.h"

#define totalNumberPages 256 // Max number of pages in virtual memory space
#define totalNumberFrames 256 // Max number of frames in physical memory space
#define physicalMemorySize 65536 // Number of bytes in physical memory
								 // should be totalNumberFrames * 256 (256 = page size)

void removeFromTLB(int pageNum);

char physicalMemory[physicalMemorySize];
int physMemPointer; // Points to the frame to be written
int totalPageFaults;
int pageValidTable[totalNumberPages] = {0}; // 1 if page has a valid frame in memory, 0 if not
int frameToPageTable[totalNumberFrames]; // For looking up page number by frame number (reverse PageTable)
int initialized = 0; // 1 if frameToPageTable has been initialized, 0 if not


/*
 * Fixes pagefault by loading page from BACKING_STORE into physical memory,
 * then updating page table and TLB
 * Input: int pageNum
 */
int fixPageFault(int pageNum) {
		/* initialize frameToPageTable if not already initialized */
	if (!initialized) { 
		for (int i = 0; i < totalNumberFrames; i++) {
			frameToPageTable[i] = -1;
		}
		initialized = 1;
	}

	totalPageFaults++;

	int frameNum = physMemPointer / 256;
	int overwritePage = frameToPageTable[frameNum]; // Page that is being overwrriten by FIFO
	if (overwritePage != -1) { // If page is -1 then this frame didn't have a page assigned 
		pageValidTable[overwritePage] = 0;
		removeFromTLB(pageNum);
	}

	FILE *fp;
	int offset = pageNum * 256;
	fp = fopen("BACKING_STORE.bin", "rb");
	char buffer[5];
	fseek(fp, offset, SEEK_SET);

	for (int i = 0; i < 256; i++) {
		fread(buffer, 1, 1, fp);
		physicalMemory[physMemPointer] = buffer[0];
		physMemPointer++;
	}
	fclose(fp);

	if (physMemPointer >= physicalMemorySize) { // Ran out of room in physical memory
		physMemPointer = 0; // Will begin overwriting pages in FIFO order
	}

	pageValidTable[pageNum] = 1; // This page has a frame in memory
	frameToPageTable[frameNum] = pageNum;

	return frameNum;
}


/*
 * Returns value in physical memory at address
 */
int readPhysicalMemory(int address) {
	return physicalMemory[address];
}


/*
 * Returns number of page faults that have occurred
 */
int getTotalPageFaults() {
	return totalPageFaults;
}


/*
 * Checks if the page's corresponding frame is in the physical memory
 */
int checkValid(int pageNum) {
	return pageValidTable[pageNum];
}