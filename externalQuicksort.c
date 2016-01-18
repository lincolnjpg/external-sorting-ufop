#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

#include "externalQuicksort.h"

void externalQuickSort(void)
{
  short RAM_SIZE_QS;
  int sorted, grade;
  tStudent aux;
  tPartition p1, p2;
  tRAM RAM;
  FILE *f;
  struct timeval t, t2;
  double tTotal;

  //define tamanho da RAM (log(N))
  RAM_SIZE_QS = log2(N);

  //cria e inicia RAM
  createRAM(&RAM, RAM_SIZE_QS);

  //file generation
  createFileQuick();

  gettimeofday(&t, NULL);

  sort(0, N - 1, 0, N - 1, &RAM, &p1, &p2);

  gettimeofday(&t2, NULL);

  tTotal = (t2.tv_sec + t2.tv_usec / 1000000.0) - (t.tv_sec + t.tv_usec / 1000000.0);

  /*grade = -1;
  sorted = 1;

  f = fopen("file.bin", "rb");

  for (int i = 0; i < N; i++)
  {
    fread(&aux, sizeof(tStudent), 1, f);

    if (aux.grade < grade)
    {
      sorted = 0;

      break;
    }

    grade = aux.grade;
  }

  if (sorted)
    puts("sorted");
  else
    puts("not sorted");

  fclose(f);*/
}

void createFileQuick()
{
  int i;
  tStudent arrayItem[N];
  //tStudent arrayItem[5];
  FILE *file;

  file = fopen("file.bin", "wb");

  srand(time(NULL));

  puts("original");

  /*arrayItem[0].grade = 3;
  arrayItem[1].grade = 12;
  arrayItem[2].grade = 90;
  arrayItem[3].grade = 412;
  arrayItem[4].grade = 1;*/

  for (i = 0; i < N; i++)
  {
    arrayItem[i].grade = rand() % (12 * N);

    //printf("%d, ", arrayItem[i].grade);
  }

  puts("");
  fflush(stdout);

  fwrite(arrayItem, sizeof(int) * N, 1, file);

  fclose(file);
}

void insertionSort(tRAM *RAM)
{
  short i, j;
  tStudent aux;

  for (i = RAM->start + 1; i < RAM->end; i++)
  {
    aux = RAM->student[i];
    j = i;

    while (aux.grade < RAM->student[j - 1].grade && j > 0)
    {
      RAM->student[j] = RAM->student[j - 1];
      j--;
    }

    RAM->student[j] = aux;
  }
}

