/* contaClient.c */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <rpc/rpc.h>
#include "RPC_xFile.h"

int main(int argc, char *argv[])
{

  CLIENT *clnt;
  int *output1;
  int *output2;
  Input1 input1;
  Input2 input2;
  char requestType[3];

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

  //CORPO CLIENT
  printf("Richieste servizio fino a fine file.\n");
  printf("# operazioni:  IE = Inserisci Evento, AB = Acquista Biglietto\n");

  while (gets(requestType))
  {
    if ((strcmp(requestType, "IE") != 0) && (strcmp(requestType, "AB") != 0))
    {
      printf("scelta non disponibile\n");
      printf("richieste servizio fino a fine file\n");
      printf("# operazioni:  IE = Inserisci Evento, AB = Acquista Biglietto\n");
      continue;
    }

    printf("Richiesto servizio: %s\n", requestType);

    // richiesta conteggio file nel direttorio remoto
    if (strcmp(requestType, "IE") == 0)
    {
      printf("# Inserisci descrizione evento: ");
      gets(input1.descrizione);
      printf("# Inserisci tipologia evento: ");
      gets(input1.tipo);
      printf("# Inserisci data evento: ");
      gets(input1.data);
      printf("# Inserisci luogo evento: ");
      gets(input1.luogo);
      printf("# Inserisci disponibilita evento: ");
      while(scanf("%d", &input1.disponibilita) != 1)
      {
        while(getchar() != '\n');
        printf("# Inserisci disponibilita evento: ");        
      }
      printf("# Inserisci prezzo evento: ");
      while (scanf("%d", &input1.prezzo) != 1)
      {
        while (getchar() != '\n');
        printf("# Inserisci prezzo evento: ");
      }
      while (getchar() != '\n');

      output1 = inserimento_evento_1(&input1, clnt);

      if (output1 == (int *)NULL)
        clnt_perror(clnt, "call failed");
      else if ((*output1) == -1)
        printf("E' avvenuto un errore lato server\n");
      else{
        printf("# Operazione eseguita correttamente.\n");
        }
    } // Inserisci evento

    // Acquista Biglietti
    else if (strcmp(requestType, "AB") == 0)
    {
      printf("# Inserisci descrizione evento: ");
      gets(input2.descrizione);
      printf("# Inserisci numero di biglietti da acquistare: ");
      while (scanf("%d", &input2.numBiglietti) != 1)
      {
        while (getchar() != '\n');
        printf("# Inserisci numero di biglietti da acquistare: ");
      }
      while (getchar() != '\n');

      output2 = acquista_biglietti_1(&input2, clnt);

      if (output2 == (int *)NULL)
        clnt_perror(clnt, "call failed");
      else if ((*output2) != 0)
        printf("E' avvenuto un errore lato server %d\n", *output2);
      else
      {
        printf("# Operazione eseguita correttamente. %d\n", *output2);
      }
    } // Acquista Biglietti

    printf("richieste servizio fino a fine file\n");
    printf("# operazioni:  IE = Inserisci Evento, AB = Acquista Biglietto\n");

  } //while

  clnt_destroy(clnt);
  printf("Esco dal client\n");
}
