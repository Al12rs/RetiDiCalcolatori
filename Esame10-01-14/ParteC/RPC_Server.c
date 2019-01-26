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

Evento tabella[MAX_ELEMENTS];

void inizializzazione(){
  int i;
  for (i = 0; i < MAX_ELEMENTS; i++){
    strcpy(tabella[i].descrizione, "L");
    strcpy(tabella[i].tipo, "L");
    strcpy(tabella[i].data, "L");
    strcpy(tabella[i].luogo, "L");
    tabella[i].disponibilita = -1;
    tabella[i].prezzo = -1;            
  }
  i = 0;
  strcpy(tabella[i].descrizione, "String");
  strcpy(tabella[i].tipo, "Concerto");
  strcpy(tabella[i].data, "11/01/2014");
  strcpy(tabella[i].luogo, "Verona");
  tabella[i].disponibilita = 40;
  tabella[i].prezzo = 40;
  i++;
  strcpy(tabella[i].descrizione, "Junentus-Inger");
  strcpy(tabella[i].tipo, "Calcio");
  strcpy(tabella[i].data, "03/05/2014");
  strcpy(tabella[i].luogo, "Torino");
  tabella[i].disponibilita = 21;
  tabella[i].prezzo = 150;
  i++;
  strcpy(tabella[i].descrizione, "GP Bologna");
  strcpy(tabella[i].tipo, "Formula1");
  strcpy(tabella[i].data, "07/09/2014");
  strcpy(tabella[i].luogo, "Bologna");
  tabella[i].disponibilita = 10;
  tabella[i].prezzo = 200;

  inizializzato = 1;
}

//AUXILIAR FUNCTION

//REMOTE PROCEDURE CALL 1tabella
int *inserimento_evento_1_svc(Input1 *input1, struct svc_req *rp)
{
  static int result;
  int i;
  printf("Executing RPC 1.\n");
  result = -1;

  if (!inizializzato)
    inizializzazione();

  for (i = 0; i < MAX_ELEMENTS; i++)
  {
    if(tabella[i].disponibilita < 0){
      strcpy(tabella[i].descrizione, input1->descrizione);
      strcpy(tabella[i].tipo, input1->tipo);
      strcpy(tabella[i].data, input1->data);
      strcpy(tabella[i].luogo, input1->luogo);
      tabella[i].disponibilita = input1->disponibilita;
      tabella[i].prezzo = input1->prezzo;
      result = 0;
    }
  }
  return &result;
} //rpc 1

//REMOTE PROCEDURE CALL 2
int *acquista_biglietti_1_svc(Input2 *input2, struct svc_req *rp)
{
  static int result;
  int i;
  printf("Executing RPC 2.\n");
  result = -1;
  printf("RPC 2 initial result: %d\n", result);

  if (!inizializzato)
    inizializzazione();

  for (i = 0; i < MAX_ELEMENTS; i++)
  {
    if (strcmp(tabella[i].descrizione, input2->descrizione) == 0 && tabella[i].disponibilita >= input2->numBiglietti)
    {
      tabella[i].disponibilita -= input2->numBiglietti;
      printf("Disponibilita aggiornata: %d\n", tabella[i].disponibilita);
      result = 0;
      break;
    }
  }
  printf("RPC 2 result: %d\n", result);

  return &result;
} //rpc 2
