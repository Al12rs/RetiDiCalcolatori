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
int contaLineInFile(char *linea, int fd_file)
{
  char buffer[LINELENGTH];
  char character[2];
  int nread;
  char lineaDaConfrontare[LINELENGTH];
  int result = 0;
  lineaDaConfrontare[0] = '\0';
  character[1] = '\0';
  while ((nread = read(fd_file, character, sizeof(char))) > 0)
  {
    if(character[0] == '\n'){
      if (strcmp(lineaDaConfrontare, linea) == 0){
        //trovata occorrenza
        result++;
      }
      lineaDaConfrontare[0] = '\0';
    }else{
      strcat(lineaDaConfrontare, character);      
    }
  }  
  return result;
} //contaLineInFile

//REMOTE PROCEDURE CALL 1
int *conta_occorenze_linea_1_svc(Input1 *input1, struct svc_req *rp)
{
  static int result;
  DIR *dir;
  struct dirent *dd;
  int i;
  int file;
  char filePath[256];
  strcpy(filePath, "./");

  result = -1;

  printf("# Apertura direttorio \n");
  if ((dir = opendir(".")) == NULL)
  {
    perror("# Errore apertura directory:\n");
    return (&result);
  }
  result = 0;
  //ciclo sui file in dir
  while ((dd = readdir(dir)) != NULL)
  {
    strcpy(filePath, "./");
    printf("# Dirpath: %s\n", filePath);

    strcat(filePath, dd->d_name);
    if ((file = open(filePath, O_RDONLY)) < 0)
    {
      printf("# File non aperto d_name: %s\n", dd->d_name);
      printf("# File non aperto filepath: %s\n", filePath);
      perror("# Errore apertura file :\n");
      continue;
    }
    else
    {
      printf("# File aperto d_name: %s\n", dd->d_name);
      printf("# File aperto filepath: %s\n", filePath);
      result += contaLineInFile(input1->linea, file);
    }
    printf("# Primadi fclose\n");
    close(file);
  } //while readDir

  return &result;
} //rpc 1

//REMOTE PROCEDURE CALL 2
Output2 *lista_file_prefisso_1_svc(Input2 *input2, struct svc_req *rp)
{
  static Output2 result;
  DIR *dir;
  struct dirent *fileDirent;
  int prefixLength = strlen(input2->prefisso);

  printf("# Richiesti file con prefisso %s in direttorio %s\n", input2->prefisso, input2->direttorio);

  result.numFiles = -1;
  printf("# Apertura direttorio\n");
  if ((dir = opendir(input2->direttorio)) == NULL)
  {
    perror("# Errore apertura direttorio:\n");
    return (&result);
  }

  result.numFiles = 0;
  
  //ciclo sui file in dir
  while ((fileDirent = readdir(dir)) != NULL && (result.numFiles <= N))
  {
    if (strlen(fileDirent->d_name) >= prefixLength)
    {
      if (strncmp(fileDirent->d_name, input2->prefisso, prefixLength) == 0)
      {
        //add filename to list (strdup fa malloc)
        strcpy(result.fileList[result.numFiles].name, fileDirent->d_name);
        result.numFiles++;
      }
    }
  } //while readdir
  printf("# Invio Risultato\n");
  return &result;
} //rpc 2
