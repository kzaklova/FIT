/*
 * Soubor: proj4.c
 * Datum: 10/12/2011
 * Autor: Katerina Zaklova, xzaklo00@stud.fit.vutbr.cz
 * Projekt: Ceske razeni
 * Popis: Projekt provadi razeni znaku nad ceskou znakovou sadou,
 * zaroven je mozne nastavi lokalizaci.
 */

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <locale.h>

// konec radku
#define ENTER L'\n'
// delka retezce pro alokaci
#define STR_LEN 16
// pismenko CH
#define CH 15
// tabulky znaku - usnadneni pruchodu
#define TAB1 0
#define TAB2 1

// prvky se nelisi
#define EQ 0
// prvni je vetsi nez druhy
#define GRTR 1
// prvni je mensi nez druhy
#define LESS -1
// konstanta pro realokaci
#define ALLOC 2


// tabulka pro prvni pruchod
const wint_t table[]=
{
// bile znaky
    [L'\0'] = 0,        [L' '] = 1,        [L'\t'] = 1,
// pismena
    [(wchar_t) L'a'] = 2,         [(wchar_t) L'A'] = 2,
    [(wchar_t) L'\u00E1'] = 2,    [(wchar_t) L'\u00C1'] = 2, // dlouhe a
    [(wchar_t) L'b'] = 4,         [(wchar_t) L'B'] = 4,
    [(wchar_t) L'c'] = 5,         [(wchar_t) L'C'] = 5,
    [(wchar_t) L'\u010D'] = 6,    [(wchar_t) L'\u010C'] = 6, // c s hackem
    [(wchar_t) L'd'] = 7,         [(wchar_t) L'D'] = 7,
    [(wchar_t) L'\u010F'] = 7,    [(wchar_t) L'\u010E'] = 7, // d s hackem
    [(wchar_t) L'e'] = 9,         [(wchar_t) L'E'] = 9,
    [(wchar_t) L'\u00E9'] = 9,    [(wchar_t) L'\u00C9'] = 9, // dlouhe e
    [(wchar_t) L'\u011B'] = 9,    [(wchar_t) L'\u011A'] = 9, // e s hackem
    [(wchar_t) L'f'] = 12,        [(wchar_t) L'F'] = 12,
    [(wchar_t) L'g'] = 13,        [(wchar_t) L'G'] = 13,
    [(wchar_t) L'h'] = 14,        [(wchar_t) L'H'] = 14,
    //ch
    [(wchar_t) L'i'] = 16,        [(wchar_t) L'I'] = 16,
    [(wchar_t) L'\u00ED'] = 16,   [(wchar_t) L'\u00CD'] = 16, // dlouhe i
    [(wchar_t) L'j'] = 18,        [(wchar_t) L'J'] = 18,
    [(wchar_t) L'k'] = 19,        [(wchar_t) L'K'] = 19,
    [(wchar_t) L'l'] = 20,        [(wchar_t) L'L'] = 20,
    [(wchar_t) L'm'] = 21,        [(wchar_t) L'M'] = 21,
    [(wchar_t) L'n'] = 22,        [(wchar_t) L'N'] = 22,
    [(wchar_t) L'\u0148'] = 22,   [(wchar_t) L'\u0147'] = 22, // n s hackem
    [(wchar_t) L'o'] = 24,        [(wchar_t) L'O'] = 24,
    [(wchar_t) L'\u00F3'] = 24,   [(wchar_t) L'\u00D3'] = 24, // dlouhe o
    [(wchar_t) L'p'] = 26,        [(wchar_t) L'P'] = 26,
    [(wchar_t) L'q'] = 27,        [(wchar_t) L'Q'] = 27,
    [(wchar_t) L'r'] = 28,        [(wchar_t) L'R'] = 28,
    [(wchar_t) L'\u0159'] = 29,   [(wchar_t) L'\u0158'] = 29, // r s hackem
    [(wchar_t) L's'] = 30,        [(wchar_t) L'S'] = 30,
    [(wchar_t) L'\u0161'] = 31,   [(wchar_t) L'\u0160'] = 31, // s s hackem
    [(wchar_t) L't'] = 32,        [(wchar_t) L'T'] = 32,
    [(wchar_t) L'\u0165'] = 32,   [(wchar_t) L'\u0164'] = 32, // t s hackem
    [(wchar_t) L'u'] = 34,        [(wchar_t) L'U'] = 34,
    [(wchar_t) L'\u00FA'] = 34,   [(wchar_t) L'\u00DA'] = 34, // dlouhe u
    [(wchar_t) L'\u016F'] = 34,   [(wchar_t) L'\u016E'] = 34, // u s krouzkem
    [(wchar_t) L'v'] = 37,        [(wchar_t) L'V'] = 37,
    [(wchar_t) L'w'] = 38,        [(wchar_t) L'W'] = 38,
    [(wchar_t) L'x'] = 39,        [(wchar_t) L'X'] = 39,
    [(wchar_t) L'y'] = 40,        [(wchar_t) L'Y'] = 40,
    [(wchar_t) L'\u00FD'] = 40,   [(wchar_t) L'\u00DD'] = 40, // dlouhe y
    [(wchar_t) L'z'] = 42,        [(wchar_t) L'Z'] = 42,
    [(wchar_t) L'\u017E'] = 43,   [(wchar_t) L'\u017D'] = 43, // z s hackem
// cisla
    [(wchar_t) L'0'] = 44,        [(wchar_t) L'1'] = 45,
    [(wchar_t) L'2'] = 46,        [(wchar_t) L'3'] = 47,
    [(wchar_t) L'4'] = 48,        [(wchar_t) L'5'] = 49,
    [(wchar_t) L'6'] = 50,        [(wchar_t) L'7'] = 51,
    [(wchar_t) L'8'] = 52,        [(wchar_t) L'9'] = 53,
// ostatni znaky
    [(wchar_t) L'.'] = 54,        [(wchar_t) L','] = 55,
    [(wchar_t) L';'] = 56,        [(wchar_t) L'?'] = 57,
    [(wchar_t) L'!'] = 58,        [(wchar_t) L':'] = 59,
    [(wchar_t) L'"'] = 60,        [(wchar_t) L'-'] = 61,
    [(wchar_t) L'|'] = 62,        [(wchar_t) L'/'] = 63,
    [(wchar_t) L'\\'] = 64,       [(wchar_t) L'('] = 65,
    [(wchar_t) L')'] = 66,        [(wchar_t) L'['] = 67,
    [(wchar_t) L']'] = 68,        [(wchar_t) L'<'] = 69,
    [(wchar_t) L'>'] = 70,        [(wchar_t) L'{'] = 71,
    [(wchar_t) L'}'] = 72,        [(wchar_t) L'&'] = 73,
    [(wchar_t) L'%'] = 74,        [(wchar_t) L'$'] = 75,
    [(wchar_t) L'='] = 76,        [(wchar_t) L'+'] = 77,
    [(wchar_t) L'*'] = 78,        [(wchar_t) L'#'] = 79,
    [(wchar_t) L'~'] = 80,
};

