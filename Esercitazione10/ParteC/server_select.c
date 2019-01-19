/**
  * server_select.c
  *  Il server discrimina due servizi con la select:
  *    + elimina le occorrenze di una parola in un file (UDP)
  *    + restituisce i nomi dei file che terminano con un suffisso
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

//Defines
#define LINE_LENGTH 128
#define WORD_LENGTH 32
#define MAX_SINGERS 16
#define max(a,b) ((a) > (b) ? (a) : (b))

//Structs
typedef struct
{
  char singerName[WORD_LENGTH];
} UDPRequest;

typedef struct
{
  char singerName[WORD_LENGTH];
  int voto; 
} TCPAnswer;

typedef struct
{
  char singerName[WORD_LENGTH];
  char category[WORD_LENGTH];
  int voto;
  char fileAudio[LINE_LENGTH];
} RigaTabella;

//Globals
RigaTabella tabella[MAX_SINGERS];

//Signal handlers
void gestore(int signo)
{
  int stato;
  printf("Esecuzione gestore di SIGCHLD\n");
  wait(&stato);
} 

//procedures
void inizializzazione()
{
  int i;
  //riempimento con valori default
  for (i = 0; i < MAX_SINGERS; i++)
  {
    strcpy(tabella[i].singerName, "L");
    strcpy(tabella[i].category, "L");
    tabella[i].voto = -1;
    strcpy(tabella[i].fileAudio, "L");
  }

  //riempimento con valori utili
  //1a riga
  strcpy(tabella[i].singerName, "Mandarino");
  strcpy(tabella[i].category,   "Campioni");
  tabella[i].voto = 1500;
  strcpy(tabella[i].fileAudio,  "vicino.avi");
  //2a riga
  strcpy(tabella[i].singerName, "Amara Bianchi");
  strcpy(tabella[i].category,   "Campioni");
  tabella[i].voto = 2000;
  strcpy(tabella[i].fileAudio,  "immobilismo.avi");
  //3a riga
  strcpy(tabella[i].singerName, "Zucchero");
  strcpy(tabella[i].category,   "NuoveProposte");
  tabella[i].voto = 550;
  strcpy(tabella[i].fileAudio,  "ascolto.avi");
  //4a riga
  strcpy(tabella[i].singerName, "Amari");
  strcpy(tabella[i].category,   "NuoveProposte");
  tabella[i].voto = 800;
  strcpy(tabella[i].fileAudio,  "cosaE.avi");
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
  int i, trovato, risUDP;
  int m;
  TCPAnswer tcpreply;

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
      printf("Ricevuta richiesta di UDP: eliminazione di una parola\n");
      len = sizeof(struct sockaddr_in);

      if (recvfrom(udp_sd, &reqUDP, sizeof(reqUDP), 0, (struct sockaddr *)&cliaddr, &len) < 0)
      {
        perror("recvfrom ");
        continue;
      }

      printf("Operazione richiesta: #NomeOperazione. Entità in esame: %s\n", reqUDP.singerName);

      hostUdp = gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
      if (hostUdp == NULL)
        printf("client host information not found\n");
      else
        printf("Operazione richiesta da: %s %i\n", hostUdp->h_name, (unsigned)ntohs(cliaddr.sin_port));

      //ELABORAZIONE RICHIESTA UDP
      trovato=0;
      for(i=0; i<MAX_SINGERS; i++){
        if (strcmp(tabella[i].singerName, reqUDP.singerName) == 0) {
          trovato = 1;
          break;
        }
      }
      if(trovato){
        tabella[i].voto++;
        risUDP = tabella[i].voto;
        printf("# Voti ottenuti da %s: %d\n", reqUDP.singerName, risUDP);
                                                                                                            
      }else{
        risUDP = -1;
        printf("# Cantante non trovato.\n");       
      }

      //INVIO RISPOSTA UDP
      if (sendto(udp_sd, &risUDP, sizeof(int), 0, (struct sockaddr *)&cliaddr, len) < 0)
      {
        perror("sendto ");
        continue;
      }

      //TERMINE GESTIONE RICHIESTA UDP            
      printf("SERVER: libero e riavvio.\n");
      
    }//GESTIONE UDP

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

        // Leggo la richiesta del client
        while ((nread = read(conn_sd, &m, sizeof(m))) > 0)
        {
          printf("# Server (figlio): letti %d bytes \n", nread);
          printf("# Server (figlio): soglia voti M: %d\n", m);

          for(i=0; i<MAX_SINGERS; i++){
            if(tabella[i].voto >= m){
              strcpy(tcpreply.singerName, tabella[i].singerName);
              tcpreply.voto = tabella[i].voto;
              //write reply
              if (write(conn_sd, &tcpreply, sizeof(tcpreply)) < 0)
                {
                  perror("Errore nell'invio del risultato\n");
                  continue;
                }              
            }
          }

        } //while read reqTCP

        // Libero risorse
        printf("Figlio TCP terminato, libero risorse e chiudo. \n");
        //close socket, send EOF
        close(conn_sd);

        //Terminate child
        exit(0);

      }//DEATH OF CHILD

      //chiusura socket di comunicazione del padre.
      close(conn_sd);//padre

    }//GESTIONE TCP

  }//SELECT LOOP

} //main