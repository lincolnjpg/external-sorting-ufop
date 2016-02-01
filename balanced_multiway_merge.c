//acho que é melhor usar TAPE_SET_SIZE ao invés de RAM_SIZ nas continhas de bloco e grupo abaixo...

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "balanced_multiway_merge.h"
#include "constants.h"
#include "file.h"

int sortedBlocks_withoutRepSub(FILE **file, tTapeSet *ptrIn, tRAM *RAM, int n)
{
  short destTape;
  int i, j, numBlocks;
  tStudent student;

  //this variable stores the destination tape id
  i = destTape = numBlocks = 0;

  //generate and insert sorted blocks into input tapes  
  while (loadFromFile(file, &student) == 0 && i < n)
  {
    //tStudent loaded from file
    if (getSize(RAM) < RAM_SIZE)
      insertRAM(RAM, student, getSize(RAM));
    else
    {
      //sort data
      sortRAM(RAM, RAM_SIZE);
      //put each sorted block into an input tape
      for (j = 0; j < RAM->size; j++)
        insertTape(&(ptrIn->tape[destTape]), getStudent(RAM, j));

      //now, RAM can stores RAM_SIZE more students
      cleanRAM(RAM);
      //insert latest student into the RAM
      insertRAM(RAM, student, getSize(RAM));
      //variable that controls the destination tape
      destTape++;
      numBlocks++;
      //reset destination tape control
      if (destTape == TAPE_SET_SIZE)
        destTape = 0;
    }

    i++;
  }

  if (getSize(RAM) > 0)
    numBlocks++;

  for (i = 0; i < getSize(RAM); i++)
    insertTape(&(ptrIn->tape[destTape]), getStudent(RAM, i));

  return numBlocks;
}

void mergeBlocks_withoutRepSub(tTapeSet *ptrIn, tTapeSet *ptrOut, short idxIn,
                               short idxOut, tRAM *RAM, short blockSize,
                               int numGroups)
{
  short i, j;
  tStudent aux;

  cleanRAM(RAM);

  for (i = 0; i < TAPE_SET_SIZE; i++)
  {
    openTape(&(ptrIn->tape[i]), "r\0");
    initializeTape(&(ptrOut->tape[i]), i + idxOut);
  }

  j = 0;

  while (numGroups > 0)
  {    
    for (i = 0; i < TAPE_SET_SIZE; i++)
      if (ptrIn->tape[i].status == 1 && getTapeLength(&(ptrIn->tape[i])) > 0)
      {
        //tape is active
        if (readTape(&(ptrIn->tape[i]), &aux) == 0)
        {
          //put student into RAM
          insertRAM(RAM, aux, getSize(RAM));

          //talvez não seja necessário fazer isso aqui, já que é feito logo abaixo tb

          //lembrar que a cada linha escrita, um \n tb é escrito. então
          //a cada linha, o ponteiro é um tStudent + 1... complexo...

          if (ptrIn->tape[i].numReads == blockSize ||
              getTapeLength(&(ptrIn->tape[i])) == 0)
          {
            //tape is now inactive
            setStatus(&(ptrIn->tape[i]), 0);
            setNumReads(&(ptrIn->tape[i]), 0);
          }
        }
      }

    i = 0;

    while (getSize(RAM) > 0)
    {
      //sort RAM, according to its size
      sortRAM(RAM, RAM_SIZE);
      //get student from RAM
      aux = getStudent(RAM, 0);
      //remove student from RAM
      removeStudent(RAM, 0);
      //insert first student into an output tape
      insertTape(&(ptrOut->tape[j]), aux);

      if (ptrIn->tape[aux.origin - idxIn].status == 1)
      {
        //get the next student from same tape
        if (readTape(&(ptrIn->tape[aux.origin - idxIn]), &aux) == 0)
          insertRAM(RAM, aux, 0);

        if (ptrIn->tape[aux.origin - idxIn].numReads == blockSize ||
            getTapeLength(&(ptrIn->tape[aux.origin - idxIn])) == 0)
        {
          //tape is now inactive
          setStatus(&(ptrIn->tape[aux.origin - idxIn]), 0);
          setNumReads(&(ptrIn->tape[aux.origin - idxIn]), 0);
        }
      }

      i++;
    }

    for (i = 0; i < TAPE_SET_SIZE; i++)
      if (getTapeLength(&(ptrIn->tape[i])) > 0)
        setStatus(&(ptrIn->tape[i]), 1);

    numGroups--;
    j++;

    if (j == TAPE_SET_SIZE)
      j = 0;
  }

  for (i = 0; i < TAPE_SET_SIZE; i++)
  {
    closeTape(&(ptrIn->tape[i]));
    closeTape(&(ptrOut->tape[i]));
  }
}
