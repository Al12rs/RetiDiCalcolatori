/* 
 *
 */

const PATHLENGTH = 256;
const LINELENGTH = 256;
const MAX_ELEMENTS = 10;

struct Input1{
	char descrizione [LINELENGTH];
	char tipo [LINELENGTH];
	char data [LINELENGTH];
	char luogo [LINELENGTH];
	int disponibilita;
	int prezzo;
};

struct Input2{
	char descrizione [LINELENGTH];
	int numBiglietti;
};

typedef Input1 Evento;

/*struct Output2{
  FileString fileList [N];
  int numFiles;
};*/

program RPCPROG {
	version RPCVERS {
    int INSERIMENTO_EVENTO(Input1) = 1;
		int ACQUISTA_BIGLIETTI(Input2) = 2;
	} = 1;
} = 0x20000015;
