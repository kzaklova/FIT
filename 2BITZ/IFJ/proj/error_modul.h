/* ************************ error_modul.h *********************************** */
/* Soubor:              error_modul.h - Chybove hlasky                        */
/* Kodovani:            UTF-8                                                 */
/* Datum:               Listopad.2011                                         */
/* Predmet:             Formalni jazyky a prekladace (IFJ)                    */
/* Projekt:             Implementace interpretu jazyka IFJ11                  */
/* Varianta zadani:                                                           */
/* Titul,Autori, login:         Škvařilová Radka      xskvar06                */
/*                              Zaklová Kateřina      xzaklo00                */
/*                              Zapletal Libor        xzaple29                */
/*Zivouci reinkarnace Buddhy    Zaoral Tomáš          xzaora02                */
/*Pokemoni cisar                Uhliarik Michal       xuhlia01                */
/* ************************************************************************** */

#ifndef _ERROR_MODUL_H_
#define _ERROR_MODUL_H_
/* Prevencia viacnasobneho include */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NoErr 0     /* Vsetko OK */
#define LexErr 1    /* Lexikalna chyba */
#define SynErr 2    /* Syntakticka chyba */
#define SemErr 3    /* Semanticka chyba */
#define IntErr 4    /* Chyba interpretacie */
#define ProgErr 5   /* Chyba programu */
#define Unknown -1   /* Neznama chyba */

typedef struct chyba
{
int LineNumber;
int ErrorType;
}TError;

/* Globalne premenne chyboveho modulu */
extern int ErrorFlag;
extern int ErrorCounter;
extern int ReallocCounter;
TError *ErrorList;

/* Funkcie chyboveho modulu */
void AddError(int CisloRadku, int TypChyby);
void PrintErrItem(TError *p_temp);
void PrintError(void);
void FreeError(void);
int MakeErrList(void);

#endif /*_ERROR_MODUL_H_*/
