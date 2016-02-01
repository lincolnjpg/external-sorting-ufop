/*
IMPORTANTE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

O arquivo "PROVAO.txt", original - site do professor, foi criado no S.O. Windows.
Por esse motivo, o caractere "final de linha" é diferente. Use um editor de texto
para ajustar isso (no Kate, por exemplo, vá em Tools/End of Line). Isso me deu
uma dor de cabeça danada, pois o quickSort externo não funcionava quando eu tentava
ordenar os N primeiros registros do arquivo original (sempre que eu tentava ler
os dois primeiros campos, o ftell() dizia que eu estava pulando mais que 15 caracteres,
e isso atrapalhava tudo dali pra frente). Quando eu usava aqueles meus arquivos,
menores (1K, 2K, etc), funcionava.
*/


#include <stdio.h>
#include <stdlib.h>

#include "file.h"
#include "validation.h"
#include "merge.h"
#include "quicksort.h"

int main(int argc, char *argv[])
{
  /*char MSG_VALIDACAO_ARGUMENTOS[TAM_ID], MSG_GERACAO_ARQUIVO[TAM_ID];
  int retorno, compIndex_2, compIndex_3, compPesq_1, compPesq_2,
      compPesq_3, transfIndex_1, transfPesq_1, transfIndex_2, transfPesq_2,
      transfIndex_3, transfPesq_3;
  double tempoIndex_1, tempoIndex_2, tempoIndex_3, tempoPesq_1, tempoPesq_2,
         tempoPesq_3;*/
  int retorno;
  tArguments arguments;

  /*compIndex_2 = compIndex_3 = compPesq_1 = compPesq_2 =
  compPesq_3 = transfIndex_1 = transfPesq_1 = transfIndex_2 = transfPesq_2 =
  transfIndex_3 = transfPesq_3 = 0;*/

  //Realiza validação dos argumentos
  retorno = validateArguments(argc, &argv, &arguments);

  if (retorno > 0)
  {
    //Erro na validação dos argumentos
    /*strcpy(MSG_VALIDACAO_ARGUMENTOS, "");
    strcat(MSG_VALIDACAO_ARGUMENTOS, ERRO_VALIDACAO);
    strcat(MSG_VALIDACAO_ARGUMENTOS, CODIGO_ERRO);
    printf("%s: %d\n", MSG_VALIDACAO_ARGUMENTOS, retorno);*/
  }
  else
  {
    identifyProperInputFile(arguments.situation);

    if (arguments.method != 4)
      merge(arguments.method, arguments.quantity);
    else
      externalQuickSort(arguments.quantity);
  }

  return 0;//retorno;
}

/*int main(void)
{

  printf("%d\n", FOPEN_MAX);
  balancedMerge();
  //externalQuickSort();


  return 0;
}*/


