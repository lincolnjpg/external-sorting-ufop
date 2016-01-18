#include <stdlib.h>

#include "ram.h"

void createRAM(tRAM *RAM, short size)
{
  RAM->size = 0;
  RAM->student = (tStudent *) malloc(sizeof(tStudent) * RAM_SIZE);
}

void insertRAM(tRAM *RAM, tStudent student, short pos)
{
  RAM->student[pos] = student;
  (RAM->size)++;
}

short getSize(tRAM *RAM)
{
  return RAM->size;
}

void cleanRAM(tRAM *RAM)
{
  RAM->size = 0;
}

tStudent getStudent(tRAM *RAM, short pos)
{
  if (pos >= 0 && pos < RAM->size)
    return RAM->student[pos];
}

void removeStudent(tRAM *RAM, short pos)
{
  if (pos >= 0 && pos < RAM->size)
  {
    RAM->student[pos].grade = -1;
    (RAM->size)--;
  }
}

void sortRAM(tRAM *RAM, short size)
{
  //insertion sort
  int i, j;
  tStudent aux;

  for (i = 1; i < size; i++)
  {
    j = i;
    aux = RAM->student[i];

    //no momento, a geração de um arquivo ordenado de forma crescente
    //não está funcionando (por causa do -1 que eu coloco quando o student
    //sai da RAM)


    //paliativamente, só usar o 101 (ou qq valor acima de 100) ao invés de -1
    //além de mudar o sinal da comparação abaixo

    while (aux.grade > RAM->student[j - 1].grade && j > 0)
    {
      RAM->student[j] = RAM->student[j - 1];
      j--;
    }

    RAM->student[j] = aux;
  }
}

short getPriority(tRAM *RAM, short pos)
{
  return RAM->student[pos].priority;
}

void setPriority(tRAM *RAM, short pos, short priority)
{
  RAM->student[pos].priority = priority;
}

float getGrade(tRAM *RAM, short pos)
{
  return RAM->student[pos].grade;
}

void swapStudents(tRAM *RAM, short from, short to)
{
  tStudent aux;

  aux = getStudent(RAM, from);
  RAM->student[from] = RAM->student[to];
  RAM->student[to] = aux;
}
