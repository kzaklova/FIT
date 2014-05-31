/* ******************************** ial.h *********************************** */
/* Soubor:              ial.h - Funkce pro predmet algotirmy                  */
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
/* ************************************************************************** */
#ifndef _IAL_H_
#define _IAL_H_


#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include "error_modul.h"
#include "scanner.h"
#include "str.h"

// struktura s polem pro preskladani a vyhledavani
typedef struct
{
	char *Pole;
	int delka;
}Tsort;


// funkce znovuustavujici haldu
void SiftDown( Tsort *Heap, int Left, int Right);

// samotna fce heapsort
void HeapS(Tsort *Heap);

// funkce rozseka retezec do pole a zavola HeapS - zmeni ukazatel retezce
int heapSort(string *attr);

// funkce inicializuje retezec a ulozi do struktury
int init(string *attr, Tsort *sort);

// funkce zpracuje vstupni retezec a podretezec a zavola vyhledavaci funkci
int kmp(string *text, string *pattern, double *succ);

// samotny vyhledavaci algoritmus
int kmpFind(Tsort *str, Tsort *substr, int* succ);


/** Makra */


#define T_NIL 0
#define T_INTEGER 1
#define T_REAL 2
#define T_NUMBER 1
#define T_STRING 3
#define T_BOOLEAN 4

#define TREE_SUCCESS 5
#define TREE_EXIST 6
#define TREE_FAIL 7

#define STRING_ERROR 8
#define TREE_ERROR 9

#define O_CHECK 10
#define O_ADD 11
#define O_IDENTIFY_ID 12
#define O_VAR_TYPE 13
#define O_COPY_TREE 14
#define O_ACTUALIZE 15
#define O_FETCH_VALUE 16
#define O_MERGE_ORIG 17

#define T_FID 18
#define T_VID 19

/** Struktury tabulky symbolu */
typedef struct varValue
{
int type;                        /* Typ ulozenej premennej pouziva sa vo funkciach */

int integer;
double real;
string *stringVar;
bool boolean;
}tVarValue, *p_VarValue;

typedef struct treeControl
{
int operation;
int paramCounter;
string *function;
string *variable;
string *identif;
tVarValue *varData;
tVarValue *STitem;
}tTreeCont;

typedef struct VarTreeItem
{                                /* Polozka stromu premennych + obsahuje odkaz na hodnotu */
    string *key;                 /* Identifikator pouzivany ako kluc */
    tVarValue *data;             /* Pointer na strukturu s data o premennej */
    struct VarTreeItem *LeftRoot;     /* Lavy nasledovnik */
    struct VarTreeItem *RightRoot;    /* Pravy nasledovnik */
} tVarTreeItem;


typedef struct FuncTreeItem
{
    string *key;                /* Identifikator pouzivany ako kluc */
    tVarTreeItem  *rootVarTree;  /* Ukazatel na podstrom premennych prisluchajucich funkcii */
    struct FuncTreeItem *LeftRoot;     /* Lavy nasledovnik */
    struct FuncTreeItem *RightRoot;    /* Pravy nasledovnik */
} tFuncTreeItem, *p_FuncTreeItem;

extern tFuncTreeItem *STable;
extern tFuncTreeItem *STbackUp;
extern int BackUpCounter;

/** Prototypy funkcii pre pracu nad stromom */
int tableHandler(tFuncTreeItem **mainRoot, tTreeCont *handle);

int CheckResWord(string *identif);
bool TreeInit (tFuncTreeItem **RootPtr);
bool insertBuiltIn(tFuncTreeItem **rootPtr);
bool toString(string ** dest, char * src);
void disposeString(string **str);

int VarTreeInsert(tVarTreeItem **RootPtr, string *identif, tVarValue *newVar);
int VarTreeSearch (tVarTreeItem *RootPtr, string *identif);
void DisposeVarTree(struct VarTreeItem **mainVarRoot);
int getVarTree(tVarTreeItem ***found, tFuncTreeItem *RootPtr, string *identif);
int GetVarPointer(tVarTreeItem **found, tVarTreeItem *RootPtr, string *identif);


int FuncTreeInsert(tFuncTreeItem **mainRoot,string *identif);
int FuncTreeSearch (tFuncTreeItem *RootPtr, string *identif);
void DisposeFuncTree(tFuncTreeItem **mainRoot);

void DisposeFirstBackUp(void);
int CopyTree (tVarTreeItem **Source, tVarTreeItem **BackUp);
int MergeOrigBackup(tVarTreeItem **BackUp, tVarTreeItem **Original);

#endif /*_IAL_H_*/
