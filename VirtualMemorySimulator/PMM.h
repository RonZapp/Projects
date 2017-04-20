extern int checkPageTable(int pageNum);

extern int fixPageFault(int pageNum);

extern int readPhysicalMemory(int address);

extern int getTotalPageFaults();

extern int checkValid(int pageNum);