// tabulka pro druhy pruchod
const wint_t table2[]=
{
// bile znaky
    [L'\0'] = 0,        [L' '] = 1,        [L'\t'] = 1,
// pismena
    [(wchar_t) L'a'] = 2,         [(wchar_t) L'A'] = 2,
    [(wchar_t) L'\u00E1'] = 3,    [(wchar_t) L'\u00C1'] = 3, // dlouhe a
    [(wchar_t) L'b'] = 4,         [(wchar_t) L'B'] = 4,
    [(wchar_t) L'c'] = 5,         [(wchar_t) L'C'] = 5,
    [(wchar_t) L'\u010D'] = 6,    [(wchar_t) L'\u010C'] = 6, // c s hackem
    [(wchar_t) L'd'] = 7,         [(wchar_t) L'D'] = 7,
    [(wchar_t) L'\u010F'] = 8,    [(wchar_t) L'\u010E'] = 8,  // d s hackem
    [(wchar_t) L'e'] = 9,         [(wchar_t) L'E'] = 9,
    [(wchar_t) L'\u00E9'] = 10,   [(wchar_t) L'\u00C9'] = 10, // dlouhe e
    [(wchar_t) L'\u011B'] = 11,   [(wchar_t) L'\u011A'] = 11, // e s hackem
    [(wchar_t) L'f'] = 12,        [(wchar_t) L'F'] = 12,
    [(wchar_t) L'g'] = 13,        [(wchar_t) L'G'] = 13,
    [(wchar_t) L'h'] = 14,        [(wchar_t) L'H'] = 14,
    //ch
    [(wchar_t) L'i'] = 16,        [(wchar_t) L'I'] = 16,
    [(wchar_t) L'\u00ED'] = 17,   [(wchar_t) L'\u00CD'] = 17, // dlouhe i
    [(wchar_t) L'j'] = 18,        [(wchar_t) L'J'] = 18,
    [(wchar_t) L'k'] = 19,        [(wchar_t) L'K'] = 19,
    [(wchar_t) L'l'] = 20,        [(wchar_t) L'L'] = 20,
    [(wchar_t) L'm'] = 21,        [(wchar_t) L'M'] = 21,
    [(wchar_t) L'n'] = 22,        [(wchar_t) L'N'] = 22,
    [(wchar_t) L'\u0148'] = 23,   [(wchar_t) L'\u0147'] = 23, // n s hackem
    [(wchar_t) L'o'] = 24,        [(wchar_t) L'O'] = 24,
    [(wchar_t) L'\u00F3'] = 25,   [(wchar_t) L'\u00D3'] = 25, // dlouhe o
    [(wchar_t) L'p'] = 26,        [(wchar_t) L'P'] = 26,
    [(wchar_t) L'q'] = 27,        [(wchar_t) L'Q'] = 27,
    [(wchar_t) L'r'] = 28,        [(wchar_t) L'R'] = 28,
    [(wchar_t) L'\u0159'] = 29,   [(wchar_t) L'\u0158'] = 29, // r s hackem
    [(wchar_t) L's'] = 30,        [(wchar_t) L'S'] = 30,
    [(wchar_t) L'\u0161'] = 31,   [(wchar_t) L'\u0160'] = 31, // s s hackem
    [(wchar_t) L't'] = 32,        [(wchar_t) L'T'] = 32,
    [(wchar_t) L'\u0165'] = 33,   [(wchar_t) L'\u0164'] = 33, // t s hackem
    [(wchar_t) L'u'] = 34,        [(wchar_t) L'U'] = 34,
    [(wchar_t) L'\u00FA'] = 35,   [(wchar_t) L'\u00DA'] = 35, // dlouhe u
    [(wchar_t) L'\u016F'] = 36,   [(wchar_t) L'\u016E'] = 36, // u s krouzkem
    [(wchar_t) L'v'] = 37,        [(wchar_t) L'V'] = 37,
    [(wchar_t) L'w'] = 38,        [(wchar_t) L'W'] = 38,
    [(wchar_t) L'x'] = 39,        [(wchar_t) L'X'] = 39,
    [(wchar_t) L'y'] = 40,        [(wchar_t) L'Y'] = 40,
    [(wchar_t) L'\u00FD'] = 41,   [(wchar_t) L'\u00DD'] = 41, // dlouhe y
    [(wchar_t) L'z'] = 42,        [(wchar_t) L'Z'] = 42,
    [(wchar_t) L'\u017E'] = 43,   [(wchar_t) L'\u017D'] = 43, // z s hackem
// cisla
    [(wchar_t) L'0'] = 44,        [(wchar_t) L'1'] = 45,
    [(wchar_t) L'2'] = 46,        [(wchar_t) L'3'] = 47,
    [(wchar_t) L'4'] = 48,        [(wchar_t) L'5'] = 49,
    [(wchar_t) L'6'] = 50,        [(wchar_t) L'7'] = 51,
    [(wchar_t) L'8'] = 52,        [(wchar_t) L'9'] = 53,
// ostatni znaky
    [(wchar_t) L'.'] = 54,        [(wchar_t) L','] = 55,
    [(wchar_t) L';'] = 56,        [(wchar_t) L'?'] = 57,
    [(wchar_t) L'!'] = 58,        [(wchar_t) L':'] = 59,
    [(wchar_t) L'"'] = 60,        [(wchar_t) L'-'] = 61,
    [(wchar_t) L'|'] = 62,        [(wchar_t) L'/'] = 63,
    [(wchar_t) L'\\'] = 64,       [(wchar_t) L'('] = 65,
    [(wchar_t) L')'] = 66,        [(wchar_t) L'['] = 67,
    [(wchar_t) L']'] = 68,        [(wchar_t) L'<'] = 69,
    [(wchar_t) L'>'] = 70,        [(wchar_t) L'{'] = 71,
    [(wchar_t) L'}'] = 72,        [(wchar_t) L'&'] = 73,
    [(wchar_t) L'%'] = 74,        [(wchar_t) L'$'] = 75,
    [(wchar_t) L'='] = 76,        [(wchar_t) L'+'] = 77,
    [(wchar_t) L'*'] = 78,        [(wchar_t) L'#'] = 79,
    [(wchar_t) L'~'] = 80,
};

