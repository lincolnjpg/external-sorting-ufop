#include <stdio.h>
#include <stdlib.h>

#include "externalQuicksort.h"

void createNewFile(short);

int main(void)
{

  printf("%d\n", FOPEN_MAX);
  //balancedMerge();
  externalQuickSort();


  return 0;
}
