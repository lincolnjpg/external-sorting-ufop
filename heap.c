#include "heap.h"

void buildHeap(tRAM *RAM, int size)
{
  int i, idxMinor, idxParent, idxTarget, idxLChild, idxRChild, swaps;
  tStudent aux;

  for (i = 0; i < size / 2;)
  {
    //left child index
    idxLChild = 2 * i + 1;
    //right child index
    idxRChild = 2 * i + 2;
    swaps = 0;

    if (idxLChild >= 0 && idxLChild < size &&
        idxRChild >= 0 && idxRChild < size)
    {
      //children's indexes are inside array
      if (RAM->student[idxLChild].grade < RAM->student[idxRChild].grade)
        //right child is minor than left child
        idxMinor = idxRChild;
      else
        //left child is minor than right child
        idxMinor = idxLChild;
    }
    else
      //right child index is beyond array's bounds
      idxMinor = idxLChild;

    if (RAM->student[i].grade < RAM->student[idxMinor].grade)
    {
      //"parent-minor child" swap
      aux = RAM->student[i];
      RAM->student[i] = RAM->student[idxMinor];
      RAM->student[idxMinor] = aux;

      //check for a backward swap (new a[i] is minor than its parent)
      idxTarget = i;
      idxParent = (idxTarget - 1) / 2;

      while (RAM->student[idxTarget].grade > RAM->student[idxParent].grade)
      {
        //parent node is greater
        aux = RAM->student[idxTarget];
        RAM->student[idxTarget] = RAM->student[idxParent];
        RAM->student[idxParent] = aux;

        //target index has changed
        idxTarget = idxParent;
        idxParent = (idxTarget - 1) / 2;

        //controls i variable increment
        swaps++;
      }
    }

    if (swaps == 0)
      //i must not be incremented if a[i] has changed (while statement above)
      i++;
  }
}

void rebuildHeap(tRAM *RAM, int size)
{
  int idxNewRoot, idxMinor, idxLChild, idxRChild;
  tStudent aux;

  idxNewRoot = 0;

  while (2 * idxNewRoot + 1 < size)
  {
    idxLChild = 2 * idxNewRoot + 1;
    idxRChild = 2 * idxNewRoot + 2;

    if (idxLChild >= 0 && idxLChild < size &&
        idxRChild >= 0 && idxRChild < size)
    {
      //children's indexes are inside array

      if (RAM->student[idxLChild].priority > RAM->student[idxRChild].priority)
        //right child's priority is less than left child's priority
        idxMinor = idxRChild;
      else if (RAM->student[idxLChild].priority < RAM->student[idxRChild].priority)
        //left child's priority is less than right child's priority
        idxMinor = idxLChild;
      else if (RAM->student[idxLChild].grade < RAM->student[idxRChild].grade)
        //prioridades iguais (guarda o índice do estudante com a menor nota)
        idxMinor = idxRChild;
      else
        //prioridades iguais (guarda o índice do estudante com a menor nota)
        idxMinor = idxLChild;
    }
    else
      //right child index is beyond array's bounds
      idxMinor = idxLChild;

    //compares with parent (idxNewRoot)
    if (RAM->student[idxMinor].grade < RAM->student[idxNewRoot].grade &&
        RAM->student[idxNewRoot].priority == 0)
      break;

    if (RAM->student[idxMinor].priority < RAM->student[idxNewRoot].priority ||
        (RAM->student[idxMinor].grade > RAM->student[idxNewRoot].grade &&
         RAM->student[idxMinor].priority == RAM->student[idxNewRoot].priority))
    {
      aux = RAM->student[idxNewRoot];
      RAM->student[idxNewRoot] = RAM->student[idxMinor];
      RAM->student[idxMinor] = aux;

      idxNewRoot = idxMinor;
    }
    else
      idxNewRoot++;
  }
}
