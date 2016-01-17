#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"

short loadFromFile(FILE **file, tStudent *student)
{
  char str[86], *newString;
  short stateBeg, cityBeg, cityEnd, courseBeg, courseEnd;

  stateBeg = 0;
  cityBeg = 3;
  cityEnd = cityBeg + 49;
  courseBeg = 54;
  courseEnd = courseBeg + 29;

  //file's lines have 100 characters at most (plus new-line)

  if (fscanf(*file, "%d %f ", &(student->id), &(student->grade)) == 2)
  {
    //pergunto se é igual a dois pq fscanf retorna a quantidade de "variáveis" lidas
    fgets(str, 86, *file);

    newString = strtok (str, " ");

    strcpy(student->city, "");
    strcpy(student->course, "");

    while (newString != NULL)
    {
      if (newString >= str + cityBeg && newString <= str + cityEnd)
      {
        strcat(student->city, newString);
        strcat(student->city, " ");
      }
      else if (newString >= str + courseBeg && newString <= str + courseEnd)
      {
        strcat(student->course, newString);
        strcat(student->course, " ");
      }
      else if (newString == str + stateBeg)
        strcpy(student->state, newString);

      newString = strtok (NULL, " ");
    }

    //removes blank space at the end of c-string
    student->city[strlen(student->city) - 1] = '\0';
    student->course[strlen(student->course) - 1] = '\0';

    return 0;
  }
  else
    return 1;
}

void createNewFile(short situation)
{
  switch (situation)
  {
    case 1:

      break;
    case 2:
      break;
  }
}
