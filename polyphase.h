#ifndef POLYPHASE_H
#define POLYPHASE_H

#include <stdio.h>

#include "ram.h"
#include "tape.h"

int sortedBlocksPolyphase(FILE **, tTapeSet *, tRAM *, int);
int mergeBlocksPolyphase(tTapeSet *, short *, tRAM *, int, short);

#endif
