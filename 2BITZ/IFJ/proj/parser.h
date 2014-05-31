/* **************************** parser.h ************************************ */
/* Soubor:              parser.h - Syntakticka analyza                        */
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
#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "ial.h"
#include "error_modul.h"
#include "str.h"
#include "scanner.h"
#include "precedence.h"

#define INIT_TRUE 1
#define INIT_FALSE 0
#define ARG_ARR_ISIZE 10
#define FUNCS_ARGS_ISIZE 10

typedef struct ArgArr {
	string **array;
	string *identif;
	int size;
	int argCounter;
}*tArgArrPtr;

typedef struct FuncsArgs {
	tArgArrPtr *argumentArray;
	int size;
	int funcCounter;
}*tFuncArgPtr;

typedef struct SubstrIndexes {
	tVarValue *left;
	tVarValue *right;
}tSubstrIndexes;

extern tSubstrIndexes *substrIndexes;
extern tVarValue *exprResult;

bool input(string *attr, int *type, FILE *file);
bool paramList(string *attr, int *type, FILE *file);
bool param(string *attr, int *type, FILE *file, bool minusFlag);
bool assignment(string *attr, int *type, FILE *file);
bool expressionNext(string *attr, int *type, FILE *file);
bool expressionList(string *attr, int *type, FILE *file);
bool statement(string *attr, int *type, FILE *file);
bool statementList(string *attr, int *type, FILE *file);
bool factor(string *attr, int *type, FILE *file, int initFlag, bool minusFlag);
bool initialization(string *attr, int *type, FILE *file);
bool varDef(string *attr, int *type, FILE *file);//deklaruju vid
bool varDefList(string *attr, int *type, FILE *file);
bool argumentNext(string *attr, int *type, FILE *file);//pridavani argumentu jakozto vid
bool argumentList(string *attr, int *type, FILE *file);//pridavani argumentu jakozto vid
bool funcDef(string *attr, int *type, FILE *file);//menim fci
bool funcDefNext(string *attr, int *type, FILE *file);
bool funcDefList(string *attr, int *type, FILE *file);
bool controlInit(tTreeCont **control);
void controlDispose(tTreeCont **control);
bool parser(string *attr, int *type, FILE *file);
bool argumentArrayInit(tArgArrPtr *argumentArray);
void argumentArrayDispose(tArgArrPtr *argumentArray);
bool funcsArgsArrInit(tFuncArgPtr *funcsArgs);
void funcsArgsArrDispose(tFuncArgPtr *funcsArgs);
bool funcsArgsCreate(void);
bool argumentAddToArgArr(string *attr);
bool funcArgFetch();
bool validateParamNumber();
bool parserPrologue();
#endif
