/* ************************ inst_list.h ************************************* */
/* Soubor:              inst_list.h - Instrukcni list                         */
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

#ifndef _INST_LIST_H_
#define _INST_LIST_H_
/* Prevencia viacnasobneho include */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include "error_modul.h"
#include "ial.h"
#include "scanner.h"
#include "precedence.h"

#define I_STOP 0  /* Koniec vykonavania */
#define I_BEGIN 1 /* Zaciatok vykonavania*/
#define I_READ 2
#define I_WRITE 3
#define I_POWER 4 /* Mocnina */
#define I_MULTIPLY 5 /*Nasobeni */
#define I_DIVIDE 6 /* Deleni */
#define I_ADD 7  /* Scitani */
#define I_SUB 8  /* Odcitani */
#define I_CONCATE 9 /* Konkatenace */
#define I_LESS 10
#define I_MORE 11
#define I_LESS_EQUAL 12
#define I_MORE_EQUAL 13
#define I_NOT_EQUAL 14
#define I_EQUAL 15
#define I_MODULO 16
#define I_LENGHT 17
#define I_UNARY_MINUS 18
#define I_IF 19
#define I_JUMP 20
#define I_ASSIGN 21
#define I_TYPE 22
#define I_SUBSTR 23
#define I_FIND 24
#define I_SORT 25
#define I_CALL_FUNC 26
#define I_RETURN 27
#define I_WHILE 28
#define I_DO 29
#define I_DO_END 31
#define I_REPEAT 32
#define I_UNTIL 33
#define I_DECL_FUNC 34
#define I_MAIN 35
#define I_ARG_ASSIGN 36
#define I_REC_ARG_ASSIGN 37
#define I_END_DECL_FUNC 38
#define I_THEN 39
#define I_ELSE 40
#define I_END_IF 41


#define FUNC_ARRAY_SIZE 20

/* Struktura pre trojadresny kod */
typedef struct
{
    int action;
    void *addr1;
    void *addr2;
    void *addr3;
} tInst;

 /* Struktura polozky zoznamu */
typedef struct polozkaList
{
    int line_number;
    tInst *data;                /* Data instrukcie */
    struct polozkaList *next;  /* Ukazatel na dalsiu instrukciu */
} tPolozkaList, *tPolozkaPointer;

typedef struct
{
string *identif;
tVarTreeItem *VarTree;
tPolozkaList *function;
} tFunc;

/* Zoznam instrukcii */
typedef struct
{
    tPolozkaList  *First;      /* Zaciatok zoznamu */
    tPolozkaList *Last;       /* Koniec zoznamu */
    tPolozkaList  *Active;     /* Aktivny prvok */
} tInstList;


extern tInstList InstList;
extern tFunc *funcArray;


/* Vytvorenie instrukciev 3-adresnom kode */
tInst InstCreate(int type, void *addr1, void *addr2, void *addr3);
int MakeInst(int type, void *addr1, void *addr2, void *addr3, int FuncCounter); /* Obalovacia funkcia na spravu vytvarania funkcii */


/* Zakladne funkcie pre pracu nad zoznamom */
void InitInstList (tInstList *);    /* Inicializuje zoznam */
void DisposeList (tInstList *);     /* Uvolni cely zoznam */
void InsertFirst (tInstList *, tInst*);      /* Vlozi novu polozku na zaciatok zoznamu */
void InsertLast (tInstList *, tInst*);       /* Vlozi novu polozku na koniec zonznamu */
void First (tInstList *);       /* Nastavy aktivitu na prvy prvok zoznamu */
void CopyFirst (tInstList *, tInst *);      /* Zkopiruje hodnotu prveho prvku zoznamu */
void DeleteFirst (tInstList *);     /* Zmaze prvu polozku zoznamu */
void PostDelete (tInstList *);      /* Zmaze polozku za aktivnym prvkom zoznamu */
void PostInsert (tInstList *, tInst*);       /* Vlozi polozku za aktivny prvok zoznamu */
void Succ (tInstList *);        /* Posunie aktivitu na nasledujuci prvok zoznamu */
void Copy (tInstList *, tInst *);       /* Vrati hodnotu aktivneho prvku zoznamu */
void Actualize (tInstList *, tInst *);       /* Aktualizuje hodnotu aktivneho prvku zoznamu */
void ListJump(tInstList *, tPolozkaList *);        /* Nastavy ako aktivny prvok,prvok so danou adresou */
int  Active (tInstList *);      /* Kontrola aktivnosti zoznamu */

#endif /*_INST_LIST_H_*/
