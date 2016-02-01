#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"

short loadFromFile(FILE **file, tStudent *student)
{
  char str[86], *newString;
  short stateBeg, cityBeg, cityEnd, courseBeg, courseEnd;
  int test;

  stateBeg = 0;
  cityBeg = 3;
  cityEnd = cityBeg + 49;
  courseBeg = 54;
  courseEnd = courseBeg + 29;

  test = ftell(*file);

  //test
  //fseek(*file, 100 * 19, 0);

  //file's lines have 100 characters at most (plus new-line)
  test = fscanf(*file, "%d %f ", &(student->id), &(student->grade));
  //test = fscanf(*file, "%s", strTest);

  if (test == 2)
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

/*
Função: identifyProperInputFile
  - Identifica e define arquivo que será utilizado como entrada. O arquivo
    adequado é escolhido com base no valor do parâmetro de entrada. Uma vez
    identificado, tal arquivo é copiado para o mesmo diretório, mas com outro
    nome, "PROVAO.txt", que é o nome de arquivo utilizado durante o processo
    de leitura dos dados de entrada.

  - Observação: Essa função é dependente de Sistemas Operacionais baseados em
    Linux, pois faz uso do comando "cp". A função assume que os arquivos
    "PROVAO-ASC.txt", "PROVAO-DESC.txt" e "PROVAO-RANDOM.txt" existem e estejam
    localizados no mesmo diretório que o executável do programa e, por isso, não
    verifica se a cópia foi realizada com sucesso.

Parâmetros:
  - situation: número inteiro que indica situação do arquivo de entrada
               (1 - ordenado ascendentemente, 2 - ordenado descendentemente,
               3 - desordenado aleatoriamente).

Retorno: nenhum
*/

void identifyProperInputFile(short situation)
{
  switch(situation)
  {
    case 1:
      /*renomeia arquivo "PROVAO-ASC.txt", que está ordenado ascendentemente, de
      acordo com as notas, para "PROVAO.txt".*/
      system("cp ./PROVAO-ASC.txt ./PROVAO.txt");
    case 2:
      /*renomeia arquivo "PROVAO-DESC.txt", que está ordenado descendentemente,
      de acordo com as notas, para "PROVAO.txt".*/
      system("cp ./PROVAO-DESC.txt ./PROVAO.txt");
    case 3:
      /*renomeia arquivo "PROVAO-RANDOM.txt", que está desordenado
      aleatoriamente, de acordo com as notas, para "PROVAO.txt".*/
      system("cp ./PROVAO-RANDOM.txt ./PROVAO.txt");
  }
}
