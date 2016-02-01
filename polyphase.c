#include <stdlib.h>
#include <time.h>

#include "file.h"
#include "heap.h"
#include "polyphase.h"
#include "constants.h"

int sortedBlocksPolyphase(FILE **file, tTapeSet *inputTapes, tRAM *RAM, int n)
{
  short destTape, lastTape, readError, flagCount;
  int i, j, k, numBlocks;
  float lastGrade;
  tStudent student;

  readError = flagCount = numBlocks = 0;

  for (i = 0; i < POLYPHASE_TAPE_SET_SIZE - 1; i++)
  {
    loadFromFile(file, &student);
    insertRAM(RAM, student, getSize(RAM));
    setPriority(RAM, getSize(RAM) - 1, 0);
  }

  buildHeap(RAM, POLYPHASE_TAPE_SET_SIZE - 1);

  k = 0;

  srand(time(NULL));
  destTape = rand() % (POLYPHASE_TAPE_SET_SIZE - 1);

  do
  {
    //obtém o estudante com menor nota
    student = getStudent(RAM, 0);

    if (i >= n)
    {
      //troca primeiro com o último
      swapStudents(RAM, 0, POLYPHASE_TAPE_SET_SIZE - 1 - (k + 1));
      //retira estudante da RAM
      removeStudent(RAM, POLYPHASE_TAPE_SET_SIZE - 1 - (k + 1));

      k++;
    }
    else
      //remove primeiro estudante da RAM
      removeStudent(RAM, 0);

    //insere menor estudante (primeiro do heap) na fita apropriada
    insertTape(&(inputTapes->tape[destTape]), student);
    //armazena última nota escrita na fita
    lastGrade = student.grade;

    if (i < n)
      //lê próximo item de dado
      readError = loadFromFile(file, &student);

    if (readError == 0 || i >= n)
    {
      if (i < n)
      {
        //insere item lido na RAM
        insertRAM(RAM, student, 0);

        //define a prioridade do estudante recém adicionado à RAM
        if (getGrade(RAM, 0) > lastGrade)
        {
          setPriority(RAM, 0, 1);

          flagCount++;
        }
        else
          setPriority(RAM, 0, 0);

        if (flagCount == POLYPHASE_TAPE_SET_SIZE - 1)
        {
          //insere separador de bloco
          fprintf(inputTapes->tape[destTape].file, "%c\n", '@');

          destTape = rand() % (POLYPHASE_TAPE_SET_SIZE - 1);

          flagCount = 0;
          numBlocks++;

          for (j = 0; j < POLYPHASE_TAPE_SET_SIZE - 1; j++)
            setPriority(RAM, j, 0);
        }
        else
          //insere separador de linha
          fprintf(inputTapes->tape[destTape].file, "%c\n", ';');

        //reconstrói heap, considerando o tamanho máximo da RAM
        rebuildHeap(RAM, POLYPHASE_TAPE_SET_SIZE - 1);
      }
      else
      {
        //reconstrói heap, considerando o tamanho atual da RAM
        rebuildHeap(RAM, getSize(RAM));

        //verifica a prioridade do novo menor elemento
        if (getStudent(RAM, 0).priority == 1)
        {
          if (destTape != lastTape)
            //se já mudou de fita, usa ';' como separador
            fprintf(inputTapes->tape[destTape].file, "%c\n", ';');
          else
            //se ainda não mudou de fita, usa '@' como separador
            fprintf(inputTapes->tape[destTape].file, "%c\n", '@');

          //só deve mudar de fita uma vez nesse ponto
          //aqui, a nova fita de destino não pode mudar, por isso mantive a mesma regra de intercalação balanceada
          destTape = lastTape + 1;

          if (destTape == POLYPHASE_TAPE_SET_SIZE - 1)
            destTape = 0;
        }
        else
          fprintf(inputTapes->tape[destTape].file, "%c\n", ';');
      }

      i++;

      //armazena o id da fita em uso quando a leitura do arquivo terminou
      if (i == n)
        lastTape = destTape;
    }   
  } while (readError == 0 && getSize(RAM) > 0);

  fseek(inputTapes->tape[destTape].file, -2, 1);
  fprintf(inputTapes->tape[destTape].file, "%c\n", '@');

  /*contabiliza bloco formado por itens com prioridade igual a 0 que ainda
  estavam na RAM após a leitura do último item de dado*/
  numBlocks++;

  /*se, após a leitura do último item de dado, ainda existir, na RAM, algum item
  com prioridade igual a 1, tal item será enviado para um novo bloco*/
  if (destTape != lastTape)
    numBlocks++;

  if (readError == 0)
    return numBlocks;
  else
    return -1;
}

