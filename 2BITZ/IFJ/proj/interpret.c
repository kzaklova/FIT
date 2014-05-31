/* ************************ interpret.c ************************************* */
/* Soubor:              interpret.c - Funkce interpretu                       */
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

#include "interpret.h"


int returnValue(tVarValue **destination,tVarValue *source)
{
switch( source->type)
  {
  case T_INTEGER: (*destination)->type = T_INTEGER;
                (*destination)->real = source->real;
                break;

  case T_REAL: (*destination)->type = T_REAL;
             (*destination)->real = source->real;
             break;

  case T_BOOLEAN: (*destination)->type = T_BOOLEAN;
                (*destination)->boolean = source->boolean;
                break;

  case T_NIL: (*destination)->type = T_NIL;
             break;

  case T_STRING: if((*destination)->stringVar != NULL)
                    {
                    (*destination)->type = T_STRING;
                    if((strCopyString((*destination)->stringVar, source->stringVar)) ==  INT_ERROR)
                      {
                      AddError(InstList.Active->line_number , ProgErr);
                      return INT_ERROR;
                      }
                    }
                  else
                    {
                    (*destination)->type = T_STRING;
                    if(((*destination)->stringVar= malloc(sizeof(string))) == NULL)
                      {
                      AddError(InstList.Active->line_number, ProgErr);
                      return INT_ERROR;
                      }
                    if(strInit(((*destination)->stringVar)) == INT_ERROR)
                      {
                      AddError(InstList.Active->line_number , ProgErr);
                      return INT_ERROR;
                      }
                    if((strCopyString((*destination)->stringVar, source->stringVar)) == INT_ERROR)
                      {
                      AddError(InstList.Active->line_number, ProgErr);
                      return INT_ERROR;
                      }
                    }
                  break;
  }
return INT_SUCCESS;
}

