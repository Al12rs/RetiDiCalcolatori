/* 
 *
 */

const USERLENGTH = 32;
const STATUSLENGTH = 3;
const LINELENGTH = 128;
const NUM_USERS = 4;
const MAX_ELEMENTS = 5;

struct Utente{
	char nome[USERLENGTH];
};

struct Stanza{
	char nome[LINELENGTH];
	char stato[STATUSLENGTH];
	Utente utenti[NUM_USERS];			
};

typedef Utente Input1;

struct Input2{
	char nome[LINELENGTH];
};

struct Output1{
	int esito;
  Stanza stanzeLiberate [MAX_ELEMENTS];
};

program RPCPROG {
	version RPCVERS {
    Output1 ELIMINA_UTENTE(Input1) = 1;
		int ELIMINA_STANZA(Input2) = 2;
	} = 1;
} = 0x20000015;
