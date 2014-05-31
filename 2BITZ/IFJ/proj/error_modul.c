/* ************************ error_modul.c *********************************** */
/* Soubor:              error_modul.c - Chybove hlasky                        */
/* Kodovani:            UTF-8                                                 */
/* Datum:               listopad.2011     v                                   */
/* Predmet:             Formalni jazyky a prekladace (IFJ)                    */
/* Projekt:             Implementace interpretu jazyka IFJ11                  */
/* Varianta zadani:                                                           */
/* Titul,Autori, login:         Škvařilová Radka      xskvar06                */
/*                              Zaklová Kateřina      xzaklo00                */
/*                              Zapletal Libor        xzaple29                */
/*Zivouci reinkarnace Buddhy    Zaoral Tomáš          xzaora02                */
/*Pokemoni cisar                Uhliarik Michal       xuhlia01                */
/* ************************************************************************** */

/** Modul chybovych hlaseni */
/** Poznamka kouknout realoc */
#include "error_modul.h"

#define BASIC_SIZE 30

/* Inicializacia globalnych premennych aby nedoslo k chybe linkeru */
int ErrorFlag = 0;
int ErrorCounter = 0;
int ReallocCounter = 0;


/* Chybove hlasenia odpovedajuce  vyctu definovanym chybam */
const char *ECODEMSG[] =
{
  /* EUNKNOWN */
  "Vyskytla se neznama chyba ,\n vzpoura stroju zacina! \nProsme o zmilovani nase roboticke pany!",
  /* Lexiklani chyba*/
  "sa vyskytla lexikalna chyba.\n",
  /* Syntakticka chyba*/
  "sa vyskytla syntakticka chyba.\n",
  /* Semanticka chyba */
  "sa vyskytla semanticka chyba.\n",
  /* Chyba interpretace */
  "sa vyskytla chyba interpretacie dat.",
  /* Vnutorna chyba interpretu*/
  "Za behu programu sa vyskytla vnutorna chyba! \n"
};

void PrintErrItem(TError *p_temp)
{ /* Vytlaci jedno chybove hlasenie */
if((p_temp->ErrorType < Unknown) || (p_temp->ErrorType > ProgErr))
  {
  p_temp->ErrorType = Unknown;
  }
if(p_temp->ErrorType == ProgErr) fprintf(stderr, "\n%s",ECODEMSG[p_temp->ErrorType]);
else fprintf(stderr, "Na riadku %d %s \n", p_temp->LineNumber, ECODEMSG[(p_temp->ErrorType)]);

}

int MakeErrList(void)
{ /* Vytvori a inicializuje zoznam chyb */
if(((ErrorList) = malloc((sizeof(TError))*BASIC_SIZE)) == NULL)
  {
  fprintf(stderr, "\n%s",ECODEMSG[ProgErr]);
  FreeError();
  return ProgErr;
  }
for(int i=0; i< BASIC_SIZE; i++)
  {
  ErrorList[i].LineNumber = 0;
  ErrorList[i].ErrorType = 0;
  }
return 0;
}

void AddError(int CisloRadku, int TypChyby)
{ /* Vytvory novy zaznam o chybe */
TError *p_temp;
ErrorFlag = TypChyby;
//if(((p_temp)=malloc(sizeof(TError))) == NULL) fprintf(stderr, "\n%s",ECODEMSG[ProgErr]);
if(ErrorCounter >= (BASIC_SIZE + BASIC_SIZE * ReallocCounter -1))
   { /* Je vice chyb nez hranice pole */
   ReallocCounter++;
   if((p_temp =realloc((ErrorList),sizeof(TError)*(BASIC_SIZE+1))) == NULL)
      {
      fprintf(stderr, "\n%s",ECODEMSG[ProgErr]);
      FreeError();
      }
   ErrorList = p_temp;
   }
ErrorList[ErrorCounter].LineNumber = CisloRadku;
ErrorList[ErrorCounter].ErrorType = TypChyby;
ErrorCounter++;
}

void PrintError(void)
{ /* Funkcia vytlaci vsetky chybove hlasenia */
for(int i=0;i <ErrorCounter; i++)
   {
   PrintErrItem(&(ErrorList[i]));
   }
}

void FreeError(void)
{ /* Uvolnuje pole chyb */
free(ErrorList);
ErrorList = NULL;
}
