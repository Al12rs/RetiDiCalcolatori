/* 
 *
 */

const PATHLENGTH = 256;
const LINELENGTH = 256;
const NAMELENGTH = 256;
const N = 6;

struct Input1{
	char linea [LINELENGTH];
};

struct Input2{
	char direttorio [PATHLENGTH];
	char prefisso [11];
};

struct FileString{
	char name [NAMELENGTH];
};

struct Output2{
  FileString fileList [N];
  int numFiles;
};

program RPCPROG {
	version RPCVERS {
    int CONTA_OCCORENZE_LINEA(Input1) = 1;
		Output2 LISTA_FILE_PREFISSO(Input2) = 2;
	} = 1;
} = 0x20000015;