void quickSort(int p1_R, int p2_R, int p1_W, int p2_W,
               tRAM *RAM, tPartition *p1, tPartition *p2)
{
  short nextRead, insPoint;
  int i, inferiorLimit, superiorLimit;
  long p1_NextRead, p2_NextRead;
  tStudent auxItem;
  FILE *p1_Read, *p2_Read, *p1_Write, *p2_Write;

  p1_Read = fopen("file.bin", "rb");
  p1_Write = fopen("file.bin", "r+b");
  p2_Read = fopen("file.bin", "rb");
  p2_Write = fopen("file.bin", "r+b");

  fseek(p1_Read, p1_R * sizeof(tStudent), 0);
  fseek(p1_Write, p1_W * sizeof(tStudent), 0);
  fseek(p2_Read, p2_R * sizeof(tStudent), 0);
  fseek(p2_Write, p2_W * sizeof(tStudent), 0);

  inferiorLimit = INT_MIN;
  superiorLimit = INT_MAX;

  //get readers position2
  p1_NextRead = ftell(p1_Read);
  p2_NextRead = ftell(p2_Read);

  RAM->size = RAM->start = RAM->end = p1->size = p2->size =
  p1->delimiter = p2->delimiter = 0;
  nextRead = 2; //first read will be p2
  /*when reading the third item of the file, it will be put in
  the array's last position*/
  insPoint = RAM_SIZE - 1;

  while (p1_NextRead <= p2_NextRead)
  {
    if (RAM->size == RAM_SIZE)
    {
      //remove item (min or max)

      if (p1->size < p2->size)
      {
        insPoint = 0;

        fwrite(&(RAM->student[insPoint]), sizeof(tStudent), 1, p1_Write);

        inferiorLimit = RAM->student[insPoint].grade;
        (p1->size)++;
        RAM->start = insPoint + 1;
        RAM->end = RAM_SIZE;
      }
      else
      {
        insPoint = RAM_SIZE - 1;

        fwrite(&(RAM->student[insPoint]), sizeof(tStudent), 1, p2_Write);
        fseek(p2_Write, sizeof(tStudent) * -2, 1);

        superiorLimit = RAM->student[insPoint].grade;
        (p2->size)++;
        RAM->start = 0;
        RAM->end = insPoint;
      }

      (RAM->size)--;
    }

    if (nextRead == 1)
    {
      fread(&auxItem, sizeof(tStudent), 1, p1_Read);
      p1_NextRead = ftell(p1_Read);

      nextRead = 2;
    }
    else
    {
      fread(&auxItem, sizeof(tStudent), 1, p2_Read);
      fseek(p2_Read, sizeof(tStudent) * -2, 1);
      p2_NextRead = ftell(p2_Read);

      nextRead = 1;
    }

    if (RAM->size == RAM_SIZE - 1)
    {
      if (auxItem.grade > inferiorLimit && auxItem.grade < superiorLimit)
      {
        if (insPoint == 0)
          //insertion point: begining
          RAM->start = insPoint;
        else
          //insPoint == RAM_SIZE - 1 (insertion point: end)
          (RAM->end)++;

        RAM->student[insPoint] = auxItem;
        (RAM->size)++;

        //sort RAM
        insertionSort(&(*RAM));
      }
      else if (auxItem.grade <= inferiorLimit)
      {
        fwrite(&auxItem, sizeof(tStudent), 1, p1_Write);
        (p1->size)++;
      }
      else
      {
        fwrite(&auxItem, sizeof(tStudent), 1, p2_Write);
        fseek(p2_Write, sizeof(tStudent) * -2, 1);
        (p2->size)++;
      }
    }
    else
    {
      RAM->student[RAM->end] = auxItem;
      (RAM->size)++;
      (RAM->end)++;
    }
  }

  p1->delimiter = ftell(p1_Write) / sizeof(tStudent) - 1;
  p2->delimiter = p1->delimiter + RAM->size + 1;

  insertionSort(&(*RAM));

  for (i = RAM->start; i < RAM->end; i++)
    fwrite(&(RAM->student[i]), sizeof(tStudent), 1, p1_Write);

  fclose(p1_Read);
  fclose(p1_Write);
  fclose(p2_Read);
  fclose(p2_Write);

  //test(&(*RAM));
}

void test(tRAM *RAM)
{
  int i;
  tStudent auxItem;
  FILE *file;

  file = fopen("file.bin", "rb");

  puts("result");

  for (i = 0; i < N; i++)
  {
    fread(&auxItem, sizeof(tStudent), 1, file);

    printf("%f, ", auxItem.grade);
  }

  puts("");

  puts("pivo");

  for (i = RAM->start; i < RAM->end; i++)
    printf("%f, ", RAM->student[i].grade);

  puts("");

  fflush(stdout);

  fclose(file);
}

void sort(int p1_Read, int p2_Read, int p1_Write, int p2_Write,
          tRAM *RAM, tPartition *p1, tPartition *p2)
{
  int p1_size, p2_size, p1_delimiter, p2_delimiter;

  if (p1_Read == p2_Read)
    return;

  quickSort(p1_Read, p2_Read, p1_Write, p2_Write, &(*RAM), &(*p1), &(*p2));

  p1_delimiter = p1->delimiter;
  p2_delimiter = p2->delimiter;
  p1_size = p1->size;
  p2_size = p2->size;

  if (p1_size > 0)
  {
    sort(p1_delimiter - p1_size + 1,
         p1_delimiter,
         p1_delimiter - p1_size + 1,
         p1_delimiter,
         &(*RAM), &(*p1), &(*p2));
  }

  if (p2_size > 0)
  {
    sort(p2_delimiter,
         p2_delimiter + p2_size - 1,
         p2_delimiter,
         p2_delimiter + p2_size - 1,
         &(*RAM), &(*p1), &(*p2));
  }
}
