#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tape.h"

void createTape(tTape *tape, short id)
{
  char auxId[3]; //number as c-string

  sprintf(auxId, "%d", id);
  strcpy(tape->name, "tape_");
  strcat(tape->name, auxId);
  strcat(tape->name, ".txt");

  tape->id = id;
  tape->length = tape->numReads = 0;
  tape->status = 1; //0 - inactive, 1 - active

  tape->file = fopen(tape->name, "w");
}

short insertTape(tTape *tape, tStudent student)
{
  if (tape->file != NULL)
  {
    fprintf(tape->file, "%08d %05.1f %s %-50s %-31s\n",
            student.id, student.grade, student.state,
            student.city, student.course);

    if (ferror(tape->file))
      return 2;
    else
    {
      incTapeLength(tape);

      return 0;
    }
  }
  else
    return 1;
}

short readTape(tTape *tape, tStudent *student)
{
  //função deve identificar se foi possível ler os dados... (retorno de loadFromFile)

  if (tape->file == NULL)
    return 1;
  else
  {
    loadFromFile(&(tape->file), student);

    if (ferror(tape->file))
      return 2;
    else
    {      
      student->origin = tape->id;
      setNumReads(tape, getNumReads(tape) + 1);

      decTapeLength(tape);

      return 0;
    }
  }
}

int getTapeLength(tTape *tape)
{
  return tape->length;
}

short getTapeStatus(tTape *tape)
{
  return tape->status;
}

void setStatus(tTape *tape, short status)
{
  tape->status = status;
}

void incTapeLength(tTape *tape)
{
  (tape->length)++;
}

void decTapeLength(tTape *tape)
{
  (tape->length)--;
}

short openTape(tTape *tape, char mode[3])
{
  tape->file = fopen(tape->name, mode);

  return ferror(tape->file);
}

short closeTape(tTape *tape)
{
  fclose(tape->file);

  return ferror(tape->file);
}

int getNumReads(tTape *tape)
{
  return tape->numReads;
}

void setNumReads(tTape *tape, int reads)
{
  tape->numReads = reads;
}
