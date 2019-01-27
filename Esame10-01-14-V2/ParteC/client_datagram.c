/* client_datagram.c
 *
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

//Defines
#define LINE_LENGTH 64
#define MAX_ELEMENTS 10
#define FILE_LENGTH 256

//structs
typedef struct Evento
{
  char descrizione[LINE_LENGTH];
  char tipo[LINE_LENGTH];
  char data[LINE_LENGTH];
  char luogo[LINE_LENGTH];
  int disponibilita;
  int prezzo;
} Evento;

/*typedef struct{
 
} UDPRequest;*/

typedef struct
{
  Evento vettoreEventi[MAX_ELEMENTS];
  int numValidi;
} UDPAnswer;

int main(int argc, char **argv)
{
  //template
  struct hostent *host;
  struct sockaddr_in clientaddr, servaddr;
  int port, nread, sd, len;
  char c, loopString[256];
  //UDPRequest req;
  //user variables
  UDPAnswer ris;
  int prezzoMassimo;
  int i;

  //CONTROLLO DEGLI ARGOMENTI
  if (argc != 3)
  {
    printf("Usage: %s serverAdress serverPort\n", argv[0]);
    exit(1);
  }

  //INIZIALIZZAZIONE INDIRIZZO CLIENT
  memset((char *)&clientaddr, 0, sizeof(struct sockaddr_in));
  clientaddr.sin_family = AF_INET;
  clientaddr.sin_addr.s_addr == INADDR_ANY;
  clientaddr.sin_port = 0;

  //INIZIALIZZAZIONE INDIRIZZO SERVER
  memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
  servaddr.sin_family = AF_INET;
  host = gethostbyname(argv[1]);
  //Check correttezza porta
  nread = 0;
  while (argv[2][nread] != '\0')
  {
    if ((argv[2][nread] < '0') || (argv[2][nread] > '9'))
    {
      printf("Argument is not an integer.\n");
      printf("Usage: %s serverAdress serverPort\n", argv[0]);
      exit(2);
    }
    nread++;
  }
  port = atoi(argv[2]);
  if(port < 1024 || port > 6536){
    printf("Port number is out of valid range.\n");
    exit(2);
  }
  //Check correttezza host
  if(host == NULL){
    printf("%s not found in /etc/host\n", argv[1]);
    exit(2);
  } else {
    servaddr.sin_addr.s_addr = ((struct in_addr *)(host->h_addr))->s_addr;
    servaddr.sin_port = htons(port);
  }

  //CREAZIONE SOCKET
  sd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sd < 0)
  {
    perror("Apertura socket");
    exit(1);
  }
  printf("Client: Created the socket sd=%d\n", sd);

  //BIND SOCKET A PORTA CASUALE
  if (bind(sd, (struct sockaddr *) &clientaddr, sizeof(clientaddr)) < 0)
  {
    perror("Bind socket");
    exit(1);
  }
  printf("Client: bind socket ok, to port: %i\n", clientaddr.sin_port);


  //CORPO DEL CLIENT
  strcpy(loopString, "# Inserisci prezzo massimo:\n");
  printf("%s", loopString);

  //Loop of requests.
  while(scanf("%d", &prezzoMassimo) == 1){
    while((c=getchar()) != '\n'); //consumo fine linea
    
    //Invio richiesta.
    len = sizeof(servaddr);
    printf("Send request.\n");
    if (sendto(sd, &prezzoMassimo, sizeof(prezzoMassimo), 0, (struct sockaddr *)&servaddr, len) < 0)
    {
      perror("Sendto");
      //Error is not fatal, continue loop.
      printf("%s\n", loopString);
      continue;
    }

    //Ricezione risultato.
    if (recvfrom(sd, &ris, sizeof(ris), 0, (struct sockaddr *) &servaddr, &len) < 0) {
      perror("Recvfrom");
      //Error is not fatal, continue loop.
      printf("%s\n", loopString);
      continue;
    }

    //Display result.
    if(ris.numValidi < 0){
      printf("# Client: Errore lato server\n");
    }
    else
    {
      printf("Descrizione - tipo - data - luogo - disponibilita - prezzo");
      for (i = 0; i < ris.numValidi; i++)
      {
        printf("%s - %s - %s - %s - %d - %d\n", ris.vettoreEventi[i].descrizione,
               ris.vettoreEventi[i].tipo, ris.vettoreEventi[i].data, ris.vettoreEventi[i].luogo,
               ris.vettoreEventi[i].disponibilita, ris.vettoreEventi[i].prezzo);
      }
    }

    printf("%s\n", loopString);

  }//while loop

  printf("Client: received EOF, terminating.\n");  

}//main
