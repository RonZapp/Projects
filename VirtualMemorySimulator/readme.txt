Project:	Virtual Memory Simulator
Author:		Ron Zappardino

Instructions:
	Compile:	gcc VMM.c PMM.c TLB.c -o vmm
	Run:		./vmm addresses.txt

Explanation:

This project is an attempt to simulate virtual memory with a demand-paging system. The BACKING_STORE.bin file acts as the hard drive or backing store that contains all the data. The VMM acts as a Virtual Memory Manager, which has 256 pages of virtual memory. The VMM reads in the addresses from addresses.txt, which represent logical addresses. It then attempts to find their location in physcial memory, which is managed by the PMM (physical memory manager) which starts off empty but loads frames from the backing store as needed.

There are 256 pages in the virtual memory space but only 128 frame in the physical memory space, so the PMM uses FIFO (First In, First Out) replacement to load in a new frame when it runs out of room. The VMM keeps track of each page number's associated frame numbers in the Page Table, while the PMM keeps track of the validity of these frame numbers (whether they have been overwritten or not). If the VMM requests an invalid frame number, (whether it is because it was overwritten or never was loaded into physical memory in the first place) the PMM loads the data into a new frame and returns this new frame number to the VMM. The VMM then uses this frame number, along with the offset, to find the value stored at the original address read in from addresses.txt

There is also a TLB, which is a limited 16-entry Page Table that allows for fast lookup of frame numbers. It is set up with LRU (Least Recently Used) replacement as this schema allows for a more succesful hit rate than FIFO. The VMM consults the TLB first, and then consults the Page Table only if it is not found in the TLB. In reality, the TLB is slower than the Page Table as the TLB is a linked list which takes longer to iterate through than the Page Table which is set up as an array. However, the TLB is always consulted first because outside of this situation, TLBs in actual hardware are set up to be much faster than PageTables.

The program takes the path of addresses.txt as its only parameter, and prints the results into results.txt. It prints on the commandline the total number of addresses evaluated, page faults, and succesfull TLB hits.