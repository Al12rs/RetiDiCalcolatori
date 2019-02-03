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

//REMOTE PROCEDURE CALL 1
int *elimina_occorrenze_1_svc(Input1 *input1, struct svc_req *rp)
{
  static int result1;
  int i;
  int fd_original, fd_edited, nread, counter;
  char editedPath[PATHLENGTH];
  char buf;
  int prevWasDigit;

  strcpy(editedPath, input1->filePath);
  strcat(editedPath, ".edited");

  result1 = -1;
  counter = 0;

  if ((fd_original = open(input1->filePath, O_RDONLY)) < 0)
  {
    printf("# File non aperto filepath: %s\n", input1->filePath);
    perror("# Errore apertura file :\n");
    return &result1;
  }
  if ((fd_edited = open(editedPath, O_WRONLY | O_CREAT)) < 0)
  {
    printf("# File non aperto filepath: %s\n", editedPath);
    perror("# Errore apertura file :\n");
    return &result1;
  }

  prevWasDigit = 0;
  while ((nread = read(fd_original, &buf, sizeof(char))) > 0)
  {
    if(buf < '0' || buf >'9'){
      if (prevWasDigit){
        //appena finito di eliminare un numero
        prevWasDigit = 0;
        counter++;
      }
      if (write(fd_edited, &buf, sizeof(char)) < 0)
      {
        printf("Errore sulla scrittura");
        return &result1;
      }
    } else{
      prevWasDigit = 1;
    }
  }
  if (prevWasDigit){
    counter++;
  }

  close(fd_original);
  close(fd_edited);
  if(rename(editedPath, input1->filePath) < 0){
    printf("Errore sulla sovrascrittura");
    return &result1;
  }

  result1 = counter;
  return &result1;
} //rpc 1

//REMOTE PROCEDURE CALL 2
Output2 *lista_file_dispari_1_svc(Input2 *input2, struct svc_req *rp)
{
  static Output2 result2;
  int i, lastDigitIndex;
  DIR *dir;
  struct dirent *fileDirent;

  printf("# Richiesti file in direttorio %s\n", input2->direttorio);

  result2.numFiles = -1;
  printf("# Apertura direttorio\n");
  if ((dir = opendir(input2->direttorio)) == NULL)
  {
    perror("# Errore apertura direttorio:\n");
    return (&result2);
  }

  result2.numFiles = 0;
  
  //ciclo sui file in dir
  while ((fileDirent = readdir(dir)) != NULL && (result2.numFiles <= N))
  {
    lastDigitIndex = 0;
    for (i = 0; i < strlen(fileDirent->d_name); i++)
    {
      if (fileDirent->d_name[i] < '0' || fileDirent->d_name[i] > '9'){
        lastDigitIndex = i - 1;
        break;
      }
    }
    if (lastDigitIndex >= 0 && (fileDirent->d_name[lastDigitIndex]=='1' ||
        fileDirent->d_name[lastDigitIndex]=='3' || fileDirent->d_name[lastDigitIndex]=='5' ||
        fileDirent->d_name[lastDigitIndex]=='7' || fileDirent->d_name[lastDigitIndex]=='9'))
    {
      //Starts with number and is odd
      strcpy(result2.fileList[result2.numFiles].name, fileDirent->d_name);
      result2.numFiles++;
    }
  } //while readdir
  printf("# Invio Risultato\n");
  return &result2;
} //rpc 2
