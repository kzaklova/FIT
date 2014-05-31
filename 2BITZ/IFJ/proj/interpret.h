 /* ************************ interpret.h ************************************* */
/* Soubor:              interpret.h - Funkce interpretu                       */
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

#ifndef _INCL_INTERPRET_MODUL
#define _INCL_INTERPRET_MODUL
/* Prevencia viacnasobneho include */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <stdarg.h>
#include "str.h"
#include "inst_list.h"
#include "error_modul.h"
#include "parser.h"
#include "ial.h"

#define INT_SUCCESS 0
#define INT_ERROR 1
#define INT_FAIL 2
#define INT_STOP 3 /* Specialne makro informujuce cyklicke instrukcie ze maju prestat vykonavat svoju cinnost */
#define INT_STOP_RETURN 4
#define INT_ENDIF 5

extern int errno;


/*Funkce vykonavajici read*/
int iRd(tVarValue *dest, tVarValue *src);

/*Funkce vykonavajici write*/
int iWr(tVarValue *dest);

/* Funkce implementujuca return */
int returnValue(tVarValue **destination,tVarValue *source);

int Interprete(tInstList *); /* Hlavna funkcia interpretu, riadi jeho cinnost */
/*Vstupom funkcie je naplneny instrukcny list */

int ExecInstr(tInst *); /* Funkcia interpretuje konkretnu instrukciu */
/* Vstupom funkcie je konkretna instrukcia z  instrukcneho listu */


#endif
