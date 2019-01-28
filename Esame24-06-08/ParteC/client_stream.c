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
#define LINE_LENGTH 256
#define PATH_LENGTH 256
#define WORD_LENGTH 32
//structs
typedef struct
{
  char fileName[PATH_LENGTH];
  char keyword[WORD_LENGTH];
} TCPRequest;

int main(int argc, char *argv[])
{
  //template var
  int port, nread, sd, nwrite;
  struct hostent *host;
  struct sockaddr_in servaddr;
  //user var
  char tcpreply[LINE_LENGTH];
  int i;
  TCPRequest tcpreq;

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
    printf("Porta scorretta...\n");
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
  printf("# Inserire nome file EOF per terminare: \n");
  while (scanf("%s", tcpreq.fileName) == 1)
  {
    // Eliminazione del fine linea
    while (getchar() != '\n');

    printf("Inserire keyword: \n");
    scanf("%s", tcpreq.keyword);
    while (getchar() != '\n'); //consumo fine linea

    // Scrittura richiesta al server
    printf("# Invio la richiesta al server: \n");
    nwrite = write(sd, &tcpreq, sizeof(tcpreq));
    printf("# Bytes scritti al server %d char\n", nwrite);

    // Lettura risposta dal server
    printf("# Ricevo i file:\n");
    i = 0;
    while ((nread = read(sd, tcpreply, sizeof(tcpreply))) > 0)
    {
      if (strcmp(tcpreply, "#") == 0)
      {
        break;
      }
      /*if (tcpreply.fileSize == -2)
      {
        printf("Errore server: Impossibile aprire direttorio\n");
        break;
      }*/

      i++;
      printf("# Linea Ricevuta (%d): %s\n",i, tcpreply);
      
    } // ricezione delle linee
    printf("# Linee esaurite\n");

    //continuazione del ciclo
    printf("# Inserire nome file, EOF per terminare: \n");

  } //while richieste

  //Chiusura socket in ricezione
  close(sd);
  printf("\nClient: termino...\n");
  exit(0);
}
