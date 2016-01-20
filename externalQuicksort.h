#include "balancedHeap.h"

//teste
/*typedef struct
{
  //os campos start e end são usados apenas pelo método quicksort externo
  short size, start, end;
  tStudent student[17];
} tRAM;*/

typedef struct
{
  int delimiter, size;
} tPartition;

void externalQuickSort(void);
short readStudent(FILE *, tStudent *);
void insertionSort(tRAM *);
void sort(int, int, int, int, tRAM *, tPartition *, tPartition *, short);
void quickSort(int, int, int, int, tRAM *, tPartition *, tPartition *, short);
