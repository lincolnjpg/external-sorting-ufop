#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

#include "constants.h"
#include "file.h"
#include "quicksort.h"

void externalQuickSort(int N)
{
  short RAM_SIZE_QS;  
  tPartition p1, p2;
  tRAM RAM;  
  struct timeval t, t2;
  double tTotal;

  //define tamanho da RAM (log(N) na base 2)
  RAM_SIZE_QS = log2(N);

  if (RAM_SIZE_QS < 3)
    RAM_SIZE_QS = 3;

  //cria e inicia RAM
  createRAM(&RAM, RAM_SIZE_QS);

  gettimeofday(&t, NULL);

  sort(0, N - 1, 0, N - 1, &RAM, &p1, &p2, RAM_SIZE_QS);

  gettimeofday(&t2, NULL);

  tTotal = (t2.tv_sec + t2.tv_usec / 1000000.0) - (t.tv_sec + t.tv_usec / 1000000.0);
}

void insertionSort(tRAM *RAM)
{
  short i, j;
  tStudent aux;

  for (i = RAM->start + 1; i < RAM->end; i++)
  {
    aux = RAM->student[i];
    j = i;

    //decrescente
    while (aux.grade > RAM->student[j - 1].grade && j > 0)
    {
      RAM->student[j] = RAM->student[j - 1];
      j--;
    }

    RAM->student[j] = aux;
  }
}

