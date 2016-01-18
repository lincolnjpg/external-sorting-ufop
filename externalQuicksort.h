#include "balancedHeap.h"

typedef struct
{
  int delimiter, size;
} tPartition;

void externalQuickSort(void);
void createFileQuick();
void insertionSort(tRAM *);
void sort(int, int, int, int, tRAM *, tPartition *, tPartition *);
void quickSort(int, int, int, int, tRAM *, tPartition *, tPartition *);
void test(tRAM *);
