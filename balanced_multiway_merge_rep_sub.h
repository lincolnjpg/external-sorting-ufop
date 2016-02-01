#ifndef BALANCEDHEAP_H
#define BALANCEDHEAP_H

#include <stdio.h>

#include "tape.h"
#include "ram.h"

int  sortedBlocks_withRepSub(FILE **, tTapeSet *, tRAM *RAM, int);
void mergeBlocks_withRepSub(tTapeSet *, tTapeSet *, short, short, tRAM *, int,
                            short);

#endif