int mergeBlocksPolyphase(tTapeSet *ptrIn, short *outTape, tRAM *RAM,
                          int numBlocks, short lastMerge)
{
  char separator, enter1, enter2;
  short i, j, retorno, newEmptyTape, mergeAgain;
  int tapeLength;
  tStudent aux;

  /*Antes do inicio do processo de intercalacao, apenas a fita de saída, cujo
  id foi recebido como parâmetro, está vazia.*/
  /*Indica o id da próxima fita de saída*/
  newEmptyTape = *outTape;    

  cleanRAM(RAM); 

  j = 0;

  while (newEmptyTape == *outTape)
  {
    /*Indica se, após o processo de intercalacao, ainda existe alguma fita que não
    está vazia*/
    mergeAgain = 0;

    for (i = 0; i < POLYPHASE_TAPE_SET_SIZE; i++)
    {
      tapeLength = getTapeLength(&(ptrIn->tape[i]));

      if (ptrIn->tape[i].status == 1 && tapeLength > 0 && i != *outTape)
      {
        //tape is active
        if (readTape(&(ptrIn->tape[i]), &aux) == 0)
        {
          //put student into RAM
          insertRAM(RAM, aux, getSize(RAM));

          //lê separador
          retorno = fscanf(ptrIn->tape[i].file, "%[\n] %c %[\n]",
                           &enter1, &separator, &enter2);

          tapeLength = getTapeLength(&(ptrIn->tape[i]));

          if (separator == '@' || tapeLength == 0)
          {
            //tape is now inactive
            setStatus(&(ptrIn->tape[i]), 0);
            setNumReads(&(ptrIn->tape[i]), 0);

            //indica que um bloco foi completamente lido
            //numBlocks--;

            //if (tapeLength == 0)
              //newEmptyTape = i;
          }
        }
      }
    }

    i = 0;

    while (getSize(RAM) > 0)
    {
      //sort RAM, according to its size
      sortRAM(RAM, POLYPHASE_TAPE_SET_SIZE - 1);
      //get student from RAM
      aux = getStudent(RAM, 0);
      //remove student from RAM
      removeStudent(RAM, 0);
      //insert first student into an output tape
      insertTape(&(ptrIn->tape[newEmptyTape]), aux);

      if (lastMerge == 0)
        //insere separador
        fprintf(ptrIn->tape[newEmptyTape].file, "%c\n", ';');

      if (ptrIn->tape[aux.origin].status == 1)
      {
        //get the next student from same tape
        if (readTape(&(ptrIn->tape[aux.origin]), &aux) == 0)
        {
          insertRAM(RAM, aux, 0);
          //lê separador
          retorno = fscanf(ptrIn->tape[aux.origin].file,
                           "%[\n] %c %[\n]",
                           &enter1, &separator, &enter2);
        }

        tapeLength = getTapeLength(&(ptrIn->tape[aux.origin]));

        if (separator == '@' || tapeLength == 0)
        {
          //tape is now inactive
          setStatus(&(ptrIn->tape[aux.origin]), 0);
          setNumReads(&(ptrIn->tape[aux.origin]), 0);

          //indica que um bloco foi completamente lido
          //numBlocks--;

          //if (tapeLength == 0)
            //newEmptyTape = aux.origin;
        }
      }

      i++;
    }

    if (lastMerge == 0)
    {
      fseek(ptrIn->tape[newEmptyTape].file, -2, 1);
      fprintf(ptrIn->tape[newEmptyTape].file, "%c\n", '@');
    }   

    for (i = 0; i < POLYPHASE_TAPE_SET_SIZE; i++)
      if (getTapeLength(&(ptrIn->tape[i])) > 0)
      {
        setStatus(&(ptrIn->tape[i]), 1);

        mergeAgain++;
      }
      else
      {
        newEmptyTape = i;
      }
  }

  //rolando um segfault na hora de fechar a fita 19
  //(deve ser o mesmo problema da funcao sorted -
  //faltou ajustar o POLYPHASE_TAPE... para -1

  //for (i = 0; i < POLYPHASE_TAPE_SET_SIZE; i++)
    //closeTape(&(ptrIn->tape[i]));

  /*Fecha antiga fita de saída*/
  closeTape(&(ptrIn->tape[*outTape]));
  /*Abre fita de saída, no modo "leitura"*/
  openTape(&(ptrIn->tape[*outTape]), "r\0");
  /*Atualiza fita de saída*/
  *outTape = newEmptyTape;
  /*Recria nova fita de saída*/
  closeTape(&(ptrIn->tape[*outTape]));
  initializeTape(&(ptrIn->tape[*outTape]), *outTape);

  return mergeAgain;
}
