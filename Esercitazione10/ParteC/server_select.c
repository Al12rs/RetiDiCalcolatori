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

#define LINE_LENGTH 128
#define WORD_LENGTH 32
#define MAX_SINGERS 16

void gestore(int signo)
{
  int stato;
  printf("esecuzione gestore di SIGCHLD\n");
  wait(&stato);
}

/*#define max(a, b) ((a) > (b) ? (a) : (b))

typedef struct
{
    char fileName[LINE_LENGTH];
    char parola[WORD_LENGTH];
} ReqUDP;

int deleteWord(FILE *fin, FILE *fout, char *parola)
{
    int result = 1;
    char ch, string[128];
    while (fscanf(fin, "%s", string) != EOF)
    {
        if (strcmp(string, parola))
        { //parola diversa
            ch = fgetc(fin);
            fwrite(string, strlen(string), 1, fout);
            fputc(ch, fout);
            //fwrite(' ', sizeof(char), 1,fout);
        }
    }
    return result;
}
*/
typedef struct
{
  char singerName[WORD_LENGTH];
} UDPRequest;

typedef struct
{
  char singerName[WORD_LENGTH];
  char category[WORD_LENGTH];
  int voto;
  char fileAudio[LINE_LENGTH];
} RigaTabella;

RigaTabella tabella[MAX_SINGERS];

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

int main(int argc, char **argv)
{
  struct sockaddr_in cliaddr, servaddr;
  struct hostent *hostTcp, *hostUdp;
  int port, listen_sd, conn_sd, udp_sd, nread, maxfdp1, len;
  const int on = 1;
  fd_set rset;
  UDPRequest req;
  FILE *fd_sorg_udp, *fd_temp_udp;
  char charBuff[2], newDir[LINE_LENGTH], fileNameTemp[LINE_LENGTH], fileName[LINE_LENGTH], dir[LINE_LENGTH];
  DIR *dir1, *dir2, *dir3;
  struct dirent *dd1, *dd2;

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

  if (setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
  {
    perror("set opzioni socket d'ascolto");
    exit(3);
  }
  printf("Server: set opzioni socket d'ascolto ok\n");

  if (bind(listen_sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
  {
    perror("bind socket d'ascolto");
    exit(3);
  }
  printf("Server: bind socket d'ascolto ok\n");

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
    if (FD_ISSET(udp_sd, &rset))
    {
      printf("Ricevuta richiesta di UDP: eliminazione di una parola\n");
      len = sizeof(struct sockaddr_in);

      if (recvfrom(udp_sd, &req, sizeof(req), 0, (struct sockaddr *)&cliaddr, &len) < 0)
      {
        perror("recvfrom ");
        continue;
      }

      printf("Operazione richiesta: #NomeOperazione. Entità in esame: %s\n", req.singerName);

      hostUdp = gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
      if (hostUdp == NULL)
        printf("client host information not found\n");
      else
        printf("Operazione richiesta da: %s %i\n", hostUdp->h_name, (unsigned)ntohs(cliaddr.sin_port));

      //OPERAZIONI SULLE STRUTTURE
      int ris = 0;
      fd_sorg_udp = fopen(req.fileName, "rt");
      fileNameTemp[0] = '\0';
      strcat(fileNameTemp, req.fileName);
      strcat(fileNameTemp, "_temp");
      fd_temp_udp = fopen(fileNameTemp, "wb");
      if (!fd_temp_udp || !fd_sorg_udp)
      {
        perror("Errore apertura file");
        ris = -1;
      }
      else
        ris = deleteWord(fd_sorg_udp, fd_temp_udp, req.parola);

      printf("Nel file %s sono state eliminate le parole uguali a %s\n", req.fileName, req.parola);
      if (sendto(udp_sd, &ris, sizeof(int), 0, (struct sockaddr *)&cliaddr, len) < 0)
      {
        perror("sendto ");
        continue;
      }
      printf("SERVER: libero e riavvio.\n");
      fclose(fd_sorg_udp);
      fclose(fd_temp_udp);
    }
    /* GESTIONE RICHIESTE TCP  ----------------------------- */
    if (FD_ISSET(listen_sd, &rset))
    {
      printf("Ricevuta richiesta TCP: file del direttorio secondo lvl\n");
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
      if (fork() == 0)
      {
        close(listen_sd);
        hostTcp = gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
        if (hostTcp == NULL)
        {
          printf("client host information not found\n");
          close(conn_sd);
          exit(6);
        }
        else
          printf("Server (figlio): host client e' %s \n", hostTcp->h_name);

        // Leggo la richiesta del client
        while ((nread = read(conn_sd, dir, sizeof(dir))) > 0)
        {
          printf("Server (figlio):letti %d char \n", nread);
          printf("Server (figlio): direttorio richiesto: %s\n", dir);

          char risp;
          if ((dir1 = opendir(dir)) != NULL)
          { // direttorio presente
            risp = 'S';
            printf("Invio risposta affermativa al client\n");
            write(conn_sd, &risp, sizeof(char));
            while ((dd1 = readdir(dir1)) != NULL)
            {
              if (strcmp(dd1->d_name, ".") != 0 && strcmp(dd1->d_name, "..") != 0)
              {

                //build new path
                newDir[0] = '\0';
                strcat(newDir, dir);
                strcat(newDir, "/");
                strcat(newDir, dd1->d_name);
                //printf("Test apertura dir su %s\n",newDir);
                if ((dir2 = opendir(newDir)) != NULL)
                { //dir sec lvl
                  printf("Ciclo dir sec lvl %s\n", newDir);
                  while ((dd2 = readdir(dir2)) != NULL)
                  {
                    if (strcmp(dd2->d_name, ".") != 0 && strcmp(dd2->d_name, "..") != 0)
                    {

                      //build new path
                      strcat(newDir, "/");
                      strcat(newDir, dd2->d_name);
                      printf("Test apertura dir su %s\n", newDir);
                      if ((dir3 = opendir(newDir)) == NULL)
                      { // file of sec lvlv
                        printf("%s è un file di sec lvl \n", dd2->d_name);
                        strcpy(fileName, dd2->d_name);
                        strcat(fileName, "\0");
                        printf("Invio nome fileName: %s\n", fileName);
                        if (write(conn_sd, fileName, (strlen(fileName) + 1)) < 0)
                        {
                          perror("Errore nell'invio del nome file\n");
                          continue;
                        }
                      } //if file 2 lvl
                    }   //if not . and .. 2 lvl
                  }     //while in 2 lvl
                  printf("Fine invio\n");
                  risp = '#';
                  write(conn_sd, &risp, sizeof(char));
                } //if dir 2 lvl
              }   //if not . and .. 1 lvl
            }     //while frst lvl
          }       //if open dir 1 lvl
          else
          { //err apertura dir
            risp = 'N';
            printf("Invio risposta negativa al client per dir %s \n", dir);
            write(conn_sd, &risp, sizeof(char));
          }
        } //while read req

        // Libero risorse
        printf("Figlio TCP terminato, libero risorse e chiudo. \n");
        close(conn_sd);
        exit(0);
      }               //if fork
      close(conn_sd); //padre
    }                 //if TCP
  }                   //for
} //main