#include "balancedHeap.h"

int sortedBlocksHeap(FILE **file, tTapeSet *ptrIn, tRAM *RAM, int n)
{
  short destTape, lastTape, readError, flagCount;
  int i, j, k, numBlocks;
  float lastGrade;
  tStudent student;

  //this variable stores the destination tape id
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

void mergeBlocksHeap(tTapeSet *ptrIn, tTapeSet *ptrOut, short idxIn,
                 short idxOut, tRAM *RAM, int numGroups, short lastMerge)
{
  char separator, enter1, enter2;
  short i, j, retorno;
  tStudent aux;

  cleanRAM(RAM);

  for (i = 0; i < TAPE_SET_SIZE; i++)
  {
    openTape(&(ptrIn->tape[i]), "r\0");
    createTape(&(ptrOut->tape[i]), i + idxOut);
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