void quickSort(int p1_R, int p2_R, int p1_W, int p2_W,
               tRAM *RAM, tPartition *p1, tPartition *p2,
               short RAM_SIZE_QS)
{
  short nextRead, insPoint;
  int i;
  long p1_NextRead, p2_NextRead;
  float inferiorLimit, superiorLimit;
  tStudent auxItem;
  FILE *p1_Read, *p2_Read, *p1_Write, *p2_Write;

  cleanRAM(RAM);

  p1_Read = fopen("PROVAO.txt", "r");
  p1_Write = fopen("PROVAO.txt", "r+");
  p2_Read = fopen("PROVAO.txt", "r");
  p2_Write = fopen("PROVAO.txt", "r+");

  /*lembrar que, no caso dos arquivos texto, além do tamanho em bytes de cada
  estudante, tb há um \n*/

  //sizeof(tStudent)
  fseek(p1_Read, p1_R * LINE_SIZE, 0);
  fseek(p1_Write, p1_W * LINE_SIZE, 0);
  fseek(p2_Read, p2_R * LINE_SIZE, 0);
  fseek(p2_Write, p2_W * LINE_SIZE, 0);

  inferiorLimit = FLT_MIN;
  superiorLimit = FLT_MAX;

  //get readers position2
  p1_NextRead = ftell(p1_Read);
  p2_NextRead = ftell(p2_Read);

  RAM->size = RAM->start = RAM->end = p1->size = p2->size =
  p1->delimiter = p2->delimiter = 0;
  nextRead = 2; //first read will be p2
  /*when reading the third item of the file, it will be put in
  the array's last position*/
  insPoint = RAM_SIZE_QS - 1;

  while (p1_NextRead <= p2_NextRead)
  {
    if (nextRead == 1)
    {
      //fread(&auxItem, sizeof(tStudent), 1, p1_Read);
      loadFromFile(&p1_Read, &auxItem);
      p1_NextRead = ftell(p1_Read);

      nextRead = 2;
    }
    else
    {
      //fread(&auxItem, sizeof(tStudent), 1, p2_Read);
      loadFromFile(&p2_Read, &auxItem);
      //fseek(p2_Read, sizeof(tStudent) * -2, 1);
      //sizeof(tStudent)
      fseek(p2_Read, LINE_SIZE * -2, 1);
      p2_NextRead = ftell(p2_Read);

      nextRead = 1;
    }

    if (RAM->size == RAM_SIZE_QS - 1)
    {
      if (auxItem.grade > inferiorLimit && auxItem.grade < superiorLimit)
      {
        if (insPoint == 0)
          //insertion point: begining
          RAM->start = insPoint;
        else
          //insPoint == RAM_SIZE_QS - 1 (insertion point: end)
        //if (RAM->end <= RAM_SIZE_QS)
          (RAM->end)++;

        RAM->student[insPoint] = auxItem;
        (RAM->size)++;

        //sort RAM
        insertionSort(&(*RAM));

        //remove item (min or max)
        if (p1->size < p2->size)
        {
          //fwrite(&(RAM->student[insPoint]), sizeof(tStudent), 1, p1_Write);
          fprintf(p1_Write, "%08d %05.1f %s %-50s %-30s\n",
                  RAM->student[0].id,
                  RAM->student[0].grade,
                  RAM->student[0].state,
                  RAM->student[0].city,
                  RAM->student[0].course);

          superiorLimit = RAM->student[0].grade;
          (p1->size)++;
          RAM->start = 1;
          RAM->end = RAM_SIZE_QS;
          insPoint = 0;
        }
        else
        {
          //fwrite(&(RAM->student[insPoint]), sizeof(tStudent), 1, p2_Write);
          fprintf(p2_Write, "%08d %05.1f %s %-50s %-30s\n",
                  RAM->student[RAM_SIZE_QS - 1].id,
                  RAM->student[RAM_SIZE_QS - 1].grade,
                  RAM->student[RAM_SIZE_QS - 1].state,
                  RAM->student[RAM_SIZE_QS - 1].city,
                  RAM->student[RAM_SIZE_QS - 1].course);
          //fseek(p2_Write, sizeof(tStudent) * -2, 1);
          //sizeof(tStudent)
          fseek(p2_Write, LINE_SIZE * -2, 1);

          inferiorLimit = RAM->student[RAM_SIZE_QS - 1].grade;
          (p2->size)++;
          RAM->start = 0;
          RAM->end = RAM_SIZE_QS - 1;
          insPoint = RAM_SIZE_QS - 1;
        }

        (RAM->size)--;       
      }
      else if (auxItem.grade >= superiorLimit)
      {
        //fwrite(&auxItem, sizeof(tStudent), 1, p1_Write);
        fprintf(p1_Write, "%08d %05.1f %s %-50s %-30s\n",
                auxItem.id, auxItem.grade, auxItem.state,
                auxItem.city, auxItem.course);
        (p1->size)++;
      }
      else
      {
        //fwrite(&auxItem, sizeof(tStudent), 1, p2_Write);
        fprintf(p2_Write, "%08d %05.1f %s %-50s %-30s\n",
                auxItem.id, auxItem.grade, auxItem.state,
                auxItem.city, auxItem.course);
        //fseek(p2_Write, sizeof(tStudent) * -2, 1);
        //sizeof(tStudent)
        fseek(p2_Write, LINE_SIZE * -2, 1);
        (p2->size)++;
      }

      //teste
      //o ponteiro de escrita não pode ultrapassar o de leitura
      if (ftell(p1_Write) == ftell(p1_Read) && nextRead == 2)
        nextRead = 1;
      else if (ftell(p2_Write) == ftell(p2_Read) && nextRead == 1)
        nextRead = 2;
    }    
    else
    {
      RAM->student[RAM->end] = auxItem;
      (RAM->size)++;
      (RAM->end)++;
    }
  }

  //p1->delimiter = ftell(p1_Write) / sizeof(tStudent) - 1;
  //p2->delimiter = p1->delimiter + RAM->size + 1;
  //sizeof(tStudent)
  p1->delimiter = ftell(p1_Write) / LINE_SIZE - 1;
  p2->delimiter = p1->delimiter + RAM->size + 1;

  insertionSort(&(*RAM));

  for (i = RAM->start; i < RAM->end; i++)
    //fwrite(&(RAM->student[i]), sizeof(tStudent), 1, p1_Write);
    fprintf(p1_Write, "%08d %05.1f %s %-50s %-30s\n",
            RAM->student[i].id, RAM->student[i].grade, RAM->student[i].state,
            RAM->student[i].city, RAM->student[i].course);

  fclose(p1_Read);
  fclose(p1_Write);
  fclose(p2_Read);
  fclose(p2_Write);
}

void sort(int p1_Read, int p2_Read, int p1_Write, int p2_Write,
          tRAM *RAM, tPartition *p1, tPartition *p2,
          short RAM_SIZE_QS)
{
  int p1_size, p2_size, p1_delimiter, p2_delimiter;

  if (p1_Read == p2_Read)
    return;

  quickSort(p1_Read, p2_Read, p1_Write, p2_Write, &(*RAM), &(*p1), &(*p2),
            RAM_SIZE_QS);

  p1_delimiter = p1->delimiter;
  p2_delimiter = p2->delimiter;
  p1_size = p1->size;
  p2_size = p2->size;

  if (p1_size > 1)
  {
    sort(p1_delimiter - p1_size + 1,
         p1_delimiter,
         p1_delimiter - p1_size + 1,
         p1_delimiter,
         &(*RAM), &(*p1), &(*p2), RAM_SIZE_QS);
  }

  if (p2_size > 1)
  {
    sort(p2_delimiter,
         p2_delimiter + p2_size - 1,
         p2_delimiter,
         p2_delimiter + p2_size - 1,
         &(*RAM), &(*p1), &(*p2), RAM_SIZE_QS);
  }
}
