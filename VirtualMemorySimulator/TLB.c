/*
 * File:	TLB.c
 * Author:	Ron Zappardino
 * Purpose:	Simulates TLB hardware for quick lookup of frame numbers
 * Use:		Use with TLB.h, VMM.c, PMM.c, PMM.h, addresses.txt, BACKING_STORE.bin
 */

#include <stdio.h>
#include <stdlib.h>

#define TLBMaxSize 16 // Max number of entries TLB can hold

struct TLBLink* TLBhead;
struct TLBLink* TLBtail;
int TLBSize;
int totalTLBHits;

struct TLBLink {
	int pageNum;
	int frameNum;
	struct TLBLink* next;
	struct TLBLink* prev;
};

/* 
 * Helper function for findFrameNum, checks TLB for frame number and updates order if found
 * Input: int pageNum page number to search for
 * Output: frame number if found or -1 if not found
 */
int checkTLB(int pageNum) {
	struct TLBLink* pointer = TLBhead;

	while (pointer != NULL) {

		if (pointer->pageNum == pageNum) {
			int frameNum = pointer->frameNum;

			/* Move entry to head of list */
			if (pointer->prev != NULL) { // Check it's not already the head

					/* Remove entry from list */
				pointer->prev->next = pointer->next;
				if (pointer->next == NULL) { // Pointer is tail
					TLBtail = pointer->prev;
				} else {
					pointer->next->prev = pointer->prev;
				}

					/* Put entry at head of list */
				pointer->next = TLBhead;
				pointer->prev = NULL;
				TLBhead->prev = pointer;
				TLBhead = pointer;

			}

			totalTLBHits++;
			return frameNum;
		}

		pointer = pointer->next;
	}

	return -1; // Entry not found in TLB
}

/*
 * Helper function for findFrameNum, updates TLB with new page/frame entry
 * Input: int pageNum int frameNum corresponding page number and frame number for entry
 */
void updateTLB(int pageNum, int frameNum) {
	
	TLBSize++;
	if (TLBSize > TLBMaxSize) { // If TLB is now oversized, remove tail item
		struct TLBLink* toFree = TLBtail;
		TLBtail = TLBtail->prev;
		TLBtail->next = NULL;
		free(toFree);
		TLBSize--;
	}

	struct TLBLink* temp = malloc(sizeof(struct TLBLink));
	temp->pageNum = pageNum;
	temp->frameNum = frameNum;
	temp->prev = NULL;
	temp->next = TLBhead;

	if (TLBSize == 1) {
		TLBtail = temp;
	}

	if (TLBhead != NULL) {
		TLBhead->prev = temp;
	}
	TLBhead = temp;
}

int getTotalTLBHits() {
	return totalTLBHits;
}

void removeFromTLB(int pageNum) {
	struct TLBLink* pointer = TLBhead;

	while (pointer != NULL) {
		if (pointer->pageNum == pageNum) {

			if (pointer->prev == NULL) {
				if (pointer->next == NULL) {	// Link to remove is head and tail
					TLBhead = NULL;
					TLBtail = NULL;
				} else {						// Link to remove is just head
					TLBhead = pointer->next;
					pointer->next->prev = NULL;
				}
			} else if (pointer->next == NULL) {	// Link to remove is just tail
				TLBtail = pointer->prev;
				pointer->prev->next = NULL;
			} else {							// Link to remove is neither head nor tail
				pointer->next->prev = pointer->prev;
				pointer->prev->next = pointer->next;
			}

			free(pointer);
			TLBSize--;
			return;
		}

		pointer = pointer->next;
	}
}