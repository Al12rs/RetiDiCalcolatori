/* stream_client.c
 * 
 * 
 */

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

//defines
#define LINE_LENGTH 64
#define PATH_LENGTH 256
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

typedef struct 
{
  char tipo[LINE_LENGTH];
  char luogo[LINE_LENGTH];
} TCPRequest;

/*
typedef struct
{
  char fileName[PATH_LENGTH];
  long fileSize;
} TCPAnswer;
*/

int main(int argc, char *argv[])
{
  //template var
  int port, nread, sd, nwrite;
  struct hostent *host;
  struct sockaddr_in servaddr;
  //user var
  int i;
  TCPRequest tcpreq;
  Evento tcpreply;

  // CONTROLLO ARGOMENTI
  if (argc != 3)
  {
    printf("Error:%s serverAddress serverPort\n", argv[0]);
    exit(1);
  }

  // INIZIALIZZAZIONE INDIRIZZO SERVER
  memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
  servaddr.sin_family = AF_INET;
  host = gethostbyname(argv[1]);
  if (host == NULL)
  {
    printf("%s not found in /etc/hosts\n", argv[1]);
    exit(1);
  }

  nread = 0;
  while (argv[2][nread] != '\0')
  {
    if ((argv[2][nread] < '0') || (argv[2][nread] > '9'))
    {
      printf("Secondo argomento non intero\n");
      exit(2);
    }
    nread++;
  }

  port = atoi(argv[2]);
  if (port < 1024 || port > 65535)
  {
    printf("Porta scorretta...");
    exit(2);
  }

  servaddr.sin_addr.s_addr = ((struct in_addr *)(host->h_addr_list[0]))->s_addr;
  servaddr.sin_port = htons(port);

  // CREAZIONE SOCKET
  sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd < 0)
  {
    perror("apertura socket");
    exit(3);
  }
  printf("Client: creata la socket sd=%d\n", sd);

  // Operazione di BIND implicita nella connect
  if (connect(sd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) < 0)
  {
    perror("connect");
    exit(3);
  }
  printf("Client: connect ok\n");

  // CORPO DEL CLIENT:
  //ciclo di accettazione di richieste da utente
  printf("# Inserire tipo evento o EOF per terminare: \n");
  while (scanf("%s", tcpreq.tipo) == 1)
  {
    // Eliminazione del fine linea
    while (getchar() != '\n')
      ;

    //Lettura input pt2
    printf("# Inserire luogo:\n");
    scanf("%s", tcpreq.luogo);

    // Eliminazione del fine linea
    while (getchar() != '\n')
      ;

    // Scrittura richiesta al server
    printf("# Invio la richiesta al server\n");
    nwrite = write(sd, &tcpreq, sizeof(tcpreq));
    printf("# Bytes scritti al server %d char\n", nwrite);

    // Lettura risposta dal server
    printf("# Ricevo gli eventi richiesti:\n");
    while ((nread = read(sd, &tcpreply, sizeof(tcpreply))) > 0)
    {
      if (tcpreply.disponibilita == -1)
      {
        break;
      }
      if (tcpreply.disponibilita == -2)
      {
        printf("Nessun evento disponibile.\n");
        break;
      }

      i = 1;
      printf("Evento %d:\n\tDesc: %s\n\tTipo: %s\n\tData: %s\n\tLuogo: %s\n\tDisp: %d\n\tPrezzo: %d\n", i, tcpreply.descrizione,
             tcpreply.tipo, tcpreply.data, tcpreply.luogo, tcpreply.disponibilita, tcpreply.prezzo);
      i++;
    } // ricezione di tutti gli eventi
    printf("# Fine lista eventi\n");

    //continuazione del ciclo
    printf("# Inserire tipo evento o EOF per terminare: \n");

  } //while richieste

  //Chiusura socket in ricezione
  close(sd);
  printf("\nClient: termino...\n");
  exit(0);
}
