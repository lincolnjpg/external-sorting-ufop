#ifndef BALANCED_H
#define BALANCED_H

#include <stdio.h>

#include "tape.h"
#include "ram.h"

void balancedMerge();
int sortedBlocks_withoutRepSub(FILE **, tTapeSet *, tRAM *, int);
void mergeBlocks_withoutRepSub(tTapeSet *, tTapeSet *, short, short, tRAM *,
                               short, int);

#endif

