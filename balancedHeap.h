#include "balanced.h"

int  sortedBlocksHeap(FILE **, tTapeSet *, tRAM *RAM, int);
void buildHeap(tRAM *, int);
void rebuildHeap(tRAM *, int);
void mergeBlocksHeap(tTapeSet *, tTapeSet *, short, short, tRAM *, int, short);

