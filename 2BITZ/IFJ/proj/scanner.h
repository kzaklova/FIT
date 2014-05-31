/* ************************ scanner.h ************************************** */
/* Soubor:              scanner.h - Lexikalni analyzator                      */
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
#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>
#include "str.h"
#include "error_modul.h"

#define LF 10
#define ERROR_ALOK 8
#define END_OF_FILE 1
#define OPERATOR 2
#define IDENTIFIER 3
#define LOGIC 4
#define DATA_TYPE 5
#define KEY_WORD 6
#define NIL 7
#define STRING 8
#define INTEGER 9
#define REAL 10
#define MAX_KEY 14
#define MAX_LOGIC 1


// cisla radku - cislovani zacina od jednicky!!!!!
extern int line;
// fce ulozi prvni dulezity znak po chybe
void chyba (int *cislo, FILE *source);
// chyba v retezci -hledame vyskyt uvozovek
void chybastring (int *cislo, FILE* source);
// fce vraci tokeny v promene attr a jejich typ
int getNextToken(string *attr, int *typ, FILE *source);

#endif /* _SCANNER_H_ */
