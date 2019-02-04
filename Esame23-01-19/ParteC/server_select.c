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
#define PATH_LENGTH 128
#define LINE_LENGTH 64
#define PATENTE_LENGTH 5
#define TARGA_LENGTH 8
#define TIPO_LENGTH 7
#define FOLDER_LENGTH 13
#define MAX_ELEMENTS 10
#define max(a, b) ((a) > (b) ? (a) : (b))

//Structs
typedef struct Prenotazione
{
  char targa[TARGA_LENGTH];
  char patente[PATENTE_LENGTH];
  char tipo[TIPO_LENGTH];
  char folder[FOLDER_LENGTH];
} Prenotazione;

typedef struct
{
  char targa[TARGA_LENGTH];
} TCPRequest;

typedef struct
{
  char targa[TARGA_LENGTH];
  char patente[PATENTE_LENGTH];
} UDPRequest;

typedef struct
{
  char fileName[LINE_LENGTH];
  long fileSize;
} TCPReply;

//VARIABILI GLOBALI
Prenotazione tabella[MAX_ELEMENTS];

//Signal handlers
void gestore(int signo)
{
  int stato;
  printf("Esecuzione gestore di SIGCHLD\n");
  wait(&stato);
}

//auxiliaries functions/procedures
void inizializzazione()
{
  int i;
  for (i = 0; i < MAX_ELEMENTS; i++)
  {
    strcpy(tabella[i].targa, "L");
    strcpy(tabella[i].patente, "0");
    strcpy(tabella[i].tipo, "L");
    strcpy(tabella[i].folder, "L");
  }
  i = 0;
  strcpy(tabella[i].targa, "AN745NL");
  strcpy(tabella[i].patente, "00003");
  strcpy(tabella[i].tipo, "auto");
  strcpy(tabella[i].folder, "AN745NL_img/");
  i++;
  strcpy(tabella[i].targa, "FE457GF");
  strcpy(tabella[i].patente, "50006");
  strcpy(tabella[i].tipo, "camper");
  strcpy(tabella[i].folder, "FE457GF_img/");
  i++;
  strcpy(tabella[i].targa, "NU547PL");
  strcpy(tabella[i].patente, "40063");
  strcpy(tabella[i].tipo, "auto");
  strcpy(tabella[i].folder, "NU547PL_img/");
  i++;
  strcpy(tabella[i].targa, "LR897AH");
  strcpy(tabella[i].patente, "56832");
  strcpy(tabella[i].tipo, "camper");
  strcpy(tabella[i].folder, "LR897AH_img/");
  i++;
  strcpy(tabella[i].targa, "MD506DW");
  strcpy(tabella[i].patente, "00100");
  strcpy(tabella[i].tipo, "camper");
  strcpy(tabella[i].folder, "MD506DW_img/");
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
  // Se si necessita rendere il programma funzionale su sistemi
  // a differente standard bigendian/littleendian, usare htons e ntohs
  // per assicurarsi la lettura corretta dei dati.
  //user vars
  int i;
  TCPRequest reqTCP;
  TCPReply replyTCP;
  UDPRequest reqUDP;
  int replyUDP;
  int nwrite;
  DIR *dir;
  struct dirent *dd;
  int fd_file;
  char direttorio[FOLDER_LENGTH];
  char filePath[FOLDER_LENGTH + LINE_LENGTH];
  char buf;

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
      printf("Porta scorretta...");
      exit(2);
    }
  }

  //INIZIALIZZAZIONE STRUTTURE DATI
  inizializzazione();

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

      hostUdp = gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
      if (hostUdp == NULL)
        printf("client host information not found\n");
      else
        printf("Operazione richiesta da: %s %i\n", hostUdp->h_name, (unsigned)ntohs(cliaddr.sin_port));

      //ELABORAZIONE RICHIESTA UDP
      replyUDP = -1;
      for (i = 0; i < MAX_ELEMENTS; i++)
      {
        if (strcmp(tabella[i].targa, reqUDP.targa) == 0)
        {
          strcpy(tabella[i].patente, reqUDP.patente);
          printf("Cambiata linea %d\n", i);
          replyUDP = 0;
        }
      }

      printf("Invio risposta UDP: %d\n", replyUDP);
      //INVIO RISPOSTA UDP
      if (sendto(udp_sd, &replyUDP, sizeof(replyUDP), 0, (struct sockaddr *)&cliaddr, len) < 0)
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
        while ((nread = read(conn_sd, &reqTCP, sizeof(reqTCP))) > 0)
        {
          printf("# Server (figlio): letti %d bytes \n", nread);

          //si suppone che, se la targa è presente tra le prenotazioni,
          //esiste anche il direttorio con le immagini
          //preparo il path del direttorio:
          strcpy(direttorio, reqTCP.targa);
          strcat(direttorio, "_img/");

          if ((dir = opendir(direttorio)) == NULL)
          {
            replyTCP.fileSize = -2;
            if (write(conn_sd, &replyTCP, sizeof(replyTCP)) <= 0)
            {
              perror("# Errore nell'invio del risultato\n");
              //fallita comunicazione con questo client, figlio non ha più ragione d'esistere
              close(conn_sd);
              exit(1);
            }
            continue;
          }

          while ((dd = readdir(dir)) != NULL)
          {
            if (strcmp(dd->d_name, ".") == 0 || strcmp(dd->d_name, "..") == 0)
            {
              continue;
            }
            strcpy(filePath, direttorio);
            strcat(filePath, dd->d_name);
            printf("# Server (figlio): regular filePath: %s\n", filePath);

            fd_file = open(filePath, O_RDONLY);
            if (fd_file < 0)
            {
              printf("# Errore apertura file: %s\n", filePath);
              continue;
            }
            replyTCP.fileSize = lseek(fd_file, 0L, SEEK_END);
            //rewind
            lseek(fd_file, 0L, SEEK_SET);
            strcpy(replyTCP.fileName, dd->d_name);

            printf("# Server (figlio): Invio header file: %s , %ld\n", replyTCP.fileName, replyTCP.fileSize);
            if (write(conn_sd, &replyTCP, sizeof(replyTCP)) <= 0)
            {
              perror("# Errore nell'invio del risultato\n");
              //fallita comunicazione con questo client, figlio non ha più ragione d'esistere
              close(conn_sd);
              close(fd_file);
              exit(1);
            }
            for (i = 0; i < replyTCP.fileSize; i++)
            {
              nread = read(fd_file, &buf, sizeof(buf));
              nwrite = write(conn_sd, &buf, sizeof(buf));
              if (nread <= 0 || nwrite <= 0)
              {
                perror("# Errore nell'invio del file\n");
                //Protocollo di comunicazione non rispettato
                //fallita comunicazione con questo client, figlio non ha più ragione d'esistere
                close(conn_sd);
                close(fd_file);
                exit(1);
              }
            }
            close(fd_file);
            printf("Spedito file: %s\n", dd->d_name);
          } //ciclo sui file
          
          //segnalo fine di file da inviare.
          replyTCP.fileSize = -1;
          if (write(conn_sd, &replyTCP, sizeof(replyTCP)) <= 0)
          {
            perror("# Errore nell'invio del risultato\n");
            //fallita comunicazione con questo client, figlio non ha più ragione d'esistere
            close(conn_sd);
            exit(1);
          }
        } //while richieste

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