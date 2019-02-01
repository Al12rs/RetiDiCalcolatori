/* contaClient.c */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <rpc/rpc.h>
#include "RPC_xFile.h"

int main(int argc, char *argv[])
{

  CLIENT *clnt;
  int *ris;
  Output2 *output2;
  char *server, *nomeFile;
  Input1 input1;
  Input2 input2;
  char car, requestType[3];
  char linea[LINELENGTH];
  char direttorio[PATHLENGTH];
  char prefisso[LINELENGTH];
  int i = 0;

  /*
  input1.linea = (char *)malloc(LINELENGTH);
  input2.direttorio = (char *)malloc(PATHLENGTH);
  input2.prefisso = (char *)malloc(11);
  */

  // CONTROLLO ARGOMENTI
  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s nomehost\n", argv[0]);
    exit(1);
  }

  // CREAZIONE DEL PORTMAPPER
  clnt = clnt_create(argv[1], RPCPROG, RPCVERS, "udp");
  if (clnt == NULL)
  {
    clnt_pcreateerror(argv[1]);
    exit(1);
  }

  /*input2.direttorio=NULL;
  input2.prefisso=NULL;
  input1.linea=NULL;*/


  printf("Richieste servizio fino a fine file.\n");
  printf("# operazioni:  CO = Conta occorrenze di una linea, LF = Lista dei primi 6 file di un direttorio\n");

  while (gets(requestType))
  {
    if ((strcmp(requestType, "CO") != 0) && (strcmp(requestType, "LF") != 0))
    {
      printf("scelta non disponibile\n");
      printf("richieste servizio fino a fine file\n");
      printf("# operazioni:  CO = Conta occorrenze di una linea, LF = Lista dei primi 6 file di un direttorio\n");
      continue;
    }

    printf("Richiesto servizio: %s\n", requestType);

    // richiesta conteggio file nel direttorio remoto
    if (strcmp(requestType, "LF") == 0)
    {
      printf("# Inserisci il nome direttorio: \n");
      gets(direttorio);
	    printf("Post gets\n");
      //free(input2.direttorio);
      //printf("Post free\n");
      //input2.direttorio = malloc(strlen(direttorio) + 1);
	  strcpy(input2.direttorio, direttorio);

      printf("# Inserisci il prefisso (max 10 caratteri): \n");
      gets(prefisso);
      while (strlen(prefisso) > 10){
        printf("# Inserisci il prefisso (max 10 caratteri): \n");
        gets(prefisso);
      }
      //free(input2.prefisso);
      //input2.prefisso = malloc(strlen(prefisso) + 1);
	    strcpy(input2.prefisso, prefisso);

      output2 = lista_file_prefisso_1(&input2, clnt);

      if (output2 == (Output2 *)NULL)
        clnt_perror(clnt, "call failed");
      else if (output2->numFiles == -1)
        printf("E' avvenuto un errore lato server\n");
      else{
        printf("# Ho contato %d file con prefisso %s.\n", output2->numFiles, input2.prefisso);
        for (i = 0; i < output2->numFiles; i++){
          printf("File %d: %s\n", i+1, output2->fileList[i].name);
        }
      }
    } // List Files

    // richiesta conteggio occorrenza linea nel direttorio remoto
    else if (strcmp(requestType, "CO") == 0)
    {
      printf("inserisci la linea da cercare: \n");
      gets(linea);
      //free(input1.linea);
      //input1.linea = malloc(strlen(linea) + 1);
      strcpy(input1.linea, linea);
      ris = conta_occorenze_linea_1(&input1, clnt);

      if (ris == (int *)NULL)
        clnt_perror(clnt, "call failed");
      else if ((*ris) == -1)
        printf("E' avvenuto un errore lato server\n");
      else
        printf("# Ho contato %d occorrenza della linea -%s-\n",
               (*ris), input1.linea);
    } // Conta Occorrenze

    printf("richieste servizio fino a fine file\n");
    printf("# operazioni:  CO = Conta occorrenze di una linea, LF = Lista dei primi 6 file di un direttorio\n");

  } //while

  clnt_destroy(clnt);
  /*free(input1.linea);
  free(input2.direttorio);
  free(input2.prefisso);*/
  printf("Esco dal client\n");
}
