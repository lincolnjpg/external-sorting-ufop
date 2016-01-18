#include <stdio.h>
#include <stdlib.h>

#include "externalQuicksort.h"

void createNewFile(short);

int main(void)
{
  balancedMerge();
  externalQuickSort();

  return 0;
}
