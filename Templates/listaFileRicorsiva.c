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

#define PATH_LENGTH 256
#define LINE_LENGTH 32
#define N 3


//misleading name: not exactly a fileName but a filePath
typedef struct nomeFile
{
  char fileName[PATH_LENGTH];
} NomeFile;

//auxiliaries functions/procedures
int is_regular_file(const char *path)
{
  struct stat path_stat;
  stat(path, &path_stat);
  return S_ISREG(path_stat.st_mode);
}

int listFilesInDirRecursive(char currentDir[], NomeFile listaFile[], int index)
{
  DIR *dir;
  struct dirent *dd;
  char filePath[PATH_LENGTH];
  dir = opendir(currentDir);
  while ((dd = readdir(dir)) != NULL)
  {
    if(strcmp(dd->d_name, ".") == 0 || strcmp(dd->d_name, "..") == 0)
    {
      continue;
    }
    strcpy(filePath, currentDir);
    strcat(filePath, dd->d_name);
    if (is_regular_file(filePath))
    {
      printf("file regular %s\n", filePath);
      if (index >= N)
      {
        return index;
      }
      else
      {
        strcpy(listaFile[index].fileName, filePath);
        printf("Aggiunto all'array %s in posizione %d\n", listaFile[index].fileName, index);
        index++;
      }
    }
    else
    {
      strcat(filePath, "/");
      index = listFilesInDirRecursive(filePath, listaFile, index);
    }
  }
  return index;
}

int main(int argc, char **argv)
{
  char originalDirectory[PATH_LENGTH];
  NomeFile listaFile[N];
  int index, i;
  strcpy(originalDirectory, "/home/alexb/Test/");
  printf("Post strcpy\n");
  if (originalDirectory[strlen(originalDirectory) - 1] != '/')
  {
    strcat(originalDirectory, "/");
  }
  index = 0;
  printf("inizio ricorsione sul direttorio: %s\n", originalDirectory);
  index = listFilesInDirRecursive(originalDirectory, listaFile, index);
  printf("finita ricorsione\n");
  for (i = 0; i < index; i++)
  {
    printf("File %d di nome: %s\n", i + 1, listaFile[i].fileName);
  }
  return 0;
}