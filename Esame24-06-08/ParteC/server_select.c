/**
  * server_select.c
  *  
  * 
  **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <regex.h>
#include <sys/stat.h>

//Defines
#define PATH_LENGTH 256
#define LINE_LENGTH 256
#define WORD_LENGTH 32
#define max(a, b) ((a) > (b) ? (a) : (b))

//Structs
typedef struct
{
  char fileName[PATH_LENGTH];
  char keyword[WORD_LENGTH]
} UDPRequest;

typedef UDPRequest TCPRequest;

typedef struct
{
  char fileName[PATH_LENGTH];
  long fileSize;
} TCPAnswer;

//Signal handlers
void gestore(int signo)
{
  int stato;
  printf("Esecuzione gestore di SIGCHLD\n");
  wait(&stato);
}

//auxiliaries functions/procedures
int is_regular_file(const char *path)
{
  struct stat path_stat;
  stat(path, &path_stat);
  return S_ISREG(path_stat.st_mode);
}

//MAIN
int main(int argc, char **argv)
{
  //template vars
  struct sockaddr_in cliaddr, servaddr;
  struct hostent *hostTcp, *hostUdp;
  int port, listen_sd, conn_sd, udp_sd, nread, maxfdp1, len;
  const int on = 1;
  fd_set rset;
  UDPRequest reqUDP;
  //user vars
  TCPRequest tcpreq;
  char risTCP[LINE_LENGTH];
  int i, trovato, risUDP;
  int fd_r, fd_w;
  char editedFileName[PATH_LENGTH], direttorio[PATH_LENGTH], vowels[11], buf[2];
  char filePath[PATH_LENGTH], dirWithTrailingSlash[PATH_LENGTH];

  char currentLine[LINE_LENGTH];
  char currentWord[WORD_LENGTH];

  int isInCurrentLine;
  DIR *dir;
  struct dirent *dd;
  int fd_file;
  long bytes;

  //CONTROLLO ARGOMENTI
  if (argc != 2)
  {
    printf("Error: %s port \n", argv[0]);
    exit(1);
  }
  else
  {
    nread = 0;
    while (argv[1][nread] != '\0')
    {
      if ((argv[1][nread] < '0') || (argv[1][nread] > '9'))
      {
        printf("Secondo argomento non intero\n");
        exit(2);
      }
      nread++;
    }
    port = atoi(argv[1]);
    if (port < 1024 || port > 65535)
    {
      printf("Porta scorretta...\n");
      exit(2);
    }
  }

  //INIZIALIZZAZIONE STRUTTURE DATI
  //inizializzazione();
  strcpy(vowels, "AaEeIiOoUu");

  //INIZIALIZZAZIONE INDIRIZZO SERVER
  memset((char *)&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(port);

  //CREAZIONE E SETTAGGI SOCKET TCP
  listen_sd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_sd < 0)
  {
    perror("creazione socket ");
    exit(3);
  }
  printf("Server: creata la socket d'ascolto per le richieste di ordinamento, fd=%d\n", listen_sd);
  //SetOptional
  if (setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
  {
    perror("set opzioni socket d'ascolto");
    exit(3);
  }
  printf("Server: set opzioni socket d'ascolto ok\n");
  //bind
  if (bind(listen_sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
  {
    perror("bind socket d'ascolto");
    exit(3);
  }
  printf("Server: bind socket d'ascolto ok\n");
  //listen
  if (listen(listen_sd, 5) < 0)
  {
    perror("listen");
    exit(3);
  }
  printf("Server: listen ok\n");

  //CREAZIONE E SETTAGGI SOCKET UDP
  udp_sd = socket(AF_INET, SOCK_DGRAM, 0);
  if (udp_sd < 0)
  {
    perror("apertura socket UDP");
    exit(4);
  }
  printf("Creata la socket UDP, fd=%d\n", udp_sd);

  if (setsockopt(udp_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
  {
    perror("set opzioni socket UDP");
    exit(4);
  }
  printf("Set opzioni socket UDP ok\n");

  if (bind(udp_sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
  {
    perror("bind socket UDP");
    exit(4);
  }
  printf("Bind socket UDP ok\n");

  //BINDING DEL GESTORE DEI FIGLI (OPERAZIONI IN PARALLELO)
  signal(SIGCHLD, gestore);

  //PULIZIA E SETTAGGIO MASCHERA DEI FILE DESCRIPTOR
  FD_ZERO(&rset);
  maxfdp1 = max(listen_sd, udp_sd) + 1;

  //CICLO DI RICEZIONE RICHIESTE
  for (;;)
  {
    FD_SET(listen_sd, &rset);
    FD_SET(udp_sd, &rset);

    if ((nread = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0)
    {
      if (errno == EINTR)
        continue;
      else
      {
        perror("select");
        exit(5);
      }
    }

    //GESTIONE RICHIESTE UDP
    //sequenziale
    if (FD_ISSET(udp_sd, &rset))
    {
      printf("Ricevuta richiesta di UDP\n");
      len = sizeof(struct sockaddr_in);

      if (recvfrom(udp_sd, &reqUDP, sizeof(reqUDP), 0, (struct sockaddr *)&cliaddr, &len) < 0)
      {
        perror("recvfrom ");
        continue;
      }

      printf("# Entità in esame: %s\n", reqUDP.fileName);

      hostUdp = gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
      if (hostUdp == NULL)
        printf("client host information not found\n");
      else
        printf("Operazione richiesta da: %s %i\n", hostUdp->h_name, (unsigned)ntohs(cliaddr.sin_port));

      //ELABORAZIONE RICHIESTA UDP

      if ((fd_r = open(reqUDP.fileName, O_RDONLY)) < 0)
      {
        risUDP = -1;
        printf("# Fallita apertura file %s.\n", reqUDP.fileName);
      }
      else
      {
        risUDP = 0;
        currentWord[0] = '\0';
        buf[1] = '\0';
        while ((nread = read(fd_r, buf, sizeof(char))) > 0)
        {
          if (buf[0] == '\n' || buf[0] == ' ')
          {
            if (strcmp(currentWord, reqUDP.keyword) == 0)
            {
              risUDP = 1;
              break;
            }
            currentWord[0] = '\0';
          }
          else
          {
            strcat(currentWord, buf);
          }
        } //while lettura file
        close(fd_r);
      }

      //INVIO RISPOSTA UDP
      if (sendto(udp_sd, &risUDP, sizeof(risUDP), 0, (struct sockaddr *)&cliaddr, len) < 0)
      {
        perror("sendto ");
        continue;
      }

      //TERMINE GESTIONE RICHIESTA UDP
      printf("SERVER: libero e riavvio.\n");

    } //GESTIONE UDP

    //GESTIONE RICHIESTE TCP
    //parallelo
    if (FD_ISSET(listen_sd, &rset))
    {
      printf("Ricevuta richiesta TCP\n");
      len = sizeof(cliaddr);
      if ((conn_sd = accept(listen_sd, (struct sockaddr *)&cliaddr, &len)) < 0)
      {
        if (errno == EINTR)
        {
          perror("Forzo la continuazione della accept");
          continue;
        }
        else
          exit(6);
      }

      //ELABORAZIONE RICHIESTA IN NUOVO PROCESSO
      if (fork() == 0)
      {
        close(listen_sd);
        hostTcp = gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
        if (hostTcp == NULL)
        {
          printf("Server: client host information not found.\n");
          close(conn_sd);
          exit(6);
        }
        else
          printf("Server (figlio): host client e' %s \n", hostTcp->h_name);

        // Leggo le richieste del client
        while ((nread = read(conn_sd, &tcpreq, sizeof(tcpreq))) > 0)
        {
          printf("# Server (figlio): letti %d bytes \n", nread);
          printf("# Server (figlio): richiesta: %s - %s\n", tcpreq.fileName, tcpreq.keyword);

          if ((fd_r = open(reqUDP.fileName, O_RDONLY)) < 0)
          {
            strcpy(risTCP, "*");
            if (write(conn_sd, risTCP, sizeof(risTCP)) <= 0)
            {
              perror("# Errore nell'invio del risultato\n");
              //fallita comunicazione con questo client, figlio non ha più ragione d'esistere
              close(conn_sd);
              exit(1);
            }
            printf("# Fallita apertura file %s.\n", tcpreq.fileName);
            continue;
          }
          else
          {
            currentWord[0] = '\0';
            currentLine[0] = '\0';
            buf[1] = '\0';
            isInCurrentLine = 0;
            while ((nread = read(fd_r, buf, sizeof(char))) > 0)
            {
              strcat(currentLine, buf);

              if (!isInCurrentLine){
                if (buf[0] == '\n' || buf[0] == ' ')
                {
                  if (strcmp(currentWord, reqUDP.keyword) == 0)
                  {
                    isInCurrentLine = 1;
                  }
                  currentWord[0] = '\0';
                }
                else
                {
                  strcat(currentWord, buf);
                }
              }

              if (buf[0] == '\n'){
                if (write(conn_sd, currentLine, sizeof(currentLine)) <= 0)
                {
                  perror("# Errore nell'invio del risultato\n");
                  //fallita comunicazione con questo client, figlio non ha più ragione d'esistere
                  close(conn_sd);
                  exit(1);
                }
                currentLine[0] = '\0';
              }
            } //while lettura file
            close(fd_r);
          }

          
          //send risTCP with "#" to signify end of lines
          strcpy(risTCP, "#");
          if (write(conn_sd, risTCP, sizeof(risTCP)) <= 0)
          {
            perror("# Errore nell'invio del risultato\n");
            //fallita comunicazione con questo client, figlio non ha più ragione d'esistere
            close(conn_sd);
            exit(1);
          }

        } //while richieste direttori

        // Libero risorse
        printf("Figlio TCP terminato, libero risorse e chiudo. \n");
        //close socket, send EOF
        close(conn_sd);

        //Terminate child
        exit(0);

      } //DEATH OF CHILD

      //chiusura socket di comunicazione del padre.
      close(conn_sd); //padre

    } //GESTIONE TCP

  } //SELECT LOOP

} //main