// usnadneni prace s tabulkami
const wint_t *TAB[] = { [TAB1] = table, [TAB2] = table2 };

/** Kody chyb programu */
enum tecodes
{
	EOK = 0,     /**< Bez chyby */
	EHELP,       /**< Chybne parametry */
	EINPUT,      /**< Chyba na vstupu */
	EOUTPUT,     /**< Chyba vystupu */
	EFILE,       /**< Chybny vstupni soubor */
	EINSERT,     /**< Chyba insertu */
	ELOC,        /**< Chyba lokalizace */
	EALLOC,      /**< Chyba pri alokaci */
	EUNKNOWN,    /**< Neznama chyba */
};

/** Stavove kody programu */
enum tstates
{
	CHELP,      /**< Napoveda */
	CSORT,      /**< Serazeni seznamu */
	CLOC,       /**< Podpora lokalizace */
};


/** Chybova hlaseni odpovidajici chybovym kodum. */
const char *ECODEMSG[] =
{
	/* EOK */
	"Vse v poradku.\n",
	/* EHELP */
	"Chybne parametry prikazoveho radku, pro napovedu zadejte parametr -h!\n",
	/* EINPUT */
	"Chybny vstup!\n",
	/* EOUTPUT */
	"Nepodarilo se zapsat do vystupniho souboru!\n",
	/* EFILE */
	"Chyba pri nacitani vstupniho souboru!\n",
	/* EINSERT */
	"Nepodarilo se vlozit prvek!\n",
	/* ELOC */
	"Chyba nastaveni lokalizace!\n",
	/* EALLOC */
	"Chyba pri alokaci!\n",
	/* EUNKNOWN */
	"Neznama chyba!\n",

};