int ExecInstr(tInst *Inst)
{ /* Funkcia interpretuje konkretnu instrukciu */

switch(Inst->action)
    {
    case I_STOP: /** Specialna dummy instrukcia oznamujuca cyklicky vykonavajucim instrukciam ze maju prestat vykonavat svoju cinnost
                     semantika: I_STOP   NULL, NULL, NULL    */
               return INT_STOP;
               break;

    case I_BEGIN: /* Zaciatok programu */
                break;
    case I_READ: /**Zavola pomocnou fuknci na nacteni ze stdin iRd, ta ocekava dve adresy, do prvni uklada a ve druhe ma argument
    								semantika: I_READ, destination, source, NULL*/
    						if (iRd(((tVarValue *) Inst->addr1), ((tVarValue *) Inst->addr2)))
    							return INT_ERROR;
               break;
    case  I_WRITE: /**Zavola pomocnou funkci na tisknuti vysledku vyrazu iWr() -- ta ocekava jeden parametr typu tVarValue *
    									semantika: I_WRITE, source, NULL, NULL*/
								if (iWr(((tVarValue *) Inst->addr1)))
									return INT_ERROR;
	              break;

/** Aritmeticke operacie
semantika: I_instrukce   destination, left_source, right_source */

    case  I_POWER:   /* Mocnina */
                 if(!((((tVarValue *) Inst->addr2)->type == T_INTEGER) || (((tVarValue *) Inst->addr2)->type == T_REAL)) || !((((tVarValue *) Inst->addr3)->type == T_INTEGER ) || (((tVarValue *) Inst->addr3)->type == T_REAL )))
                   { /*  Vieme umocnovat iba cele cisla alebo desatinne, string ee */
                   AddError(InstList.Active->line_number, IntErr);
                   return INT_FAIL;
                   }
                 if((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_INTEGER)) ((tVarValue *) Inst->addr1)->type = T_INTEGER;
                 else ((tVarValue *) Inst->addr1)->type = T_REAL;
                 ((tVarValue *) Inst->addr1)->real= pow(((tVarValue *) Inst->addr2)->real , ((tVarValue *) Inst->addr3)->real);

                 if(errno)
                   {
                   AddError(InstList.Active->line_number, IntErr);
                   return INT_FAIL;
                   }
                 return INT_SUCCESS;
                 break;

    case  I_MULTIPLY:   /* Nasobenie */
                    if(!((((tVarValue *) Inst->addr2)->type == T_INTEGER) || (((tVarValue *) Inst->addr2)->type == T_REAL)) || !((((tVarValue *) Inst->addr3)->type == T_INTEGER ) || (((tVarValue *) Inst->addr3)->type == T_REAL )))
                      { /*  Vieme nasobit iba cele cisla alebo desatinne, string ee */
                      AddError(InstList.Active->line_number, IntErr);
                      return INT_FAIL;
                      }
                    if((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_INTEGER)) ((tVarValue *) Inst->addr1)->type = T_INTEGER;
                    else ((tVarValue *) Inst->addr1)->type = T_REAL;



                    ((tVarValue *) Inst->addr1)->real=(((tVarValue *) Inst->addr2)->real * ((tVarValue *) Inst->addr3)->real);


                    if(errno)
                      {
                      AddError(InstList.Active->line_number, IntErr);
                      return INT_FAIL;
                      }
                    return INT_SUCCESS;

                    break;  /*Nasobeni */

    case  I_DIVIDE:
                   if(!((((tVarValue *) Inst->addr2)->type == T_INTEGER) || (((tVarValue *) Inst->addr2)->type == T_REAL)) || !((((tVarValue *) Inst->addr3)->type == T_INTEGER ) || (((tVarValue *) Inst->addr3)->type == T_REAL )))
                      { /*  Vieme delit iba cele cisla alebo desatinne, string ee */
                      AddError(InstList.Active->line_number, IntErr);
                      return INT_FAIL;
                      }
                    if((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_INTEGER)) ((tVarValue *) Inst->addr1)->type = T_INTEGER;
                    else ((tVarValue *) Inst->addr1)->type = T_REAL;

                    if(((tVarValue *) Inst->addr3)->real == 0)
                      {
                      AddError(InstList.Active->line_number, IntErr);
                      return INT_FAIL;
                      }

                    ((tVarValue *) Inst->addr1)->real=(((tVarValue *) Inst->addr2)->real / ((tVarValue *) Inst->addr3)->real);
                    if(errno)
                      {
                      AddError(InstList.Active->line_number, IntErr);
                      return INT_FAIL;
                      }
                    return INT_SUCCESS;
                    break;

    case  I_ADD:
                   if(!((((tVarValue *) Inst->addr2)->type == T_INTEGER) || (((tVarValue *) Inst->addr2)->type == T_REAL)) || !((((tVarValue *) Inst->addr3)->type == T_INTEGER ) || (((tVarValue *) Inst->addr3)->type == T_REAL )))
                      { /*  Vieme scitat iba cele cisla alebo desatinne, string ee */
                      AddError(InstList.Active->line_number, IntErr);
                      return INT_FAIL;
                      }
                    if((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_INTEGER)) ((tVarValue *) Inst->addr1)->type = T_INTEGER;
                    else ((tVarValue *) Inst->addr1)->type = T_REAL;

                    ((tVarValue *) Inst->addr1)->real=(((tVarValue *) Inst->addr2)->real + ((tVarValue *) Inst->addr3)->real);
                    if(errno)
                      {
                      AddError(InstList.Active->line_number, IntErr);
                      return INT_FAIL;
                      }
                    return INT_SUCCESS;
                    break;  /* Scitani */

    case  I_SUB:
                    if(!((((tVarValue *) Inst->addr2)->type == T_INTEGER) || (((tVarValue *) Inst->addr2)->type == T_REAL)) || !((((tVarValue *) Inst->addr3)->type == T_INTEGER ) || (((tVarValue *) Inst->addr3)->type == T_REAL )))
                      { /*  Vieme scitat iba cele cisla alebo desatinne, string ee */
                      AddError(InstList.Active->line_number, IntErr);
                      return INT_FAIL;
                      }
                    if((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_INTEGER)) ((tVarValue *) Inst->addr1)->type = T_INTEGER;
                    else ((tVarValue *) Inst->addr1)->type = T_REAL;

                    ((tVarValue *) Inst->addr1)->real=(((tVarValue *) Inst->addr2)->real - ((tVarValue *) Inst->addr3)->real);
                    if(errno)
                      {
                      AddError(InstList.Active->line_number, IntErr);
                      return INT_FAIL;
                      }
                    return INT_SUCCESS;
                    break;  /* Odcitani */

    case  I_MODULO: /* Funkce modulo */
                    if(!((((tVarValue *) Inst->addr2)->type == T_INTEGER) || (((tVarValue *) Inst->addr2)->type == T_REAL)) || !((((tVarValue *) Inst->addr3)->type == T_INTEGER ) || (((tVarValue *) Inst->addr3)->type == T_REAL )))
                      { /*  Vieme scitat iba cele cisla alebo desatinne, string ee */
                      AddError(InstList.Active->line_number, IntErr);
                      return INT_FAIL;
                      }
                    if((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_INTEGER)) ((tVarValue *) Inst->addr1)->type = T_INTEGER;
                    else ((tVarValue *) Inst->addr1)->type = T_REAL;

                    int result = (int)((((tVarValue *) Inst->addr2)->real) / (((tVarValue *) Inst->addr3)->real));

                    ((tVarValue *) Inst->addr1)->real=(((tVarValue *) Inst->addr2)->real) - ((double)(result) * (((tVarValue *) Inst->addr3)->real));
                    if(errno)
                      {
                      AddError(InstList.Active->line_number, IntErr);
                      return INT_FAIL;
                      }
                    return INT_SUCCESS;
                    break;  /* Odcitani */
                  break;

    case  I_CONCATE:
                   if((((tVarValue *) Inst->addr2)->type != T_STRING) || (((tVarValue *) Inst->addr3)->type != T_STRING))
                      { /*  konkatenace sa moze vykonavat iba na stringom */
                      AddError(InstList.Active->line_number, IntErr);
                      return INT_FAIL;
                      }
                    {
                    ((tVarValue *) Inst->addr1)->type= T_STRING;
                    if (strCopyString(((tVarValue *) Inst->addr1)->stringVar, ((tVarValue *) Inst->addr2)->stringVar) == INT_ERROR)
                       { /* Skopirujem string na adrese 2  do stringu na adrese 1*/
                       AddError(InstList.Active->line_number, ProgErr);
                       return INT_ERROR;
                       }

                    int i =0;


                    while(((tVarValue *) Inst->addr3)->stringVar->str[i] != '\0')
                      { /* Do konca retazca na adrese3  pridavam do retazce na adrese1 po znaku */
                      if((strAddChar(((tVarValue *) Inst->addr1)->stringVar, ((tVarValue *) Inst->addr3)->stringVar->str[i]) == INT_ERROR))
                         {
                         AddError(InstList.Active->line_number, ProgErr);
                         return INT_ERROR;
                         }
                      i++;
                      }
                    }
                   break; /* Konkatenace */


/** Relacne porovnavanie
semantika: I_instrukce   destination, left_source, right_source */

    case  I_LESS: /* Je mensi nez */

                   if(((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_INTEGER)) || ((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                       || ((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_INTEGER)) || ((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                     )
                      { /*  Porovnanie s moze vykonavat iba nad danymi typmy */
                      ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                      if(((tVarValue *) Inst->addr2)->type != T_STRING)
                        { /* Ak pracujem s cislami */
                        ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                        if((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_INTEGER))
                          {
                          ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real < ((tVarValue *) Inst->addr3)->real);
                          }

                        if((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                          {
                          ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real < ((tVarValue *) Inst->addr3)->real);
                          }

                        if((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_INTEGER))
                          {
                          ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real < ((tVarValue *) Inst->addr3)->real);
                          }

                        if((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                          {
                          ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real < ((tVarValue *) Inst->addr3)->real);
                          }
                        }
                      }
                       else if ((((tVarValue *) Inst->addr2)->type == T_STRING) && (((tVarValue *) Inst->addr3)->type == T_STRING))
                         { /* Dva stringy */
                         ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                        int porovnanie = 0;
                        porovnanie= strcmp(((tVarValue *) Inst->addr2)->stringVar->str, ((tVarValue *) Inst->addr3)->stringVar->str);
                        if(porovnanie < 0)
                          {
                          ((tVarValue *) Inst->addr1)->boolean = true;
                          }
                        else ((tVarValue *) Inst->addr1)->boolean = false;
                        }

                      else
                      { /* Nepripustna kombinacia typov */
                       AddError(InstList.Active->line_number, SemErr);
                       return INT_ERROR;
                      }
                   break;

    case  I_MORE:  /* Je vatsie nez */
                   if(((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_INTEGER)) || ((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                       || ((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_INTEGER)) || ((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                        )
                      { /*  Porovnanie s moze vykonavat iba nad danymi typmy */
                      ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                      if(((tVarValue *) Inst->addr2)->type != T_STRING)
                        { /* Ak pracujem s cislami */
                        ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                        if((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_INTEGER))
                          {
                          ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real > ((tVarValue *) Inst->addr3)->real);
                          }

                        if((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                          {
                          ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real > ((tVarValue *) Inst->addr3)->real);
                          }

                        if((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_INTEGER))
                          {
                          ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real > ((tVarValue *) Inst->addr3)->real);
                          }

                        if((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                          {
                          ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real > ((tVarValue *) Inst->addr3)->real);
                          }
                        }
                      }
                       else if ((((tVarValue *) Inst->addr2)->type == T_STRING) && (((tVarValue *) Inst->addr3)->type == T_STRING))
                         { /* Dva stringy */
                         ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                        int porovnanie = 0;
                        porovnanie= strcmp(((tVarValue *) Inst->addr2)->stringVar->str, ((tVarValue *) Inst->addr3)->stringVar->str);
                        if(porovnanie > 0)
                          {
                          ((tVarValue *) Inst->addr1)->boolean = true;
                          }
                        else ((tVarValue *) Inst->addr1)->boolean = false;
                        }

                      else
                      { /* Nepripustna kombinacia typov */
                       AddError(InstList.Active->line_number, SemErr);
                       return INT_ERROR;
                      }
                   break;

    case  I_LESS_EQUAL: /* Je mensie alebo rovne */
                    if(((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_INTEGER)) || ((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                       || ((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_INTEGER)) || ((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                        )
                       {
                      /*  Porovnanie s moze vykonavat iba nad danymi typmy */
                      ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                      if(((tVarValue *) Inst->addr2)->type != T_STRING)
                        { /* Ak pracujem s cislami */
                        ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                        if((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_INTEGER))
                          {
                          ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real <= ((tVarValue *) Inst->addr3)->real);
                          }

                        if((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                          {
                          ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real <= ((tVarValue *) Inst->addr3)->real);
                          }

                        if((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_INTEGER))
                          {
                          ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real <= ((tVarValue *) Inst->addr3)->real);
                          }

                        if((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                          {
                          ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real <= ((tVarValue *) Inst->addr3)->real);
                          }
                        }
                       }
                       else if ((((tVarValue *) Inst->addr2)->type == T_STRING) && (((tVarValue *) Inst->addr3)->type == T_STRING))
                         { /* Dva stringy */
                         ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;

                        int porovnanie = 0;
                        porovnanie= strcmp(((tVarValue *) Inst->addr2)->stringVar->str, ((tVarValue *) Inst->addr3)->stringVar->str);
                        if(porovnanie <= 0)
                          {
                          ((tVarValue *) Inst->addr1)->boolean = true;
                          }
                        else ((tVarValue *) Inst->addr1)->boolean = false;
                         }

                      else
                      { /* Nepripustna kombinacia typov */
                       AddError(InstList.Active->line_number, SemErr);
                       return INT_ERROR;
                      }
                      break;

    case  I_MORE_EQUAL: /* Je vacsie alebo rovne */
                       if(((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_INTEGER)) || ((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                         || ((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_INTEGER)) || ((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                         )
                       { /*  Porovnanie s moze vykonavat iba nad danymi typmy */
                       ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                       if(((tVarValue *) Inst->addr2)->type != T_STRING)
                         { /* Ak pracujem s cislami */
                         ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                         if((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_INTEGER))
                           {
                           ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real >= ((tVarValue *) Inst->addr3)->real);
                           }

                         if((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                           {
                           ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real >= ((tVarValue *) Inst->addr3)->real);
                           }

                         if((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_INTEGER))
                           {
                           ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real >= ((tVarValue *) Inst->addr3)->real);
                           }

                         if((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                           {
                           ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real >= ((tVarValue *) Inst->addr3)->real);
                           }
                         }
                       }
                       else if ((((tVarValue *) Inst->addr2)->type == T_STRING) && (((tVarValue *) Inst->addr3)->type == T_STRING))
                         { /* Dva stringy */
                         ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                         int porovnanie = 0;
                         porovnanie= strcmp(((tVarValue *) Inst->addr2)->stringVar->str, ((tVarValue *) Inst->addr3)->stringVar->str);
                         if(porovnanie >= 0)
                           {
                           ((tVarValue *) Inst->addr1)->boolean = true;
                           }
                         else ((tVarValue *) Inst->addr1)->boolean = false;
                         }
                       else
                       { /* Nepripustna kombinacia typov */
                       AddError(InstList.Active->line_number, SemErr);
                       return INT_ERROR;
                       }
                       break;

    case  I_NOT_EQUAL: /* Nerovno */

                      if(((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_INTEGER)) || ((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                         || ((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_INTEGER)) || ((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                        || ((((tVarValue *) Inst->addr2)->type == T_BOOLEAN) && (((tVarValue *) Inst->addr3)->type == T_BOOLEAN)))

                       { /*  Porovnanie s moze vykonavat iba nad danymi typmy */
                       ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                       if(((tVarValue *) Inst->addr2)->type != T_STRING)
                         { /* Ak pracujem s cislami */
                         ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                         if((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_INTEGER))
                           {
                           ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real != ((tVarValue *) Inst->addr3)->real);
                           }

                         if((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                           {
                           ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real != ((tVarValue *) Inst->addr3)->real);
                           }

                         if((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_INTEGER))
                           {
                           ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real != ((tVarValue *) Inst->addr3)->real);
                           }

                         if((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                           {
                           ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real != ((tVarValue *) Inst->addr3)->real);
                           }
                         }
                      if((((tVarValue *) Inst->addr2)->type == T_BOOLEAN) && (((tVarValue *) Inst->addr3)->type == T_BOOLEAN))
                        {
                        ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                        ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->boolean != ((tVarValue *) Inst->addr3)->boolean);
                        }
                       }
                       else if ((((tVarValue *) Inst->addr2)->type == T_STRING) && (((tVarValue *) Inst->addr3)->type == T_STRING))
                         { /* Dva stringy */
                         ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                         int porovnanie = 0;
                         porovnanie= strcmp(((tVarValue *) Inst->addr2)->stringVar->str, ((tVarValue *) Inst->addr3)->stringVar->str);
                         if(porovnanie != 0)
                           {
                           ((tVarValue *) Inst->addr1)->boolean = true;
                           }
                         else ((tVarValue *) Inst->addr1)->boolean = false;
                         }

                       else
                       { /* Nepripustna kombinacia typov */
                       ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                       ((tVarValue *) Inst->addr1)->boolean = true;
                       }
                     break;

    case  I_EQUAL:
                    if(((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_INTEGER)) || ((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                         || ((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_INTEGER)) || ((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                        || ((((tVarValue *) Inst->addr2)->type == T_BOOLEAN) && (((tVarValue *) Inst->addr3)->type == T_BOOLEAN)))

                       /*  Porovnanie s moze vykonavat iba nad danymi typmy */
                       {
                       ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                       if(((tVarValue *) Inst->addr2)->type != T_STRING)
                         { /* Ak pracujem s cislami */
                         ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                         if((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_INTEGER))
                           {
                           ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real == ((tVarValue *) Inst->addr3)->real);
                           }

                         if((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                           {
                           ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real == ((tVarValue *) Inst->addr3)->real);
                           }

                         if((((tVarValue *) Inst->addr2)->type == T_REAL) && (((tVarValue *) Inst->addr3)->type == T_INTEGER))
                           {
                           ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real == ((tVarValue *) Inst->addr3)->real);
                           }

                         if((((tVarValue *) Inst->addr2)->type == T_INTEGER) && (((tVarValue *) Inst->addr3)->type == T_REAL))
                           {
                           ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->real == ((tVarValue *) Inst->addr3)->real);
                           }
                         }
                      if((((tVarValue *) Inst->addr2)->type == T_BOOLEAN) && (((tVarValue *) Inst->addr3)->type == T_BOOLEAN))
                        {
                        ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                        ((tVarValue *) Inst->addr1)->boolean = (((tVarValue *) Inst->addr2)->boolean == ((tVarValue *) Inst->addr3)->boolean);
                        }
                       }
                      else if ((((tVarValue *) Inst->addr2)->type == T_STRING) && (((tVarValue *) Inst->addr3)->type == T_STRING))
                         {
                         ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                         int porovnanie = 0;
                         porovnanie= strcmp(((tVarValue *) Inst->addr2)->stringVar->str, ((tVarValue *) Inst->addr3)->stringVar->str);
                         if(porovnanie == 0)
                           {
                           ((tVarValue *) Inst->addr1)->boolean = true;
                           }
                         else ((tVarValue *) Inst->addr1)->boolean = false;
                         }
                       else
                       { /* Nepripustna kombinacia typov */
                       ((tVarValue *) Inst->addr1)->type = T_BOOLEAN;
                       ((tVarValue *) Inst->addr1)->boolean = false;
                       }
                 break;



    case  I_LENGHT: /* Dlzka retazca
                    semantika: I_instrukce   destination, source, NULL */
                    if(((tVarValue *) Inst->addr2)->type == T_STRING)
                      {
                      ((tVarValue *) Inst->addr1)->type = T_INTEGER;
                      ((tVarValue *) Inst->addr1)->real = (((tVarValue *) Inst->addr2)->stringVar->length);
                      }
                    else
                      {
                      AddError(InstList.Active->line_number, IntErr);
                      return INT_ERROR;
                      }
                    break;

    case  I_UNARY_MINUS:
                       break;

    case  I_IF:  /** Dummy instrukcia ktora signalizuje pociatok podminky if
                     semantika: I_IF NULL NULL NULL */
              while((InstList.Active->next->data->action != I_THEN) && (ErrorFlag == NoErr))
                {/* Vyhodnocujem vyraz kym sa nedostanem na jeho koniec */
                Succ(&InstList);        /* Posunie aktivitu na nasledujuci prvok zoznamu */
                ExecInstr(((InstList.Active)->data));
                }
              if(ErrorFlag != NoErr) return INT_ERROR;
              /*Mam vyhodnoteny vyraz teraz sa rozhodujem podla jeho vysledku */

                Succ(&InstList);        /* Posunie aktivitu na nasledujuci prvok zoznamu */

              if(ExecInstr(((InstList.Active)->data)))
                { /* Podmienka bola pravdiva */
                while((InstList.Active->next->data->action != I_ELSE)&& (ErrorFlag == NoErr))
                   {/** Vykonavam prikazi dokial sa nedostanem po else */
                   Succ(&InstList);        /* Posunie aktivitu na nasledujuci prvok zoznamu */
                   ExecInstr(((InstList.Active)->data));
                   }
                if(ErrorFlag != NoErr) return INT_ERROR;
                /* Dostal som sa na else */
                while((InstList.Active->next->data->action != I_END_IF)&& (ErrorFlag == NoErr))
                  { /* Preskakujem polozky dokial sa nedostanem na I_IF_STOP */
                   Succ(&InstList);        /* Posunie aktivitu na nasledujuci prvok zoznamu */
                  }
                  if(ErrorFlag != NoErr) return INT_ERROR;
                  Succ(&InstList);        /* Posunie aktivitu na nasledujuci prvok zoznamu */
                  ExecInstr(((InstList.Active)->data)); /*Vykonam end */
                }
              else
                { /* Podmienka bola nepravdiva */
                while((InstList.Active->next->data->action != I_ELSE)&& (ErrorFlag == NoErr))
                   {/** Preskakujem prikazi dokial sa nedostanem po else */
                   Succ(&InstList);        /* Posunie aktivitu na nasledujuci prvok zoznamu */
                   }
                if(ErrorFlag != NoErr) return INT_ERROR;
                /* Dostal som sa na else */
                while((InstList.Active->next->data->action != I_END_IF)&& (ErrorFlag == NoErr))
                  { /* Preskakujem polozky dokial sa nedostanem na I_IF_STOP */
                   Succ(&InstList);        /* Posunie aktivitu na nasledujuci prvok zoznamu */
                   ExecInstr(((InstList.Active)->data));
                  }
                  if(ErrorFlag != NoErr) return INT_ERROR;
                  Succ(&InstList);        /* Posunie aktivitu na nasledujuci prvok zoznamu */
                  ExecInstr(((InstList.Active)->data)); /*Vykonam end */
                }
              break;

    case I_END_IF:
                break;
    case  I_JUMP:
                break;

    case  I_ASSIGN: /* Instrukcia priradenia
                       semantika: I_instrukce   destination, source, NULL */

                    if(((tVarValue *) Inst->addr2)->type == T_STRING)
                      { /* Prirazujem string */
                      if(((tVarValue *) Inst->addr1)->stringVar != NULL)
                        { /* CIlova promenna ma alokovane miestopre string */
                        if((strCopyString(((tVarValue *) Inst->addr1)->stringVar, ((tVarValue *) Inst->addr2)->stringVar)) == INT_ERROR)
                          {
                          AddError(InstList.Active->line_number, ProgErr);
                          return INT_ERROR;
                          }
                        }
                      else
                        {
                        if((((tVarValue *) Inst->addr1)->stringVar= malloc(sizeof(string))) == NULL )
                          {
                          AddError(InstList.Active->line_number , ProgErr);
                          return INT_ERROR;
                          }
                        if(strInit((((tVarValue *) Inst->addr1)->stringVar)) == 1)
                          {
                          AddError(InstList.Active->line_number , ProgErr);
                          return INT_ERROR;
                          }
                        }
                      if((strCopyString(((tVarValue *) Inst->addr1)->stringVar, ((tVarValue *) Inst->addr2)->stringVar)) == INT_ERROR)
                        {
                        AddError(InstList.Active->line_number, ProgErr);
                        return INT_ERROR;
                        }

                      ((tVarValue *) Inst->addr1)->type = ((tVarValue *) Inst->addr2)->type;
                      }
                     else
                      {
                      ((tVarValue *) Inst->addr1)->type = ((tVarValue *) Inst->addr2)->type;
                      if(((tVarValue *) Inst->addr2)->type == T_INTEGER) ((tVarValue *) Inst->addr1)->real = ((tVarValue *) Inst->addr2)->real;
                      else if(((tVarValue *) Inst->addr2)->type == T_REAL) ((tVarValue *) Inst->addr1)->real = ((tVarValue *) Inst->addr2)->real;
                      else ((tVarValue *) Inst->addr1)->boolean = ((tVarValue *) Inst->addr2)->boolean;
                      }
                  break;

   case  I_TYPE: /* Instrukcia vratenia typu
                     semantika: I_instrukce   destination, source, NULL */
                ((tVarValue *) Inst->addr1)->type = T_STRING;
				switch (((tVarValue *) Inst->addr2)->type)
				   {
                   case T_INTEGER:
				   case T_REAL:
                   strcpy(((tVarValue *) Inst->addr1)->stringVar->str, "number");
				   ((tVarValue *) Inst->addr1)->stringVar->length = 6;//pocet znaku ve slove number
				   break;

                   case T_STRING:
                   strcpy(((tVarValue *) Inst->addr1)->stringVar->str, "string");
				   ((tVarValue *) Inst->addr1)->stringVar->length = 6;//pocet znaku ve slove string
                   break;

                   case T_BOOLEAN:
                   strcpy(((tVarValue *) Inst->addr1)->stringVar->str, "boolean");
                   ((tVarValue *) Inst->addr1)->stringVar->length = 7;//pocet znaku ve slove boolean
                   break;

                   case T_NIL:
                   strcpy(((tVarValue *) Inst->addr1)->stringVar->str, "nil");
                   ((tVarValue *) Inst->addr1)->stringVar->length = 3;//pocet znaku ve slove boolean
                   break;
				   }
                break;

    case  I_SUBSTR: /** Instrukce ulozi do dest podretezec v zadanem retezci
                        semantika: instrukce: I_SUBSTR dest string indexes*/
                     if (((tVarValue*)Inst->addr2)->type==T_STRING && (((tSubstrIndexes*)Inst->addr3)->left->type==T_REAL ||
                         ((tSubstrIndexes*)Inst->addr3)->left->type==T_INTEGER) && (((tSubstrIndexes*)Inst->addr3)->right->type==T_REAL ||
                         ((tSubstrIndexes*)Inst->addr3)->right->type==T_INTEGER))
                    {
                     int start,finish;
                     start = ((tSubstrIndexes*)Inst->addr3)->left->real;
                     finish = ((tSubstrIndexes*)Inst->addr3)->right->real;
                     if (start<0)  /*prevedeme na indexy od zacatku retezce*/
                      start=((tVarValue*)Inst->addr2)->stringVar->length + start +1;
                     if (finish<0)/*prevedeme na indexy od zacatku retezce*/
                      finish=((tVarValue*)Inst->addr2)->stringVar->length + finish +1;
                     /* kontrolujeme specialni pripady indexu*/
                     if (start>finish || finish<=0 || start>((tVarValue*)Inst->addr2)->stringVar->length )
                     {
                      ((tVarValue*)Inst->addr1)->type=T_STRING;
                      strClear(((tVarValue*)Inst->addr1)->stringVar);
                     }
                     else
                      {
                        if (start<=0) /* osetrime krajni hodnoty*/
                          start=1;
                        if (finish>((tVarValue*)Inst->addr2)->stringVar->length ) /* osetrime krajni hodnoty*/
                          finish=((tVarValue*)Inst->addr2)->stringVar->length ;
                        ((tVarValue*)Inst->addr1)->type=T_STRING;
                        strClear(((tVarValue*)Inst->addr1)->stringVar); /* vyprazdnime retezec*/
                        while(start<=finish)
                        {
                          if(strAddChar(((tVarValue*)Inst->addr1)->stringVar,((tVarValue*)Inst->addr2)->stringVar->str[start-1]) )
                          {
                            AddError(InstList.Active->line_number, ProgErr);
                            return INT_ERROR;
                          }
                          start++;
                        }
                      }
                    }
                    else
                    {
                     ((tVarValue*)Inst->addr1)->type=T_NIL;
                    }

                  break;

    case  I_FIND:/**Instrukce implementuje vestavenou fci find -- najde prvni vyskyt retezce
    								na adrese2 v retezci na adrese1
    								semantika: I_TYPE, destination, string, substring*/
    						if ((((tVarValue*)Inst->addr2)->type==T_STRING) && (((tVarValue*)Inst->addr3)->type==T_STRING))
                  {
                    if (((tVarValue*)Inst->addr3)->stringVar->str[0]=='\0') /** prazdny druhy retezec-specialne nula*/
                    {
                    ((tVarValue*)Inst->addr1)->type=T_INTEGER;
                     ((tVarValue*)Inst->addr1)->real=0;
                    }
                    else
                    {
                     ((tVarValue*)Inst->addr1)->type=T_INTEGER;/** zavolani samotne fce na hledani*/
                      if (kmp(((tVarValue*)Inst->addr2)->stringVar,((tVarValue*)Inst->addr3)->stringVar,&((tVarValue*)Inst->addr1)->real)!=0)
                      {
                      AddError(InstList.Active->line_number,ProgErr);
                      return INT_ERROR;
                      }
                      if ((((tVarValue*)Inst->addr1)->real)<0) /* jestlize se jedna o zaporne cislo, musime spravit typ*/
                      {
                      ((tVarValue*)Inst->addr1)->type=T_BOOLEAN;
                      ((tVarValue*)Inst->addr1)->boolean=false;
                      }
                    }

                  }
                  else
                  {
                   ((tVarValue*)Inst->addr1)->type = T_NIL;
                  }
                break;

    case  I_SORT: /** Instrukce seradi znaky retezce na adrese2 a priradi vysledek do adresy1
                      semantika: instrukce: I_SORT destination source NULL */
                 if ((((tVarValue*)Inst->addr2)->stringVar !=NULL) && (((tVarValue*)Inst->addr2)->type==T_STRING)) /** Posleme neprazdny retezec*/
                 {
                 		if (strCopyString(((tVarValue*)Inst->addr1)->stringVar, ((tVarValue*)Inst->addr2)->stringVar)) {
                 			AddError(InstList.Active->line_number, ProgErr);
                 			return INT_ERROR;
                 		}
                 		((tVarValue*)Inst->addr1)->type = T_STRING;
                    if (heapSort(((tVarValue*)Inst->addr1)->stringVar)==ProgErr)/**Radici algoritmus*/
                      {
                      AddError(InstList.Active->line_number,ProgErr);
                      return INT_ERROR;
                      }
                 }
                 else
                 {
                  ((tVarValue*)Inst->addr1)->type = T_NIL;
                  AddError(InstList.Active->line_number, SemErr);
                  return INT_ERROR;
                 }
                break;

    case I_CALL_FUNC: /** Instrukcia volania uzivatelom definovanych funkcii
                          semantika: I_instrukce   return destination, FID volanej, FID volajucej */
                    {
                    tVarValue *returnDestination = NULL;
                    returnDestination = ((tVarValue *) Inst->addr1); /* Do lokalnej premennej ukladam cielovu destinaciu pre instrukciu return */

                    if((strCopyString(controlHelper->identif,(string *)Inst->addr3)) == INT_ERROR)
                      { /* Do identif si skopirujem meno premennej ktoru chcem zalohovat */
                      AddError(InstList.Active->line_number, ProgErr);
                      return INT_ERROR;
                      }

                    controlHelper->operation = O_COPY_TREE;
                    tableHandler(&STable, controlHelper); /* Vytvaram si zalohu volajucej  fce */

                    if((strCopyString(controlHelper->identif,(string *)Inst->addr2)) == INT_ERROR)
                      { /* Do identif si skopirujem meno premennej ktoru chcem zalohovat */
                      AddError(InstList.Active->line_number, ProgErr);
                      return INT_ERROR;
                      }
                    tableHandler(&STable, controlHelper); /* Vytvaram si zalohu volanej fce */



                    int retValue = 0;
                    int i=0;
                    tPolozkaList *returnInst = NULL; /* Pomocna premenna do ktorej si ulozim aktualnu poziciu v instr liste pre neskorsi navrat */
                    tVarTreeItem **p_var_help = NULL;
                    tVarValue *p_var_temp = NULL;

                    while(funcArray[i].identif != NULL)
                      {
                      if((strcmp(funcArray[i].identif->str,((string *)Inst->addr2)->str)) == 0)
                        {/* Ak som vo func Array nasiel danu funkciu */

                        if((getVarTree(&(p_var_help), &(*STable), funcArray[i].identif )) == TREE_ERROR)
                          {/* Ziskavam pointer na strom hodnot funkcie s id funcArray[i].identif */
                          AddError(0, ProgErr);
                          return INT_ERROR;
                          }

                         /* Prepisujem hodnoty vo VarTree na deklarovane */
                         MergeOrigBackup(&(funcArray[i].VarTree),p_var_help);
                        break;
                        }
                      i++;
                      }

                    while(true)
                         { /* Kontrolujem List dalej */
                         if(InstList.Active->next != NULL)
                           {
                           if((InstList.Active->next->data->action == I_ARG_ASSIGN) || (InstList.Active->next->data->action == I_REC_ARG_ASSIGN))
                             { /* Ak je potreba zmenit implicitne hodnoty argumentov */
                             Succ(&InstList);        /* Posunie aktivitu na nasledujuci prvok zoznamu */
                             retValue = ExecInstr(((InstList.Active)->data));
                             }
                           else break;
                           }
                         else break;
                         }

                    returnInst = InstList.Active; /* Ukladam si aktualnu poziciu */


                    i=0;
                    while(funcArray[i].identif != NULL)
                      {
                      if((strcmp(funcArray[i].identif->str,((string *)Inst->addr2)->str)) == 0)
                        {/* Ak som vo func Array nasiel danu funkciu */
                        ListJump(&InstList ,funcArray[i].function);
                        break;
                        }
                      i++;
                      }

                    while(((InstList.Active)->next->data->action != I_END_DECL_FUNC) && (ErrorFlag == NoErr) && ((InstList.Active)->next->data->action != I_RETURN ))
                         { /** Vykonavam telo funkcie dokial nenarazim na I_END_DECL_FUNC alebo I_RETURN alebo nedojde k chybe */
                         Succ(&InstList);        /* Posunie aktivitu na nasledujuci prvok zoznamu */
                         retValue = ExecInstr(((InstList.Active)->data));
                         }


                    if((InstList.Active)->next->data->action == I_RETURN )
                      { /* Ak je dalsia instrukcia return */
                      if(!exprAlloc(pointerTrashPtr, &p_var_temp))
                        {
                      	AddError(InstList.Active->line_number, ProgErr);
                      	return INT_ERROR;
                        }

                      Succ(&InstList);        /* Aktualna instrukcia je return  */
                      retValue= ExecInstr(((InstList.Active)->data));

                      if((returnValue(&p_var_temp,((tVarValue *) InstList.Active->data->addr1))) == INT_ERROR)  /* Do pomocnej premennej si skopirujem vysledok */
                        {
                        return INT_ERROR;
                        }
                      }

                    else if(ErrorFlag != NoErr)
                      {
                      return INT_ERROR;
                      }

                    if(returnInst == NULL)
                      {
                      AddError(InstList.Active->line_number, ProgErr);
                      return INT_ERROR;
                      }


                    /** Davam volanu funkciu do stavu pred volanim */
                    if((getVarTree(&(p_var_help), &(*STable),((string *) Inst->addr2))) == TREE_ERROR)
                      {/* Ziskavam pointer na strom hodnot funkce volajici*/
                      AddError(0, ProgErr);
                      return INT_ERROR;
                      }
                    MergeOrigBackup(&(STbackUp->rootVarTree),p_var_help); /*** TADY */
                    DisposeFirstBackUp();


                    /** Davam volajucu funkciu do stavu pred volanim */
                    if((getVarTree(&(p_var_help), &(*STable),((string *) Inst->addr3))) == TREE_ERROR)
                      {/* Ziskavam pointer na strom hodnot funkce volajici*/
                      AddError(0, ProgErr);
                      return INT_ERROR;
                      }


                    MergeOrigBackup(&(STbackUp->rootVarTree),p_var_help); /*** TADY */
                    DisposeFirstBackUp();

                    if(p_var_temp != NULL)
                      {/* Funkce se ukoncuje pomoci return */
                      if((returnValue(&returnDestination , p_var_temp)) == INT_ERROR) /* Ukladam hodnotu do navratovej adresy */
                        {
                        return INT_ERROR;
                        }
                      }

                    ListJump(&InstList, returnInst); /* Navrat do bodu volania */
                    }

                    break;
    case I_RETURN: /** Instrukcia navratu z volanej funkcie
                       semantika: I_RETURN temp_variable NULL NULL */
                 return INT_STOP_RETURN;
                 break;

    case I_WHILE: /** Dummy instrukcia ktora signalizuje zaciatok cyklu while
                     semantika: I_REPEAT NULL NULL NULL */

                {
                tPolozkaList *whileLabel = NULL; /* Pomocna premenna do ktorej si ulozim aktualnu poziciu v instr liste pre neskorsi navrat */
                whileLabel = InstList.Active; /* Ukladam si adresu repeat*/
                bool retValue = false;
                while(((InstList.Active)->next->data->action != I_DO) && (ErrorFlag == NoErr))
                  { /** Vyhodnocujem podmienku */
                  Succ(&InstList);        /* Posunie aktivitu na nasledujuci prvok zoznamu */
                  retValue = ExecInstr(((InstList.Active)->data));
                  }

                if(ErrorFlag != NoErr) return INT_ERROR;
                else
                  { /** Narazil som na I_DO */
                  Succ(&InstList);        /* Posunie aktivitu na nasledujuci prvok zoznamu */
                  retValue = ExecInstr(((InstList.Active)->data));
                  if(retValue)
                    {/* Podmienka bola pravdiva */
                    while(((InstList.Active)->next->data->action != I_DO_END) && (ErrorFlag == NoErr))
                       { /** Vykonavam prikazi dokial nenarazim na end*/
                       Succ(&InstList);        /* Posunie aktivitu na nasledujuci prvok zoznamu */
                       retValue = ExecInstr(((InstList.Active)->data));
                       }
                       if(ErrorFlag != NoErr) return INT_ERROR;
                       else
                         { /* Som na end */
                         ListJump(&InstList, whileLabel); /* Navrat na zaciatok while pre opetovne vyhodnotenie podmienky*/
                         ExecInstr(((InstList.Active)->data));
                         }
                    }
                  else
                    {
                    while(((InstList.Active)->next->data->action != I_DO_END) && (ErrorFlag == NoErr))
                       { /** Preskakujem prikazi dokial nenarazim na end*/
                       Succ(&InstList);        /* Posunie aktivitu na nasledujuci prvok zoznamu */
                       }
                    if(ErrorFlag != NoErr) return INT_ERROR;
                    else Succ(&InstList);  break;
                    }
                  }
                }
                break;

    case I_DO: /** Dummy instrukcia poukazujuca na koniec vyhodnocovania podmienky vracia volajucej funkcii vysledok vyrazu
                     semantika: I_DO expr_result NULL NULL */
                if(((((tVarValue *) Inst->addr1)->type == T_BOOLEAN) && (((tVarValue *) Inst->addr1)->boolean == false)) || (((tVarValue *) Inst->addr1)->type == T_NIL))
                  {/* Ak je podmienka nepravdiva */
                  return false;
                  }
                 else return true;
                 break;
    case I_DO_END:
                 break;
    case I_REPEAT: /** Dummy instrukcia ktora signalizuje zaciatok cyklu repeat
                     semantika: I_REPEAT NULL NULL NULL */
                {
                tPolozkaList *repeatLabel = NULL; /* Pomocna premenna do ktorej si ulozim aktualnu poziciu v instr liste pre neskorsi navrat */
                repeatLabel = InstList.Active; /* Ukladam si adresu repeat*/
                bool retValue = false;
                while(((InstList.Active)->next->data->action != I_UNTIL) && (ErrorFlag == NoErr))
                  { /** Vykonavam prikazi dokial nenarazim na until */
                  Succ(&InstList);        /* Posunie aktivitu na nasledujuci prvok zoznamu */
                  retValue = ExecInstr(((InstList.Active)->data));
                  }

                if(ErrorFlag != NoErr) return INT_ERROR;
                else
                  { /** Narazil som na I_UNTIL */
                  Succ(&InstList);        /* Posunie aktivitu na nasledujuci prvok zoznamu */
                  retValue = ExecInstr(((InstList.Active)->data));
                  if(retValue)
                    {/* Podmienka bola pravdiva */
                    ListJump(&InstList, repeatLabel); /* Navrat do bodu volania */
                    ExecInstr(((InstList.Active)->data));
                    }
                  else break;
                  }
                }
                break;
    case I_DECL_FUNC:/**Dummy instrukcia, navesti deklarovane fce
                        semantika: I_DECL_FUNC, FID, NULL, NULL*/
    						break;
    case I_END_DECL_FUNC:
    						return INT_STOP;
    						break;

    case I_ARG_ASSIGN: /** Specialna instrukcia assign pre zmenenie implicitne deklarovanych argumentu funcii pri volani funkcie
                            semantika: I_ARG_ASSIGN dest, source , NULL */
                    {
                    tVarValue *p_var_temp=((tVarValue *) Inst->addr1);
                    if(returnValue( &(p_var_temp) , ((tVarValue *) Inst->addr2))) /* Aktualizujem argument dle parametru */
                      {
                      AddError(InstList.Active->line_number, ProgErr);
                      return INT_ERROR;
                      }
                    break;
                    }

    case I_REC_ARG_ASSIGN: /** Specialna instrukcia assign pre zmenenie implicitne deklarovanych argumento funcii pri rekurzivnom volani funkcie
                            semantika: I_ARG_ASSIGN dest, source VID , NULL */
                    {
                    /** Pri rekurzivnom volani by som si premazal source tym padom by som mal nekorektne hodnoty , aby sa to nestalo vyberam si hodnoty z backup */
                    tVarTreeItem *p_variable = NULL;
                    if(GetVarPointer(&(p_variable), STbackUp->rootVarTree, ((string *) Inst->addr2) ) == TREE_ERROR)
                      {/* Ziskavam source */
                      AddError(0, ProgErr);
                      return INT_ERROR;
                      }


                    tVarValue *p_var_temp=((tVarValue *) Inst->addr1);
                    if(returnValue(&(p_var_temp) , p_variable->data )) /* Aktualizujem argument dle parametru */
                      {
                      AddError(InstList.Active->line_number, ProgErr);
                      return INT_ERROR;
                      }
                    break;
                    }

    case I_THEN: /** Dummy instrukcia poukazujuca na koniec vyhodnocovania podmienky vracia volajucej funkcii vysledok vyrazu
                     semantika: I_COND expr_result NULL NULL */
                if(((((tVarValue *) Inst->addr1)->type == T_BOOLEAN) && (((tVarValue *) Inst->addr1)->boolean == false)) || (((tVarValue *) Inst->addr1)->type == T_NIL))
                  {/* Ak je podmienka nepravdiva */
                  return false;
                  }
                 else return true;
                 break;

    case I_UNTIL: /** Dummy instrukcia poukazujuca na koniec vyhodnocovania podmienky vracia volajucej funkcii vysledok vyrazu
                     semantika: I_UNTIL expr_result NULL NULL */
                if(((((tVarValue *) Inst->addr1)->type == T_BOOLEAN) && (((tVarValue *) Inst->addr1)->boolean == false)) || (((tVarValue *) Inst->addr1)->type == T_NIL))
                  {/* Ak je podmienka nepravdiva */
                  return false;
                  }
                 else return true;
                 break;

    default: break;

    }
return INT_SUCCESS;
}

int iWr(tVarValue *src)
{
 switch(src->type)
   {
   case T_NIL:
             AddError(InstList.Active->line_number, SemErr);
             return INT_ERROR;
  case T_INTEGER:
             printf("%g", src->real);
             break;
  case T_REAL:
             printf("%g", src->real);
             break;
  case T_STRING:
             printf("%s", src->stringVar->str);
             break;
  case T_BOOLEAN:
             AddError(InstList.Active->line_number, SemErr);
             return INT_ERROR;
  default:
             AddError(InstList.Active->line_number, SemErr);
             return 1;
 }
 return 0;
}

//Funkce ktera vykonava volani read -- ocekava: destination, source
int iRd(tVarValue *dest, tVarValue *src)
{
  char znak;

  switch(src->type)
  {
    case T_NIL:
      break;
    case T_INTEGER:
    case T_REAL:
      dest->type = T_STRING;
      strClear(dest->stringVar);

      int i = (int) src->real;
      while(i > 0)
      {
        znak = getchar();
	if(znak == EOF || znak == '\n')
        {
          AddError(InstList.Active->line_number, SemErr);
          return 1;
        }
	if(strAddChar(dest->stringVar, znak))
        {
          AddError(InstList.Active->line_number, ProgErr);
          return 1;
        }
        i--;
      }
   break;
  case T_STRING:
    if(src->stringVar->str[0] == '*')
    {
      switch(src->stringVar->str[1])
      {
        case 'n':
          dest->type = T_REAL;
          if(scanf("%lf", &(dest->real)) != 1)
          {
            AddError(InstList.Active->line_number, SemErr);
            return 1;
          }
          break;
        case 'l':
          dest->type = T_STRING;
          strClear(dest->stringVar);
          while((znak = getchar()) != '\n')
          {
            if(strAddChar(dest->stringVar, znak))
            {
              AddError(InstList.Active->line_number, ProgErr);
              return 1;
            }
          }
          break;
        case 'a':
          dest->type = T_STRING;
          strClear(dest->stringVar);

          while((znak = getchar()) != EOF)
          {
            if(strAddChar(dest->stringVar, znak))
            {
              AddError(InstList.Active->line_number, ProgErr);
              return 1;
            }
          }
          break;
        default:
          AddError(InstList.Active->line_number, SemErr);
          return 1;
          break;
       }
   }
   else
   {
     AddError(InstList.Active->line_number, SemErr);
     return 1;
   }
   break;
  case T_BOOLEAN:
   break;
  default:
   break;
 }
 return 0;
}


int Interprete(tInstList *InstList)
{/* Hlavna funkcia interpretu, riadi jeho cinnost */

int returnValue = 0;

int i=0;
while(funcArray[i].identif != NULL)
  {
  if(strcmp(funcArray[i].identif->str,"main" ) == 0)
    {/* Ak som vo func Array nasiel funkciu main, nastavujem na nu pociatok vykonavania instr listu */
    ListJump(InstList ,funcArray[i].function);
    break;
    }
  i++;
  }
while ((InstList->Active != NULL) && (ErrorFlag == NoErr) && (returnValue != INT_STOP_RETURN)) {
  returnValue = ExecInstr((InstList->Active->data));
  Succ(InstList);
}
return returnValue;

}

/** Pokusny main funkce ktore interpretuje musia byt aj v hlavnom maine programu */

