/* contaClient.c */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <rpc/rpc.h>
#include "RPC_xFile.h"

int main(int argc, char *argv[])
{

  CLIENT *clnt;
  Output1 *output1;
  int *output2;
  Input1 input1;
  Input2 input2;
  char requestType[3];
  int i, j;

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
  printf("# operazioni:  EU = Elimina Utente, ES = Elimina Stanza\n");

  while (gets(requestType))
  {
    if ((strcmp(requestType, "EU") != 0) && (strcmp(requestType, "ES") != 0))
    {
      printf("scelta non disponibile\n");
      printf("richieste servizio fino a fine file\n");
      printf("# operazioni:  EU = Elimina Utente, ES = Elimina Stanza\n");
      continue;
    }

    printf("Richiesto servizio: %s\n", requestType);

    // richiesta 1
    if (strcmp(requestType, "EU") == 0)
    {
      printf("# Inserisci nome utente da eliminare: ");
      gets(input1.nome);

      output1 = elimina_utente_1(&input1, clnt);

      if (output1 == (Output1 *)NULL){
        clnt_perror(clnt, "call failed");
      }
      else if (output1->esito == -1)
        printf("E' avvenuto un errore lato server\n");
      else
      {
        printf("# Operazione eseguita correttamente.\n");
        printf("Lista stanza liberate: \n");
        for (i = 0; i < MAX_ELEMENTS; i++)
        {
          if (strcmp(output1->stanzeLiberate[i].nome, "L") != 0)
          {
            printf("Nome: %s\n", output1->stanzeLiberate[i].nome);
            printf("Stato: %s\n", output1->stanzeLiberate[i].stato);
            for (j = 0; j < NUM_USERS; j++)
            {
              if (strcmp(output1->stanzeLiberate[i].utenti[j].nome, "L") != 0)
              {
                printf("Utente%d: %s\n", j + 1, output1->stanzeLiberate[i].utenti[j].nome);
              }
            }
            printf("\n");
          }
          else
          {
            //fine elementi validi
            break;
          }
        }//ciclo di stampa

      }
    } // fine richiesta 1

    // Richiesta 2
    else if (strcmp(requestType, "ES") == 0)
    {
      printf("# Inserisci nome Stanza: \n");
      gets(input2.nome);

      output2 = elimina_stanza_1(&input2, clnt);

      if (output2 == (int *)NULL)
        clnt_perror(clnt, "call failed");
      else if ((*output2) != 0)
        printf("E' avvenuto un errore lato server %d\n", *output2);
      else
      {
        printf("# Operazione eseguita correttamente. %d\n", *output2);
      }
    } // Fine richiesta 2

    printf("richieste servizio fino a fine file\n");
    printf("# operazioni:  EU = Elimina Utente, ES = Elimina Stanza\n");

  } //while

  clnt_destroy(clnt);
  printf("Esco dal client\n");
}
