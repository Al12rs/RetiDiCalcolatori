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
#define LINE_LENGTH 64
#define MAX_ELEMENTS 10
#define max(a, b) ((a) > (b) ? (a) : (b))

//Structs
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

typedef struct
{
  Evento vettoreEventi[MAX_ELEMENTS];
  int numValidi;
} UDPAnswer;

/*
typedef struct 
{
  
} UDPRequest;

*/

//VARIABILI GLOBALI
Evento tabella[MAX_ELEMENTS];

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
  int reqUDP;
  //user vars
  int i;
  TCPRequest tcpreq;
  UDPAnswer udpReply;
  Evento tcpreply;
  int numEventi;

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

      printf("# prezzo massimo: %d\n", reqUDP);

      hostUdp = gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
      if (hostUdp == NULL)
        printf("client host information not found\n");
      else
        printf("Operazione richiesta da: %s %i\n", hostUdp->h_name, (unsigned)ntohs(cliaddr.sin_port));

      //ELABORAZIONE RICHIESTA UDP
      udpReply.numValidi = 0;
      for (i = 0; i < MAX_ELEMENTS; i++)
      {
        if (tabella[i].disponibilita > 0 && tabella[i].prezzo <= reqUDP)
        {
          udpReply.vettoreEventi[udpReply.numValidi] = tabella[i];
          udpReply.numValidi++;
        }
      }
      printf("# Trovati %d elementi\n", udpReply.numValidi);

      //INVIO RISPOSTA UDP
      if (sendto(udp_sd, &udpReply, sizeof(udpReply), 0, (struct sockaddr *)&cliaddr, len) < 0)
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
          numEventi = 0;

          for (i = 0; i < MAX_ELEMENTS; i++)
          {
            if (strcmp(tabella[i].tipo, tcpreq.tipo) == 0 && strcmp(tabella[i].luogo, tcpreq.luogo) == 0 && tabella[i].disponibilita > 0)
            {
              numEventi++;
              if (write(conn_sd, &tabella[i], sizeof(tabella[i])) <= 0)
              {
                perror("# Errore nell'invio del risultato\n");
                //fallita comunicazione con questo client, figlio non ha più ragione d'esistere
                close(conn_sd);
                exit(1);
              }
            }
          }

          //send tcpreply with disponibilta
          // =-1 to signify end of list
          // =-2 to signify no valid elements
          if(numEventi == 0)
            tcpreply.disponibilita = -2;
          else
            tcpreply.disponibilita = -1;
          strcpy(tcpreply.descrizione, "L");
          strcpy(tcpreply.tipo, "L");
          strcpy(tcpreply.data, "L");
          strcpy(tcpreply.luogo, "L");
          tcpreply.prezzo = -1;
          if (write(conn_sd, &tcpreply, sizeof(tcpreply)) <= 0)
          {
            perror("# Errore nell'invio del risultato\n");
            //fallita comunicazione con questo client, figlio non ha più ragione d'esistere
            close(conn_sd);
            exit(1);
          }

        } //while lista

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