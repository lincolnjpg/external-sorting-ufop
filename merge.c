#include <stdlib.h>

#include "merge.h"
#include "constants.h"
#include "ram.h"
#include "tape.h"
#include "balanced_multiway_merge.h"
#include "balanced_multiway_merge_rep_sub.h"
#include "polyphase.h"

void merge(short method, int N)
{
  short posIn, posOut, posCtrl, idxIn, idxOut, idxCtrl, lastMerge, outTape,
        retorno;
  int i, blockSize, numBlocks, numGroups;
  tTapeSet tapeSet[2], tapeSetPolyphase, *ptrIn, *ptrOut;
  tRAM RAM;
  FILE *file;

  //open main input file
  file = fopen("PROVAO.txt", "r");

  //this variable will control swaps between in/out tapes
  posIn = idxIn = 0;
  posOut = posCtrl = 1;

  if (method == 1 || method == 2)
  {
    idxOut = idxCtrl = TAPE_SET_SIZE;

    //controlling the loops
    blockSize = RAM_SIZE;
    numBlocks = N / RAM_SIZE;

    if (N % RAM_SIZE > 0)
      numBlocks++;

    //create and initialize tapes
    createTape(&(tapeSet[posIn].tape), TAPE_SET_SIZE);
    createTape(&(tapeSet[posOut].tape), TAPE_SET_SIZE);

    for (i = 0; i < TAPE_SET_SIZE; i++)
      initializeTape(&(tapeSet[posIn].tape[i]), i);

    //create and initialize RAM
    createRAM(&RAM, RAM_SIZE);

    if (method == 1)    
      /*Intercalacao balanceada de varios caminhos - 2f fitas
        (Sem substituicao por selecao)*/      
      numBlocks = sortedBlocks_withoutRepSub(&file, tapeSet, &RAM, N);
    else    
      /*Intercalacao balanceada de varios caminhos - 2f fitas
        (Com substituicao por selecao)*/      
      numBlocks = sortedBlocks_withRepSub(&file, tapeSet, &RAM, N);

    for (i = 0; i < TAPE_SET_SIZE; i++)
      closeTape(&(tapeSet[posIn].tape[i]));

    fclose(file);

    while (numBlocks > 1)
    {
      //set pointers to tape sets
      ptrIn = &tapeSet[posIn];
      ptrOut = &tapeSet[posOut];

      //calcula número de grupos de blocos
      numGroups = numBlocks / TAPE_SET_SIZE;

      if (numBlocks % TAPE_SET_SIZE > 0)
        numGroups++;

      //verifica se a próxima intercalação será a última
      if (numGroups == 1)
        lastMerge = 1;
      else
        lastMerge = 0;

      if (method == 1)
        mergeBlocks_withoutRepSub(ptrIn, ptrOut, idxIn, idxOut, &RAM, blockSize,
                                  numGroups);
      else
        mergeBlocks_withRepSub(ptrIn, ptrOut, idxIn, idxOut, &RAM, numGroups,
                               lastMerge);

      posIn += posCtrl;
      posCtrl *= -1;
      posOut += posCtrl;

      idxIn += idxCtrl;
      idxCtrl *= -1;
      idxOut += idxCtrl;

      blockSize *= TAPE_SET_SIZE;

      numBlocks = numGroups;
    }
  }
  else
  {
    /*First output tape is the last one*/
    outTape = POLYPHASE_TAPE_SET_SIZE - 1;

    //create and initialize tapes
    createTape(&(tapeSetPolyphase.tape), POLYPHASE_TAPE_SET_SIZE);

    for (i = 0; i < POLYPHASE_TAPE_SET_SIZE; i++)
      initializeTape(&(tapeSetPolyphase.tape[i]), i);

    //create and initialize RAM
    createRAM(&RAM, POLYPHASE_TAPE_SET_SIZE - 1);

    /*Intercalacao polifásica - f + 1 fitas
      (Com substituicao por selecao)*/
    numBlocks = sortedBlocksPolyphase(&file, &tapeSetPolyphase, &RAM, N);

    for (i = 0; i < POLYPHASE_TAPE_SET_SIZE; i++)
      closeTape(&(tapeSetPolyphase.tape[i]));

    /*Abre todas as fitas, no modo leitura*/
    for (i = 0; i < POLYPHASE_TAPE_SET_SIZE; i++)
      openTape(&(tapeSetPolyphase.tape[i]), "r\0");

    /*Recria nova fita de saída*/
    closeTape(&(tapeSetPolyphase.tape[outTape]));
    initializeTape(&(tapeSetPolyphase.tape[outTape]), outTape);

    retorno = numBlocks;

    while (retorno > 1)
    {
      retorno = mergeBlocksPolyphase(&tapeSetPolyphase, &outTape, &RAM,
                                       numBlocks, 0);
    }

    for (i = 0; i < POLYPHASE_TAPE_SET_SIZE; i++)
      closeTape(&(tapeSetPolyphase.tape[i]));
  }
}
