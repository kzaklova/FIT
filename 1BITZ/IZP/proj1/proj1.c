/*
 * Soubor:  proj1.c
 * Datum:   2011/08/03
 * Autor:   Katerina Zaklova, xzaklo00@stud.fit.vutbr.cz
 * Projekt: Casove udaje, projekt c. 1 pro predmet IZP
 * Popis:   Program provadi prevod cisel na presne casove udaje (tydny, dny, hodiny, minuty, sekundy) dle zadanych parametru.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <errno.h>

// ASCII hodnota znaku ENTER
#define ENTER 10
// pocet sekund v tydnu - 60*60*24*7
#define TYDEN 604800
// pocet sekund v jednom dnu - 60*60*24
#define DEN 86400
// pocet sekund v hodine - 60*60
#define HODINA 3600
// pocet sekund v minute
#define MINUTA 60

/** Kody chyb programu */
enum tecodes
{
  EOK = 0,     /**< Bez chyby */
  ECLWRONG,    /**< Chybny prikazovy radek */
  EMAX,        /**< Prilis velke cislo */
  ENUMBER,     /**< Neni cislo */
  EUNKNOWN,    /**< Neznama chyba */
};

/** Stavove kody programu */
enum tstates
{
  CHELP,       /**< Napoveda */
  CSEC,        /**< Sekundy */
  CMIN,        /**< Minuty */
  CHOURS,      /**< Hodiny */
  CDAYS,       /**< Dny */
  CWEEKS,      /**< Tydny */
};

/** Chybova hlaseni */
const char *ECODEMSG[] =
{
  /* EOK */
  "Vse OK\n",
  /* ECLWRONG */
  "Chybne parametry prikazoveho radku!\n",
  /*EMAX*/
  "Prilis velke cislo!\n",
  /*ENUMBER*/
  "Zadana hodnota neni cislo!\n",
  /*EUNKNOWN*/
  "Asi to bouchne...utikejte!\n",
};

const char *HELPMSG =
  "Program Casove udaje\n"
  "Autor: Katerina Zaklova (c) 2011\n"
  "Program provadi prevod cisla na casove udaje v radu sekund, minut, dnu a tydnu.\n"
  "Pouziti: proj1 --help\n"
  "         proj1\n"
  "         proj1 -t\n"
  "         proj1 -d\n"
  "         proj1 -h\n"
  "         proj1 -m\n"
  "         proj1 -s\n"
  "Popis parametru:\n"
  "Program se spousti zadanim vyse uvedenych parametru. Parametr --help vytiskne"
  "napovedu, parametry -t, -d, -h, -m, -s zobrazi vypocet s nejvyssi zadanou jednotkou"
  "(s jako sekundy, m jako minuty, h jako hodiny, d jako dny, t nebo prazdny parametr"
  "pro tydny). Jako vstup zadejte cislo, pokud program pri zpracovani narazi na"
  "neciselny znak, vezme jako vstupni hodnotu cial od zacatku radku po neciselny znak.\n";


/**
 * Parametry prikazove radky
 */
typedef struct params
{
  int ecode;        /**< Chybovy kod -> tecodes. */
  int state;        /**< Stavovy kod -> tstates. */
} TParams;

/**
 * Casove udaje
 */
typedef struct values
{
 unsigned long int sec;
 unsigned long int min;
 unsigned long int hrs;
 unsigned long int days;
 unsigned long int weeks;
 unsigned long int entries;
 // promenna pro detekci vstupu 0
 bool zero;
} TValues;

/**
 * Vytiskne hlaseni odpovidajici chybovemu kodu.
 * @param ecode kod chyby programu
 */
void printECode(int ecode)
{
  if (ecode < EOK || ecode > EUNKNOWN)
  { ecode = EUNKNOWN; }

  fprintf(stderr, "%s", ECODEMSG[ecode]);
}

/**
 * Zpracuje argumenty prikazoveho radku a vrati je ve strukture TParams.
 * Pokud je format argumentu chybny, ukonci program s chybovym kodem.
 * @param argc Pocet argumentu.
 * @param argv Pole textovych retezcu s argumenty.
 * @return Vrací analyzovane argumenty prikazoveho radku.
 */
TParams getParams(int argc, char *argv[])
{
  // inicializace struktury
  TParams result =
  {
    .ecode = EOK,
    .state = CSEC,
  };
  // zpracovani vstupnich parametru
  if (argc == 1) result.state = CWEEKS;
  else if (argc == 2)
  {
    if (strcmp("--help", argv[1]) == 0) result.state = CHELP;
    else if (strcmp("-t", argv[1]) == 0) result.state = CWEEKS;
    else if (strcmp("-d", argv[1]) == 0) result.state = CDAYS;
    else if (strcmp("-h", argv[1]) == 0) result.state = CHOURS;
    else if (strcmp("-m", argv[1]) == 0) result.state = CMIN;
    else if (strcmp("-s", argv[1]) == 0) result.state = CSEC;
    else result.ecode = ECLWRONG;
  }
  else result.ecode = ECLWRONG;

  return result;
}

