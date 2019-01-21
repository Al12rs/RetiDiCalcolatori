/*
 *
 * 
 */

#include <stdio.h>
#include <rpc/rpc.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include "RPC_xFile.h"

//AUXILIAR FUNCTION
int contaLineInFile(char *linea, int fd)
{
  char buffer[LINELENGTH + 1];
  char lineaDaConfrontare[LINELENGTH + 1];
  int result = 0;
  strcpy(lineaDaConfrontare, linea);
  strcat(lineaDaConfrontare, "\n");
  //fgets requires size of buffer to read-1 (alex deficiente...)
  while (fgets(buffer, LINELENGTH, fd) != NULL)
  {
    if (strcmp(lineaDaConfrontare, buffer) == 0)
      result++;
  }
  return result;
}

//REMOTE PROCEDURE CALL 1
int *conta_occorenze_linea_1_svc(Input1 *input1, struct svc_req *rp)
{
  static int result;

  int nread, index;
  char car;
  char buffer;
  char stringBuffer[DIM];
  DIR *dir;
  struct dirent *dd;
  int i, fd_file;
  off_t fileSize;

  result = -1;

  printf("# Apertura direttorio corrente\n");
  if ((dir = opendir(".")) == NULL)
  {
    perror("# Errore apertura cwd:\n");
    return (&result);
  }

  result = 0;
  //ciclo sui file in dir
  while ((dd = readdir(dir)) != NULL)
  {
    fd_file = open(dd->d_name, O_RDONLY);
    if (fd_file < 0)
    {
      perror("# Errore apertura file:\n");
      continue;
    }
    else
    {
      result += contaLineInFile(input1->linea, fd_file);
    }
    close(fd_file);
  } //while readDir

  while (1)
  {
    index = 0;
    do
    { //leggo a caratteri (spazi e \n iniziali)
      nread = read(fd, &car, 1);
      *caratteri = *caratteri + 1;
      if (car == '\n') //new line
        *linee = *linee + 1;
      stringBuffer[index] = car;
      index = index + 1;
    } while (((car == ' ') || (car == '\n')) && (nread > 0));

    if (nread <= 0)
    {
      *caratteri = *caratteri - 1;
      return;
    }
    else
      buffer = car;

    while (1)
    { //leggo a caratteri tutto il resto
      nread = read(fd, &car, 1);
      if (nread > 0)
      {
        *caratteri = *caratteri + 1;
        buffer = car;

        stringBuffer[index] = car;
        index = index + 1;
      }
      else
        return;
      // CONTA OCCORRENZE PAROLA
      if ((buffer == ' ') || (buffer == '\n'))
      {
        if (buffer == '\n')
          *linee = *linee + 1;
        *parole = *parole + 1;
        /*if (strstr(stringBuffer,parola)!=NULL){
					printf("DEBUG: print occurrence %s \n", stringBuffer);
					*occorrenze = *occorrenze +1;
				}*/
        memset(stringBuffer, 0, sizeof(stringBuffer));
        index = 0;
        break;
      }
    }
  }
  return &result;
}

//REMOTE PROCEDURE CALL 2
Output2 *lista_file_prefisso_1_svc(Input2 *input2, struct svc_req *rp)
{
  static Output2 result;
  int fd_file, nread;
  char cCorr;

  result.caratteri = 0;
  result.parole = 0;
  result.linee = 0;
  result.codiceErrore = -1;

  printf("Richiesto file %s \n", *input);

  fd_file = open(*input, O_RDONLY);
  if (fd_file < 0)
  {
    printf("File inesistente\n");
    return (&result);
  }
  else
  {
    result.codiceErrore = 0;
    /* Conteggio caratteri */
    conta(fd_file, &(result.caratteri), &(result.parole), &(result.linee));

    printf("Ho letto %d caratteri, %d parole e %d linee\n", result.caratteri,
           result.parole, result.linee);
    close(fd_file); //TODO
    return (&result);
  }
}

int *conta_file_1_svc(Input *input, struct svc_req *rp)
{
  static int result = 0;
  DIR *dir;
  struct dirent *dd;
  int i, fd_file;
  off_t fileSize;

  result = -1;

  printf("Richiesto direttorio %s\n", input->direttorio);
  if ((dir = opendir(input->direttorio)) == NULL)
    return (&result);

  result = 0;
  while ((dd = readdir(dir)) != NULL)
  {

    fd_file = open(dd->d_name, O_RDONLY);
    if (fd_file < 0)
    {
      printf("File inesistente\n");
      return (&result);
    }
    fileSize = lseek(fd_file, 0L, SEEK_END);
    /*memcpy(stringBuffer, &dd->d_name[0], strlen(input->suffisso));
			stringBuffer[strlen(input->suffisso)+1] = '\0';
			if(strstr(stringBuffer,input->suffisso) != NULL)*/
    if (fileSize >= input->soglia)
    {
      printf("DEBUG: candidate file size %ld\n", fileSize);
      result++;
    }
  }
  printf("Numero totale di file nel direttorio richiesto %d\n", result);

  return (&result);
}
