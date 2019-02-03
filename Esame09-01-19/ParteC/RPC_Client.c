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
  char filePath[PATHLENGTH];
  char direttorio[PATHLENGTH];
  int i = 0;

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

  printf("Richieste servizio fino a fine file.\n");
  printf("# operazioni:  EO = Elimina occorrenze numeri, LF = Lista dei primi 6 file con un numero dispari\n");

  while (gets(requestType))
  {
    if ((strcmp(requestType, "EO") != 0) && (strcmp(requestType, "LF") != 0))
    {
      printf("scelta non disponibile\n");
      printf("richieste servizio fino a fine file\n");
      printf("# operazioni:  EO = Elimina occorrenze numeri, LF = Lista dei primi 6 file con un numero dispari\n");
      continue;
    }

    printf("Richiesto servizio: %s\n", requestType);

    // richiesta conteggio file nel direttorio remoto
    if (strcmp(requestType, "LF") == 0)
    {
      printf("# Inserisci il nome direttorio: \n");
      gets(direttorio);
	    strcpy(input2.direttorio, direttorio);

      output2 = lista_file_dispari_1(&input2, clnt);

      if (output2 == (Output2 *)NULL)
        clnt_perror(clnt, "call failed");
      else if (output2->numFiles == -1)
        printf("E' avvenuto un errore lato server\n");
      else{
        printf("# Ho contato %d file che iniziano con un numero dispari.\n", output2->numFiles);
        for (i = 0; i < output2->numFiles; i++){
          printf("File %d: %s\n", i+1, output2->fileList[i].name);
        }
      }
    } // List Files

    // richiesta conteggio occorrenza linea nel direttorio remoto
    else if (strcmp(requestType, "EO") == 0)
    {
      printf("# Inserisci il file path: \n");
      gets(filePath);
      strcpy(input1.filePath, filePath);

      ris = elimina_occorrenze_1(&input1, clnt);

      if (ris == (int *)NULL)
        clnt_perror(clnt, "call failed");
      else if ((*ris) == -1)
        printf("E' avvenuto un errore lato server\n");
      else
        printf("# Ho eliminato %d occorrenze di numeri\n", (*ris));
    } // Elimina Occorrenze

    printf("richieste servizio fino a fine file\n");
    printf("# operazioni:  EO = Elimina occorrenze numeri, LF = Lista dei primi 6 file con un numero dispari\n");

  } //while

  clnt_destroy(clnt);
  printf("Esco dal client\n");
}
