#include "file.h"
#include "heap.h"
#include "balanced_multiway_merge_rep_sub.h"
#include "constants.h"

int sortedBlocks_withRepSub(FILE **file, tTapeSet *ptrIn, tRAM *RAM, int n)
{
  short destTape, lastTape, readError, flagCount;
  int i, j, k, numBlocks;
  float lastGrade;
  tStudent student;

  destTape = readError = flagCount = numBlocks = 0;

  for (i = 0; i < RAM_SIZE; i++)
  {
    loadFromFile(file, &student);    
    insertRAM(RAM, student, getSize(RAM));
    setPriority(RAM, getSize(RAM) - 1, 0);
  }

  buildHeap(RAM, RAM_SIZE);

  //talvez seja bom fazer removeStudent() já retornar o estudante removido
  //talvez com isso seja possível retirar a função getStudent()...

  k = 0;

  do
  {
    //obtém o estudante com menor nota
    student = getStudent(RAM, 0);

    if (i >= n)
    {
      //troca primeiro com o último
      swapStudents(RAM, 0, RAM_SIZE - (k + 1));
      //retira estudante da RAM
      removeStudent(RAM, RAM_SIZE - (k + 1));

      k++;
    }
    else
      //remove primeiro estudante da RAM
      removeStudent(RAM, 0);

    //insere menor estudante (primeiro do heap) na fita apropriada
    insertTape(&(ptrIn->tape[destTape]), student);
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

        if (flagCount == RAM_SIZE)
        {
          //insere separador de bloco
          fprintf(ptrIn->tape[destTape].file, "%c\n", '@');

          flagCount = 0;
          destTape++;
          numBlocks++;

          if (destTape == RAM_SIZE)
            destTape = 0;

          for (j = 0; j < RAM_SIZE; j++)
            setPriority(RAM, j, 0);
        }
        else
          //insere separador de linha
          fprintf(ptrIn->tape[destTape].file, "%c\n", ';');

        //reconstrói heap, considerando o tamanho máximo da RAM
        rebuildHeap(RAM, RAM_SIZE);
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
            fprintf(ptrIn->tape[destTape].file, "%c\n", ';');
          else
            //se ainda não mudou de fita, usa '@' como separador
            fprintf(ptrIn->tape[destTape].file, "%c\n", '@');

          //só deve mudar de fita uma vez nesse ponto
          destTape = lastTape + 1;

          if (destTape == TAPE_SET_SIZE)
            destTape = 0;
        }
        else
          fprintf(ptrIn->tape[destTape].file, "%c\n", ';');
      }

      i++;

      //armazena o id da fita em uso quando a leitura do arquivo terminou
      if (i == n)
        lastTape = destTape;
    }
  } while (/*i < n &&*/ readError == 0 && getSize(RAM) > 0);

  fseek(ptrIn->tape[destTape].file, -2, 1);
  fprintf(ptrIn->tape[destTape].file, "%c\n", '@');

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

void mergeBlocks_withRepSub(tTapeSet *ptrIn, tTapeSet *ptrOut, short idxIn,
                            short idxOut, tRAM *RAM, int numGroups,
                            short lastMerge)
{
  char separator, enter1, enter2;
  short i, j, retorno;
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

          //lê separador
          retorno = fscanf(ptrIn->tape[i].file, "%[\n] %c %[\n]",
                           &enter1, &separator, &enter2);

          //talvez não seja necessário fazer isso aqui, já que é feito logo abaixo tb

          //lembrar que a cada linha escrita, um \n tb é escrito. então
          //a cada linha, o ponteiro é um tStudent + 1... complexo...

          if (separator == '@' || getTapeLength(&(ptrIn->tape[i])) == 0)
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

      if (lastMerge == 0)
        //insere separador
        fprintf(ptrOut->tape[j].file, "%c\n", ';');

      if (ptrIn->tape[aux.origin - idxIn].status == 1)
      {
        //get the next student from same tape
        if (readTape(&(ptrIn->tape[aux.origin - idxIn]), &aux) == 0)
        {
          insertRAM(RAM, aux, 0);
          //lê separador
          retorno = fscanf(ptrIn->tape[aux.origin - idxIn].file,
                           "%[\n] %c %[\n]",
                           &enter1, &separator, &enter2);
        }

        if (separator == '@' ||
            getTapeLength(&(ptrIn->tape[aux.origin - idxIn])) == 0)
        {
          //tape is now inactive
          setStatus(&(ptrIn->tape[aux.origin - idxIn]), 0);
          setNumReads(&(ptrIn->tape[aux.origin - idxIn]), 0);
        }
      }

      i++;
    }

    if (lastMerge == 0)
    {
      fseek(ptrOut->tape[j].file, -2, 1);
      fprintf(ptrOut->tape[j].file, "%c\n", '@');
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