const char *HELPMSG =
	"  ____________\n"
	"||            ||\n"
	"||CESKE RAZENI||\n"
	"||____________||\n\n"
	"Autor: Katerina Zaklova (c) 2011\n"
	"-------------------------------------------------------------------\n"
	"Program provadi razeni ceskych znaku v ruznem nastaveni lokalizace.\n"
	"-------------------------------------------------------------------\n"
	"Pouziti: proj4 [--loc LOCALE] soubor1.txt soubor2.txt\n"
	"LOCALE je volitelny parametr, reprezentuje znakovou sadu vstupniho\n"
	"souboru\n"
	"soubor1.txt je soubor se vstupnim textem\n"
	"soubor2.txt je soubor, kam bude ulozen serazeny vystup\n";


// Struktura - parametry prikazove radky
typedef struct params
{
	char *filename1;	/**< Vstupni soubor */
	char *filename2;	/**< Vystupni soubor */
	char *loc;			/**< Lokalizace */
	int ecode;			/**< Chybovy kod programu, odpovida vyctu tecodes. */
	int state;			/**< Stavovy kod programu, odpovida vyctu tstates. */
} TParams;

// Struktura pro polozku seznamu
typedef struct tElem {
	struct tElem *lptr;
	struct tElem *rptr;
	wchar_t *data;
} *tElemPtr;

// Struktura pro seznam
typedef struct {
	tElemPtr Act;
	tElemPtr First;
	tElemPtr Last;
	int length;
} tList;

/**
 * Vytiskne hlaseni odpovidajici chybovemu kodu
 * @param ecode chybovy kod
 */
void printECode(int ecode)
{
	if (ecode < EOK || ecode > EALLOC)
	{ ecode = EUNKNOWN; }

	fprintf(stderr, "%s", ECODEMSG[ecode]);
}

/**
 * Zpracuje argumenty prikazoveho radku
 * @param argc pocet parametru
 * @param argv pole poli parametru
 * @return  a vraci nactene parametry ve strukture TParams
 */
