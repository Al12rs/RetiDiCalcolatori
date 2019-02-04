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
#define PATH_LENGTH 128
#define LINE_LENGTH 64
#define PATENTE_LENGTH 5
#define TARGA_LENGTH 8
#define TIPO_LENGTH 7
#define FOLDER_LENGTH 13
#define MAX_ELEMENTS 10
//structs
typedef struct
{
  char targa[TARGA_LENGTH];
} TCPRequest;

typedef struct
{
  char fileName[LINE_LENGTH];
  long fileSize;
} TCPReply;

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
  int i, fd_file;
  TCPRequest reqTCP;
  TCPReply replyTCP;
  char buf;

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
  printf("# Inserire targa o EOF per terminare: \n");
  while (gets(reqTCP.targa))
  {
    // Scrittura richiesta al server
    printf("# Invio la richiesta al server\n");
    nwrite = write(sd, &reqTCP, sizeof(reqTCP));
    printf("# Bytes scritti al server %d char\n", nwrite);

    // Lettura risposta dal server
    printf("# Ricevo i file richiesti:\n");
    while ((nread = read(sd, &replyTCP, sizeof(replyTCP))) > 0)
    {
      if (replyTCP.fileSize == -1)
      {
        printf("# Fine trasmissione files.\n");
        break;
      }
      if (replyTCP.fileSize == -2)
      {
        printf("Errore lato server.\n");
        break;
      }

      i = 0;
      printf("Provo ad aprire file locale: %s fileLength: %ld\n", replyTCP.fileName, replyTCP.fileSize);

      if ((fd_file = open(replyTCP.fileName, O_CREAT | O_WRONLY, 0777)) < 0){
        //fallita apertuara file locale
        printf("Fallita creazione del file locale, consumo lo stream\n");
        while (i < replyTCP.fileSize && (read(sd, &buf, sizeof(buf)) > 0)){
          i++;          
        }
        continue;
      }
      
      while (i < replyTCP.fileSize && (read(sd, &buf, sizeof(buf)) > 0))
      {
        write(fd_file, &buf, sizeof(buf));
        i++;
      }
      close(fd_file);
      printf("File scritto con successo: %s fileLength: %ld\n", replyTCP.fileName, replyTCP.fileSize);

    } // ricezione di tutti i file
    printf("# Fine lista file\n");

    //continuazione del ciclo
    printf("# Inserire targa o EOF per terminare: \n");

  } //while richieste

  //Chiusura socket in ricezione
  close(sd);
  printf("\nClient: termino...\n");
  exit(0);
}