/**
 * Zpracuje vstup po znacich a vrati ho ve strukture TValues.
 * Pokud vstup neni cislo nebo je hodnota vysledneho cisla
 * vetsi nez ULONG_MAX, program se s chybou ukonci.
 * @param TValues Struktura s casovymi udaji.
 * @return Vrací v polozce struktury pocet sekund.
 */
int getInput(TValues *values)
{
  int n;
  unsigned long int number = 0;
 
  while ((n = getchar()) != EOF && n != ENTER)
  { 
    if (isdigit(n))
    {
      // kontrola preteceni
      if (((ULONG_MAX - (n - '0'))/10) < number)
      {
	printECode(EMAX);
	return EXIT_FAILURE;	
      }
      // prevod na cislo
      number = number * 10 + (n - '0');
    }
    else
    {
      // neni cislo
      printECode(ENUMBER);
      return EXIT_FAILURE;
    }
      // detekce nuloveho vstupu
  if (values->sec == 0 && number == 0) values->zero = true;
  else values->zero = false;
  }
  if (values->zero == true) return values->zero;
  // ulozime do struktury
  values->sec = number;
  return values->sec;
}

/**
 * Vytiskne casove udaje na vystup a nazev udaje
 * sklonuje dle pravidel ceskeho pravopisu.
 * @param unsigned long int sec Polozka struktury se zadanym udajem k tisku.
 * @param char *asec[] Pole s klicovymi slovy pro tisk.
 */
void printEntries(unsigned long int sec, char *asec[])
{
  //tiskneme jen pro hodnoty vetsi nez 0
  if (sec > 0)
  {
    if (sec == 1) printf("%lu %s", sec, asec[0]);
    else if (sec > 1 && sec < 5) printf("%lu %s", sec, asec[1]);
    else if (sec >= 5) printf("%lu %s", sec, asec[2]);
    printf("%s", asec[3]);
  }
}

/**
 * Prepocita dle argumentu prikazove radky vstupni hodnotu na
 * tydny/dny/hodiny/minuty/sekundy a ulozi jejich hodnoty do struktury TValues.
 * Mezi jednotlivymi kroky vola funkci k tisku hodnot.
 * @param TValues Struktura s casovymi udaji.
 */
void countEntries(TValues *values)
{
  // pole textovych retezcu - klicova slova
  char *asec[4] = {"sekunda", "sekundy", "sekund", "\n"};
  char *amin[4] = {"minuta", "minuty", "minut"};
  char *ahrs[4] = {"hodina", "hodiny", "hodin"};
  char *adays[4] = {"den", "dny", "dnu"};
  char *aweeks[4] = {"tyden", "tydny", "tydnu"};
  
  // prepocitame vysledne hodnoty, ulozime do struktury a zavolame funkci,
  //ktera je vytiskne 
  switch (values->entries)
  {
    case 't':
      values->weeks = values->sec / TYDEN;
      values->sec = values->sec % TYDEN;
      // kdy tisknout mezeru a kdy ne
      if (values->sec != 0) aweeks[3] = " ";
      else aweeks[3] = "\n";
      printEntries(values->weeks, aweeks);      
    case 'd':
      values->days = values->sec / DEN;
      values->sec = values->sec % DEN;
      if (values->sec != 0) adays[3] = " ";
      else adays[3] = "\n";
      printEntries(values->days, adays);
    case 'h':
      values->hrs = values->sec / HODINA;
      values->sec = values->sec % HODINA;
      if (values->sec != 0) ahrs[3] = " ";
      else ahrs[3] = "\n";
      printEntries(values->hrs, ahrs);
    case 'm':
      values->min = values->sec / MINUTA;
      values->sec = values->sec % MINUTA;
      if (values->sec != 0) amin[3] = " ";
      else amin[3] = "\n";
      printEntries(values->min, amin);
    case 's' :
      printEntries(values->sec, asec);
      break;
  };
}

/////////////////////////////////////////////////////////////////
/**
 * Hlavni program.
 */
int main(int argc, char *argv[])
{
  // inicializace struktury
  TValues values;
  values.sec = 0;
  values.min = 0;
  values.hrs = 0;
  values.days = 0;
  values.weeks = 0;
  values.entries = 0;
  values.zero = false;
  
  TParams params = getParams(argc, argv);
  if (params.ecode != EOK)
  {
    printECode(params.ecode);
    return EXIT_FAILURE;
  }

// // hlavni cast programu
  if (params.state == CHELP) printf("%s", HELPMSG);
  else
  { 
    if (getInput(&values))
    {
      switch (params.state)
      {
	case CSEC:
	  values.entries='s';
	  countEntries(&values);
	  break;
	case CMIN:
	  values.entries='m';
	  countEntries(&values);
	  break;
	case CHOURS:
	  values.entries='h';
	  countEntries(&values);
	  break;
	case CDAYS:
	  values.entries='d';
	  countEntries(&values);
	  break;
	case CWEEKS:
	  values.entries='t';
	  countEntries(&values);
	  break;
      }
      // tisk vysledku pro nulovy vstup
      if (values.zero == true) printf("0 sekund\n");
    }
  }
  return EXIT_SUCCESS;
}