TParams getParams(int argc, char *argv[])
{
	TParams result =
	{
	.filename1 = NULL,
	.filename2 = NULL,
	.ecode = EOK,
	.state = CSORT,
	};
/* Nacitani parametru */
	// napoveda
	if (argc == 2)
	{
		if (strcmp ("-h", argv[1]) == 0) result.state = CHELP;
		else result.ecode = EHELP;
	}
	else if (argc == 3)
	{
		result.state = CSORT;
		result.filename1 = argv[1];
		result.filename2 = argv[2];
	}
	else if (argc == 5 && strcmp ("--loc", argv[1]) == 0)
	{
		result.state = CSORT;
		result.loc = argv[2];
		result.filename1 = argv[3];
		result.filename2 = argv[4];
	}
	else
	{
		result.ecode = EHELP;
	}

	return result;
}

/**
 * Inicializuje linearni seznam
 * @param L seznam
 */
void InitList (tList *L)
{
	L->Act = NULL;
	L->First = NULL;
	L->Last = NULL;
	L->length = 0;
}

/**
 * Vlozi prvek do seznamu
 * @param L seznam, kam budeme vkladat
 * @param arr pole znaku, ktere vkladame do datove casti polozky
 * @return vraci chybovy kod nebo kod EOK
 */
int InsertFirst (tList *L, wchar_t *arr)
{
	tElemPtr new = NULL;

	if ((new = malloc(sizeof(struct tElem))) == NULL) return EALLOC;
	else
	{
		L->length++;
		new->data = arr;
		new->lptr = NULL;
		new->rptr = L->First;

		if (L->First == NULL) L->Last = new;
		else L->First->lptr = new;
		L->First = new;
	}

	return EOK;
}

/**
 * Smaze cely seznam a uvolni pamet
 * @param L seznam, ktery budeme mazat
 */
void DisposeList (tList *L)
{
	tElemPtr aux;
	while (L->First != NULL)
	{
		aux = L->First;
		L->First = L->First->rptr;
		free(aux->data);
		free(aux);
	}
	L->Last = NULL;
	L->Act = NULL;
}

/**
 * Nacte data ze vstupniho souboru a ulozi je do seznamu pomoci
 * fce InsertFirst
 * @param L seznam, kam budeme vkladat
 * @param filename soubor, ze ktereho budeme cist
 * @return vraci chybovy kod nebo kod EOK
 */
int loadFile (tList * L, char * filename)
{
	FILE *myfile;
	wint_t c, i = 0;
	// pomocna promenna pro alokaci a realokaci
	wint_t alloclen = STR_LEN;
	bool input = false;
	//do tohoto pole se bude alokovat retezec ze vstupu
	wchar_t *aux;
	//pomocna promenna pro zabraneni ztraty ukazatele pri realokaci
	wchar_t *auxx;

	myfile = fopen(filename, "r");
	if (myfile == NULL) return EFILE;
	if ((aux = malloc(sizeof(wchar_t) * alloclen)) == NULL) return EALLOC;
	aux[i] = L'\0';

	while ((c = fgetwc(myfile)) != WEOF || input)
	{
		if (c == ENTER || c == WEOF)
		{
			if (input != false)
			{
				if (InsertFirst(L, aux))
				{
					free(aux);
					return EINSERT;
				}
				i = 0;
				alloclen = STR_LEN;
				if ((aux = malloc(sizeof(wchar_t) * alloclen)) == NULL) return EALLOC;
				aux[i] = L'\0';
				input = false;
			}
		}
		else if (i + 1< alloclen)
		{
			aux[i] = c;
			aux[i + 1] = L'\0';
			i++;
			input = true;
		}
		else
		{// i + 1 == alloclen
			alloclen *= ALLOC;
			if ((auxx = realloc(aux, sizeof(wchar_t) * alloclen)) == NULL)
			{
				free(aux);
				return EALLOC;
			}
			aux = auxx;
			aux[i] = c;
			aux[i + 1] = L'\0';
			i++;
		}
	}

	free(aux);

	if (fclose(myfile) == EOF) return EFILE;
	return EOK;
}

/**
 * Ulozi serazena data do vystupniho souboru
 * @param L seznam, ktery ukladame/tiskneme
 * @param filename soubor, do ktereho budeme zapisovat
 * @return vraci chybovy kod nebo kod EOK
 */
