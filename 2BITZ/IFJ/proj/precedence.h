/* ************************* precedence.h *********************************** */
/* Soubor:              precedence.h - Precedencni analyza                    */
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
#ifndef _PRECEDENCE_H_
#define _PRECEDENCE_H_


#define PT_MORE 0//reprezentuje znak '>' v precedencni tabulce
#define PT_LESS 1//reprezentuje znak '<' v precedencni tabulce
#define PT_EQUAL 2//reprezentuje znak '=' v precedencni tabulce
#define PT_NULL -1//reprezentuje zadny zaznam v precedencni tabulce
#define STACK_INIT_SIZE 20//inicializacni velikost zasobniku

#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "str.h"
#include "ial.h"
#include "scanner.h"
#include "error_modul.h"
#include "parser.h"
#include "inst_list.h"

enum entries	{//vnitrni reprezentace terminalu a neterminalu
	SA_EQUAL = 1,//==
	SA_NOTEQUAL,//~= (2)
	SA_LESSEQUAL,//<= (3)
	SA_MOREEQUAL,//>= (4)
	SA_LESS,//< (5)
	SA_MORE,//> (6)
	SA_CONCATE,//.. (7)
	SA_ADD,//+ (8)
	SA_SUB,//- (9)
	SA_MUL,//* (10)
	SA_DIV,/// (11)
	SA_MODULO,//% (12)
	SA_LENGTH,//# (13)
	SA_UNARYMINUS,//- (14)
	SA_POWER,//^ (15)
	SA_LBRACE,//( (16)
	SA_RBRACE,//) (17)
	SA_IDENTIFIER,//vid (18)
	SA_DATATYPE,//STRING || BOOL || INTEGER || REAL (19)
	SA_TERMINAL,//konecny operator (20)
	SA_EXPR,//neterminal <expression> (21)
	SA_PT_LESS//znak < vlozeny z precedencni tabulky (22)
};

typedef struct {
	int entry;
	tVarValue *expr;
}tData;

typedef struct {
	tData **arr;
	int top;
	int stackSize;
}tStack;

typedef struct PointerTrash {
	tVarValue **arr;
	int size;
	int top;
}*tPointTrashPtr;

extern tPointTrashPtr pointerTrashPtr;
extern tTreeCont *controlHelper, control;
extern tVarValue *intermResult;

bool precedenceSA(string *attr, int *type, FILE *file, bool write);
void precedenceTableInit(int (*precedenceTable)[21]);
int getTermType(char *value, int type);
bool stackInit (tStack* s);
bool stackEmpty (tStack* s);
bool stackFull (tStack* s);
void stackTop (tStack* s, tData *data);
void stackPop (tStack* s);
bool stackPush (tStack* s, tData *newData);
bool stackPushPtr(tStack *stack, tData *data);
void stackTopPtr(tStack *stack, tData *data);
void stackPopPtr(tStack *stack);
void stackDestroy(tStack *s);
bool compareIntArrays(int *array1, int *array2);
int generateRule(int *rule);
void fillExpr(tVarValue *expr, int type, int integer, double real,
				  string *stringVar, bool boolean);
int getTopTerm(tStack *s);
bool fillData(string *attr, int *type, FILE *file, tData *data);
bool transferDataToTerm(tStack *src, tStack *dest, tData *helper);
bool transferDataAll(tStack *src, tStack *dest, tData *helper);
int determRuleOnStack(tStack *mainStack, tStack *helperStack, tData *helperTerminal);
void initData(tData *data);
void freeData(tData *data, tVarValue *expr);
bool exprAlloc(tPointTrashPtr trash, p_VarValue *expr);
bool pointerTrashInit(tPointTrashPtr *trash);
bool pointerTrashAdd(tPointTrashPtr trash, tVarValue *item);
void pointerTrashDump(tPointTrashPtr *trash);
//Nasleduji testovaci fce
#endif /* _PRECEDENCE_H_ */
