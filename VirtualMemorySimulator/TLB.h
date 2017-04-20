extern int checkTLB(int pageNum);

extern void updateTLB(int pageNum, int frameNum);

extern int getTotalTLBHits();

extern void removeFromTLB(int pageNum);