int saveFile(tList * L, char * filename)
{
	FILE *myfile;
	tElemPtr aux;

	myfile = fopen(filename, "w");
	if (myfile == NULL) return EFILE;

	for (aux = L->Last; aux != NULL; aux=aux->lptr)
	{
		wint_t len = wcslen(aux->data);
		for (wint_t i = 0; i < len; i++)
		{
			if (fputwc(aux->data[i], myfile) == WEOF) return EOUTPUT;
		}
		fwprintf(myfile, L"\n");
	}

	if (fclose(myfile) == EOF) return EFILE;
	return EOK;
}

/**
 * Porovna po znacich dva retezce a rozhodne, zda je
 * nektery z nich vetsi, mensi nebo jsou si rovny
 * @param first prvni retezec
 * @param second druhy retezec
 * @return vraci LESS(prvni je mensi), GRTR(prvni je vetsi)
 * nebo EQ(jsou stejne)
 */
int stringCompare(wchar_t *first, wchar_t *second)
{
	wint_t value_f, value_s;
	wint_t slen_f, slen_s;
	wint_t i = 0, j = 0;

	slen_f = wcslen(first);
	slen_s = wcslen(second);

for (int tab = TAB1; tab <= TAB2; tab++)
{
	i=0, j=0;
	while (first[i] != L'\0' && second[j] != L'\0')
	{
		while((first[i] == L' ' || first[i] == L'\t') && (first[i+1] == L' ' || first[i+1] == L'\t')) i++;
		while((second[j] == L' ' || second[j] == L'\t') && (second[j+1] == L' ' || second[j+1] == L'\t')) j++;

		value_f = TAB[tab][(wchar_t) first[i]];
		value_s = TAB[tab][(wchar_t) second[j]];

		if ((first[i] == (L'c') || first[i] == (L'C')) && (first[i+1] == (L'h') || first[i+1] == (L'H')) && i < slen_f-1)
		{
			value_f = CH;
			i++;
		}
		if ((second[j] == (L'c') || second[j] == (L'C')) && (second[j+1] == (L'h') || second[j+1] == (L'H')) && j < slen_s-1)
		{
			value_s = CH;
			j++;
		}

		if(value_f < value_s) return LESS;
		else if (value_f > value_s) return GRTR;
		else
		{
			i++;
			j++;
		}
	}
}

	if (slen_f < slen_s) return LESS;
	else if (slen_f > slen_s) return GRTR;
	else
	{
		return EQ;
	}
}

/**
 * Seradi seznam metodou bubble sort
 * @param L seznam, ktery budeme radit
 */
void sortList(tList * L)
{
	int count = L->length;
	wchar_t *dataHelper;
	L->Act = L->First;

	if (L->First != L->Last || L->First != NULL)
	{
		for (int i=0; i < count; i++)
		{
			for (int j = 0; j < count - 1; j++)
			{
				if (stringCompare(L->Act->data, L->Act->rptr->data) == LESS)
				{
					// prohodi ukazatele na data
					dataHelper = L->Act->data;
					L->Act->data = L->Act->rptr->data;
					L->Act->rptr->data = dataHelper;
				}
				//pak dojde k posunu v seznamu
				L->Act = L->Act->rptr;
			}
		L->Act = L->First;
		}
	}
}

int locale (char* loc)
{
	if (loc == NULL)
	{
		setlocale(LC_ALL,"");
		if(setlocale(LC_ALL, loc) == NULL) return ELOC;
	}
	else
	{
		if(setlocale(LC_ALL, loc) == NULL) return ELOC;
	}
	return EOK;
}
/////////////////////////////////////////
//                MAIN                 //
/////////////////////////////////////////
int main(int argc, char *argv[])
{
	tList L;
	TParams params = getParams(argc, argv);

	if (params.ecode != EOK)
	{
		printECode(params.ecode);
		return EXIT_FAILURE;
	}

///Volani fce

	int ecode = EOK;
	if (params.state == CHELP) printf("%s", HELPMSG);
	else if(params.state == CSORT)
	{
		if((ecode = locale(params.loc)) == EOK)
		{
			InitList(&L);
			if((ecode = loadFile(&L, params.filename1)) == EOK)
			{
				sortList(&L);
				saveFile(&L, params.filename2);
				DisposeList(&L);
			}
		}
	}

	if (ecode != EOK)
	{
		printECode(ecode);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
