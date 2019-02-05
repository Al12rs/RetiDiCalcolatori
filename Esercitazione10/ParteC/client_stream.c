/* stream_client.c
 * Cliente invia nome dir e riceve
 * nomi file in dir sec lvl
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

//defines
#define LINE_LENGTH 256
#define WORD_LENGTH 32
//structs
typedef struct
{
  char singerName[WORD_LENGTH];
  int voto; 
} TCPAnswer;

int main(int argc, char *argv[])
{
  //template var
  int port, nread, sd, nwrite;
  struct hostent *host;
  struct sockaddr_in servaddr;
  //user var
  int m;
  TCPAnswer tcpreply;

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

  //CORPO DEL CLIENT:ciclo di accettazione di richieste da utente
  printf("# Inserire valore di soglia, EOF per terminare: ");
  while (scanf("%d", &m) == 1)
  {
    // Scrittura richiesta al server
    printf("# Invio valore di soglia al server: %d\n", m);
    nwrite = write(sd, &m, sizeof(m));
    printf("# Bytes scritti al server %d char\n", nwrite);

    // Lettura risposta dal server
    printf("# Ricevo e stampo tutti i cantanti coi requisiti richiesti:\n");
    while ((nread = read(sd, &tcpreply, sizeof(tcpreply))) > 0)
    {
      if (tcpreply.voto != -1)
        printf("Cantante: %s \t Voti: %d\n", tcpreply.singerName, tcpreply.voto);
      else{
        break;
      }
    }
    printf("# Fine dei risultati\n");

    //continuazione del ciclo
    printf("# Inserire valore di soglia, EOF per terminare: ");
  }
  //Chiusura socket in ricezione
  close(sd);
  printf("\nClient: termino...\n");
  exit(0);
}