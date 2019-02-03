/* 
 *
 */

const PATHLENGTH = 256;
const LINELENGTH = 256;
const NAMELENGTH = 64;
const N = 6;

struct Input1{
	char filePath [PATHLENGTH];
};

struct Input2{
	char direttorio [PATHLENGTH];
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
    int ELIMINA_OCCORRENZE(Input1) = 1;
		Output2 LISTA_FILE_DISPARI(Input2) = 2;
	} = 1;
} = 0x20000015;
