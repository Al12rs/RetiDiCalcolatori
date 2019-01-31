/*
 *
 * 
 */

#include <stdio.h>
#include <rpc/rpc.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include "RPC_xFile.h"

int inizializzato = 0;

Stanza tabella[MAX_ELEMENTS];

Stanza emptyStanza;

void inizializzazione(){
  int i,j;
  //Inizializzio una stanza vouta come stampino
  strcpy(emptyStanza.nome, "L");
  strcpy(emptyStanza.stato, "L");
  for (j = 0; j < NUM_USERS; j++)
  {
    strcpy(emptyStanza.utenti[j].nome, "L");
  }
  
  for (i = 0; i < MAX_ELEMENTS; i++)
  {
    tabella[i] = emptyStanza;
  }

  i = 0;
  strcpy(tabella[i].nome, "stanza1");
  strcpy(tabella[i].stato, "P");
  j = 0;
  strcpy(tabella[i].utenti[j].nome, "Pippo");
  j++;
  strcpy(tabella[i].utenti[j].nome, "Minnie");
  j++;
  strcpy(tabella[i].utenti[j].nome, "Orazio");

  i++;
  strcpy(tabella[i].nome, "stanza2");
  strcpy(tabella[i].stato, "SP");
  j = 0;
  strcpy(tabella[i].utenti[j].nome, "Pippo");
  j++;
  strcpy(tabella[i].utenti[j].nome, "Pluto");

  i++;
  strcpy(tabella[i].nome, "stanza3");
  strcpy(tabella[i].stato, "M");
  j = 0;
  strcpy(tabella[i].utenti[j].nome, "Pluto");
  j++;
  strcpy(tabella[i].utenti[j].nome, "Paperino");
  j++;
  strcpy(tabella[i].utenti[j].nome, "Quo");
  j++;
  strcpy(tabella[i].utenti[j].nome, "Qui");

  i = MAX_ELEMENTS - 1;
  strcpy(tabella[i].nome, "stanzaN");
  strcpy(tabella[i].stato, "SM");

  inizializzato = 1;
}

//AUXILIAR FUNCTION

//REMOTE PROCEDURE CALL 1tabella
Output1 *elimina_utente_1_svc(Input1 * input1, struct svc_req * rp)
{
  static Output1 result1;
  int i, j, k, l;
  printf("Executing RPC 1.\n");
  k = 0;

  if (!inizializzato)
    inizializzazione();

  //Inizializzazzione result1
  result1.esito = -1;

  for (i = 0; i < MAX_ELEMENTS; i++)
  {
    result1.stanzeLiberate[i] = emptyStanza;
  }
  printf("RPC 1 dopo inizializzzazzione: %d\n", result1.esito);

  //cerco result1ati
  for (i = 0; i < MAX_ELEMENTS; i++)
  {
    if (strcmp(tabella[i].nome, "L") != 0)
    {
      for (j = 0; j < NUM_USERS; j++)
      {
        if (strcmp(tabella[i].utenti[j].nome, input1->nome) == 0)
        {
          strcpy(tabella[i].utenti[j].nome, "L");
          /*
        strcpy(result1.stanzeLiberate[k].nome, tabella[i].nome);
        strcpy(result1.stanzeLiberate[k].stato, tabella[i].stato);
        for (l = 0; l < NUM_USERS; l++){
          strcpy(result1.stanzeLiberate[k].utenti[l].nome, tabella[i].utenti[l].nome);                    
        }
        */
          result1.stanzeLiberate[k] = tabella[i];
          k++;
          result1.esito = 0;
          //a stanza can't have the same user twice.
          break;
        }//if found user
      }//for users
    }//if valid room
  }//for rooms
  
  printf("RPC 1 result1: %d\n", result1.esito);
  return &result1;
} //rpc 1

//REMOTE PROCEDURE CALL 2
int * elimina_stanza_1_svc(Input2 * input2, struct svc_req * rp)
{
  static int result2;
  int i;
  printf("Executing RPC 2.\n");
  result2 = -1;

  if (!inizializzato)
    inizializzazione();

  for (i = 0; i < MAX_ELEMENTS; i++)
  {
    if (strcmp(tabella[i].nome, input2->nome) == 0)
    {
      tabella[i] = emptyStanza;
      result2 = 0;
      break;
    }
  }
  printf("RPC 2 result2: %d\n", result2);

  return &result2;
} //rpc 2
