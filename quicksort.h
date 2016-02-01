#ifndef EXTERNALQUICKSORT_H
#define EXTERNALQUICKSORT_H

#include <stdio.h>

#include "ram.h"
#include "student.h"

typedef struct
{
  int delimiter, size;
} tPartition;

void externalQuickSort(int);
short readStudent(FILE *, tStudent *);
void insertionSort(tRAM *);
void sort(int, int, int, int, tRAM *, tPartition *, tPartition *, short);
void quickSort(int, int, int, int, tRAM *, tPartition *, tPartition *, short);

#endif
