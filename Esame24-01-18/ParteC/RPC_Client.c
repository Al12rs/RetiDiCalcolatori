/* contaClient.c */

#include <stdio.h>
#include <rpc/rpc.h>
#include "RPC_xFile.h"

int main(int argc, char *argv[])
{

  CLIENT *clnt;
  int *ris;
  Output *out;
  char *server, *nomeFile;
  Input input;
  char car, ok[128];

  input.direttorio = (char *)malloc(50);
  input.soglia = 0;
  nomeFile = (char *)malloc(50);

  if (argc != 2)
  {
    fprintf(stderr, "uso: %s nomehost\n", argv[0]);
    exit(1);
  }

  clnt = clnt_create(argv[1], CONTAPROG, CONTAVERS, "udp");

  if (clnt == NULL)
  {
    clnt_pcreateerror(argv[1]);
    exit(1);
  }

  printf("richieste servizio fino a fine file\n");
  printf("operazioni:  CS=Conta File maggiori di, C=Conta\n");

  while (gets(ok))
  {
    if ((strcmp(ok, "CS") != 0) && (strcmp(ok, "C") != 0))
    {
      printf("scelta non disponibile\n");
      printf("richieste servizio fino a fine file\n");
      printf("operazioni:  CS=Conta File maggiori di, C=Conta\n");
      continue;
    }

    printf("Richiesto servizio: %s\n", ok);

    // richiesta conteggio file nel direttorio remoto
    if (strcmp(ok, "CS") == 0)
    {
      printf("inserisci il nome direttorio: \n");
      gets(input.direttorio);
      printf("inserisci la soglia: \n");
      //controllo intero
      while (scanf("%d", &input.soglia) != 1)
      {
        do
        {
          car = getchar();
          printf("%c ", car);
        } while (car != '\n');
        printf("Inserire int");
        continue;
      }
      gets(ok);
      printf("Stringa letta: %s\n", ok);

      ris = conta_file_1(&input, clnt);

      if (ris == (int *)NULL)
        clnt_perror(clnt, "call failed");
      else if (*ris == -1)
        printf("E' avvenuto un errore lato server\n");
      else
        printf("Ho contato %d file con dim >= %d!\n", *ris, input.soglia);
    } // CS

    // richiesta conteggio caratteri nel file remoto
    else if (strcmp(ok, "C") == 0)
    {
      printf("inserisci il nome del file: \n");
      gets(nomeFile);
      out = conta_1(&nomeFile, clnt);

      if (out == (int *)NULL)
        clnt_perror(clnt, "call failed");
      else if (out->codiceErrore == -1)
        printf("E' avvenuto un errore lato server\n");
      else
        printf("Ho contato %d caratteri, %d parole e %d linee !\n",
               out->caratteri, out->parole, out->linee);
      //clean input buffer
      memset(nomeFile, 0, sizeof(nomeFile));
    } // CF
    printf("richieste servizio fino a fine file\n");
    printf("operazioni:  CS=Conta File maggiori di, C=Conta\n");
  } //while

  clnt_destroy(clnt);
  free(input.direttorio);
  free(nomeFile);
  printf("Esco dal client\n");
}
