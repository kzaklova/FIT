/* **************************** main.c ************************************** */
/* Soubor:              main.c - Ridici funkce interpretu                     */
/* Kodovani:            UTF-8                                                 */
/* Datum:               listopad.2011                                         */
/* Predmet:             Formalni jazyky a prekladace (IFJ)                    */
/* Projekt:             Implementace interpretu jazyka IFJ11                  */
/* Varianta zadani:                                                           */
/* Titul,Autori, login:         Škvařilová Radka      xskvar06                */
/*                              Zaklová Kateřina      xzaklo00                */
/*                              Zapletal Libor        xzaple29                */
/*Zivouci reinkarnace Buddhy    Zaoral Tomáš          xzaora02                */
/*Pokemoni cisar                Uhliarik Michal       xuhlia01                */
/* ****************************************************************************/

#include "main.h"

tFuncTreeItem *STable;
tFuncTreeItem *STbackUp = NULL;
int BackUpCounter = 0;
tInstList InstList;
tFunc *funcArray = NULL;
tPointTrashPtr pointerTrashPtr = NULL;
tVarValue *exprResult = NULL;
tSubstrIndexes *substrIndexes = NULL;

int openFile( int argc, char **argv, FILE **source) {
	if (argc == 2) {
		if ((*source = fopen(argv[1], "r")) == NULL)
			fprintf(stderr, "Nezdarilo se otevrit soubor!\n");
		else
			return EXIT_SUCCESS;
	}
	else
		fprintf(stderr, "Chybny pocet argumentu!\n");
	return EXIT_FAILURE;
}

int prepareToken(string **token) {
	if ((*token = malloc(sizeof(string))) != NULL) {
		if (!strInit(*token))
			return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

void disposeAll(string *token, FILE *source) {
	DisposeList(&InstList);
	/*** -- Majk*/
	if (funcArray != NULL) {
        int i = 0;
        while ((funcArray[i]).identif != NULL) {
            free((funcArray[i]).identif->str);
            free((funcArray[i]).identif);
            DisposeVarTree(&(funcArray[i].VarTree));
            i++;
        }
	}
	free(funcArray);
	/*****/
	strFree(token);
	free(token);
	fclose(source);
	controlDispose(&controlHelper);
	pointerTrashDump(&pointerTrashPtr);
	DisposeFuncTree(&STbackUp);
	DisposeFuncTree(&STable);
	PrintError();
	FreeError();
	free(substrIndexes);
}

int main(int argc, char *argv[]) {
	FILE *source;
	string *token;


	int type = 0;

    InitInstList (&InstList);

	if (MakeErrList())
		return ProgErr;//bude globalni prom posledni chyby
	TreeInit(&STable);
	if (openFile(argc, argv, &source)) {
		AddError(0, ProgErr);
		PrintError();//musi se osetrit prazdny seznam!
		FreeError();
		return ProgErr;
	}
	if (prepareToken(&token)) {
		AddError(0, ProgErr);
		fclose(source);
		PrintError();
		FreeError();
		return ProgErr;
	}

	if (parser(token, &type, source)) {
		Interprete(&InstList);
	}

	disposeAll( token, source);
	return ErrorFlag;
}
