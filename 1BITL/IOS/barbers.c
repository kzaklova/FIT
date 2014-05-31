/*
 * Soubor:  barbers.c
 * Datum:   2011/04/27
 * Autor:   Katerina Zaklova, xzaklo00@stud.fit.vutbr.cz
 * Projekt: Synchronizace paralelnich procesu, proj2 do predmetu IOS
 * Popis:   Program resi problem holice a zakazniku za pomoci semaforu.
 */

#include<semaphore.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<limits.h>
#include<unistd.h>
#include<signal.h>
#include<fcntl.h>
#include<time.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/stat.h>
#include<sys/time.h>
#include<sys/types.h>


/**
 * Kody chyb programu
 **/
enum tecodes
{
  EOK = 0,     /**< Bez chyby */
  ECLWRONG,    /**< Proste nejaka chyba... */
  EHELP,       /**< Chybne parametry + napoveda */
  EFILE,       /**< Otevirani souboru */
  EKEY,	       /**< Vytvareni klice */
  ESHM,	       /**< Vytvareni shm */
  ESEM,	       /**< Alokace sem */
  EINIT,       /**< Inicializace sem */
  EPROC,       /**< Proces */
  EUNKNOWN,    /**< Neznama chyba */
};

/**
 * Struktura sdilene promenne
 **/
typedef struct
{
  int customer;			//citac zakazniku
  int servedC;			//obslouzeni zakaznici
  int action;			//citac akci
  int available;		//volna mista
} TShm;

/**
 * Struktura semafory
 **/
typedef struct
{
  sem_t memory;			//zapisy do pameti
  sem_t bReady;			//zakaznik ceka, az barber bude ready
  sem_t cReady;			//barber ceka, az zakaznik bude ready
  sem_t cServe;			//zakaznik ceka, az bude ostrihan a holic skonci
  sem_t seat;			//volno/obsazeno u holice
  sem_t sleep;			//holic usne a ceka az ho vzbudi zakaznik
  sem_t finish;			//hotovo, konec
} TSem;

/**
 * Struktura parametry prikaz. radky
 **/
typedef struct params
{
  int Q;
  int GenC;
  int GenB;
  int customers;
  FILE *filename;
  int ecode;
} TParams;

/**
 * Chybova hlaseni odpovidajici kodum
 **/
const char *ECODEMSG[] = {
  /* EOK */
  "Vse v poradku.\n",
  /* ECLWRONG */
  "Proste chyba :)\n",
  /* EHELP */
  "Chybne parametry prikazove radky! Program se spousti ./barbers Q GenC GenB customers filename\n"
    "Q je pocet zidli v cekarne\n"
    "GenC je cas generovani zakazniku\n"
    "GenB je cas obsluhy holice\n"
    "customers je pocet zakazniku\n"
    "filename je nazev souboru, popr. - pro stdout\n"
    "Vsechny ciselne parametry jsou kladna cela cisla.\n",
  /* EFILE */
  "Chyba pri otevreni souboru.",
  /* EKEY */
  "Chyba pri generovani klice.\n",
  /* ESHM */
  "Chyba pri vytvareni sdilene pameti.",
  /* ESEM */
  "Chyba pri alokaci semaforu.",
  /* EINIT */
  "Chyba pri inicializaci semaforu.",
  /* EPROC */
  "Chyba vytvareni procesu.",
  /* EUNKNOWN */
  "Nastala nepredvidana chyba!\n",
};

/**
 * Vytiskne hlaseni odpovidajici chybovemu kodu
 * @param ecode kod chyby programu
 */
void
printECode (int ecode)
{
  if (ecode < EOK || ecode > EUNKNOWN)
    {
      ecode = EUNKNOWN;
    }

  fprintf (stderr, "%s", ECODEMSG[ecode]);
}

/**
 * Zpracuje argumenty prikazoveho radku a vrati je ve strukture TParams.
 * Pokud je format argumentu chybny, ukonci program s chybovym kodem.
 * @param argc Pocet argumentu.
 * @param argv Pole textovych retezcu s argumenty.
 * @return Vraci analyzovane argumenty prikazoveho radku.
 */
TParams
getParams (int argc, char *argv[])
{
  TParams result = {		// inicializace struktury
    .Q = 0,
    .GenC = 0,
    .GenB = 0,
    .customers = 0,
    .filename = NULL,
    .ecode = EOK,
  };

  char *endptr;

  if (argc == 6)
    {
      long int Zidle = 0;
      Zidle = strtol (argv[1], &endptr, 10);
      if ((Zidle > INT_MAX) || Zidle < 1 || (errno != 0) || *endptr != '\0')
	{
	  result.ecode = EHELP;
	  return result;
	}
      else
	{
	  result.Q = Zidle;
	}
      long int Cas = 0;
      Cas = strtol (argv[2], &endptr, 10);
      if ((Cas > INT_MAX) || Cas < 0 || (errno != 0) || *endptr != '\0')
	{
	  result.ecode = EHELP;
	  return result;
	}
      else
	{
	  result.GenC = Cas;
	}
      long int Obsluha = 0;
      Obsluha = strtol (argv[3], &endptr, 10);
      if ((Obsluha > INT_MAX) || Obsluha < 0 || (errno != 0)
	  || *endptr != '\0')
	{
	  result.ecode = EHELP;
	  return result;
	}
      else
	{
	  result.GenB = Obsluha;
	}
      long int Zakaznici = 0;
      Zakaznici = strtol (argv[4], &endptr, 10);
      if ((Zakaznici > INT_MAX) || Zakaznici < 1 || (errno != 0)
	  || *endptr != '\0')
	{
	  result.ecode = EHELP;
	  return result;
	}
      else
	{
	  result.customers = Zakaznici;
	}
      //stdout x ukladani do souboru
      if (strcmp ("-", argv[5]) == 0)
	{
	  result.filename = stdout;
	}
      else
	{
	  result.filename = fopen (argv[5], "w");
	  if (result.filename == NULL)
	    {
	      result.ecode = EFILE;
	    }
	}
    }

  else
    {
      result.ecode = EHELP;
    }

  return result;
}

/**
 * Uvolni sdilenou pamet a semafory
 * @param TShm struktura sdilenych promennych
 * @param TSem struktura semaforu
 */
void
freeShm (TShm * shm, int shm_id, TSem * sem, int sem_id)
{
  shmdt (shm);
  shmctl (shm_id, IPC_RMID, NULL);
  shmdt (sem);
  shmctl (sem_id, IPC_RMID, NULL);
}

/**
 * Funkce holice
 * @param TParams struktura parametru prikazove radky
 * @param TShm struktura sdilenych promennych
 * @param TSem struktura semaforu.
 */
void
barber (TParams * params, TSem * sem, TShm * shm)
{
  srand (time (NULL));
  sem_wait (&sem->memory);
//pocet obslouzenych zakazniku je mensi nez pocet zakazniku zadany parametry
  while (shm->servedC < params->customers)
    {
//barber checks
      setbuf (params->filename, NULL);
      fprintf (params->filename, "%d: Barber: checks\n", shm->action);
      shm->action = shm->action + 1;
      sem_post (&sem->memory);
//barber ready
      sem_wait (&sem->sleep);
      sem_wait (&sem->memory);
      setbuf (params->filename, NULL);
      fprintf (params->filename, "%d: Barber: ready\n", shm->action);
      shm->action = shm->action + 1;
      sem_post (&sem->memory);
      sem_post (&sem->bReady);
//barber finished
      sem_wait (&sem->cReady);
      usleep (rand () % ((params->GenB * 1000) + 1));
      sem_wait (&sem->memory);
      setbuf (params->filename, NULL);
      fprintf (params->filename, "%d: Barber: finished\n", shm->action);
      shm->action = shm->action + 1;
      sem_post (&sem->memory);
      sem_post (&sem->cServe);
      sem_wait (&sem->sleep);
      sem_wait (&sem->memory);
    }
//konec
  sem_post (&sem->finish);
}

/**
 * Funkce zakaznika
 * @param TParams struktura parametru prikazove radky
 * @param TShm struktura sdilenych promennych
 * @param TSem struktura semaforu.
 */
void
customer (TParams * params, TSem * sem, TShm * shm)
{
  int customer;
//customer created
  sem_wait (&sem->memory);
  customer = shm->customer;
  shm->customer = shm->customer + 1;
  setbuf (params->filename, NULL);
  fprintf (params->filename, "%d: Customer %d: created\n", shm->action,
	   customer);
  shm->action = shm->action + 1;
  sem_post (&sem->memory);
//customer enters
  sem_wait (&sem->memory);
  setbuf (params->filename, NULL);
  fprintf (params->filename, "%d: Customer %d: enters\n", shm->action,
	   customer);
  shm->action = shm->action + 1;
  sem_post (&sem->memory);

  sem_wait (&sem->memory);
//v cekarne je plno :-(
  if (shm->available == 0)
    {				//customer refused
      shm->servedC = shm->servedC + 1;
      setbuf (params->filename, NULL);
      fprintf (params->filename, "%d: Customer %d: refused\n", shm->action,
	       customer);
      shm->action = shm->action + 1;
      sem_post (&sem->memory);
      return;
    }
  else
    {				//o zidlicku mene
      shm->available = shm->available - 1;
      sem_post (&sem->memory);
    }

  sem_wait (&sem->seat);
  sem_post (&sem->sleep);
//customer ready
  sem_wait (&sem->bReady);
  sem_wait (&sem->memory);
  shm->available = shm->available + 1;
  setbuf (params->filename, NULL);
  fprintf (params->filename, "%d: Customer %d: ready\n", shm->action,
	   customer);
  shm->action = shm->action + 1;
  sem_post (&sem->memory);
  sem_post (&sem->cReady);
//customer served
  sem_wait (&sem->cServe);
  sem_wait (&sem->memory);
  setbuf (params->filename, NULL);
  fprintf (params->filename, "%d: Customer %d: served\n", shm->action,
	   customer);
  shm->action = shm->action + 1;
  shm->servedC = shm->servedC + 1;
  sem_post (&sem->memory);
  sem_post (&sem->sleep);
  sem_post (&sem->seat);

  return;
}

/**
 * MAIN
 */
int
main (int argc, char *argv[])
{
//promenne
  int shm_id;
  int sem_id;
  TShm *shm;
  TSem *sem;
  int pid;

//nacteni parametru
  TParams params = getParams (argc, argv);

//kontrola spravnosti parametru
  if (params.ecode != EOK)
    {
      printECode (params.ecode);
      return EXIT_FAILURE;
    }

//klic pro sdilenou pamet
  key_t klic1 = ftok ("barbers", 1);
  if (errno != 0)
    {
      printECode (EKEY);
      if (params.filename != stdout)
	{
	  fclose (params.filename);
	}
      return EXIT_FAILURE;
    }

//vytvoreni sdilene pameti
  if ((shm_id = shmget (klic1, sizeof (TShm), IPC_CREAT | 0666)) < 0)
    {
      printECode (ESHM);
      if (params.filename != stdout)
	{
	  fclose (params.filename);
	}
      return EXIT_FAILURE;
    }

  if ((shm = (TShm *) shmat (shm_id, NULL, 0)) == (void *) -1)
    {
      freeShm (shm, shm_id, NULL, 0);
      printECode (ESHM);
      if (params.filename != stdout)
	{
	  fclose (params.filename);
	}
      return EXIT_FAILURE;
    }

//inicializace sdilenych prommenych
  shm->customer = 1;
  shm->servedC = 0;
  shm->action = 1;
  shm->available = params.Q;

//klic pro semafory
  key_t klic2 = ftok ("barbers", 2);
  if (errno != 0)
    {
      printECode (EKEY);
      if (params.filename != stdout)
	{
	  fclose (params.filename);
	}
      return EXIT_FAILURE;
    }

//vytvoreni sdilenych semaforu
  if ((sem_id = shmget (klic2, sizeof (TSem), IPC_CREAT | 0666)) < 0)
    {
      printECode (ESEM);
      if (params.filename != stdout)
	{
	  fclose (params.filename);
	}
      return EXIT_FAILURE;
    }

  if ((sem = (TSem *) shmat (sem_id, NULL, 0)) == (void *) -1)
    {
      freeShm (shm, shm_id, NULL, sem_id);
      printECode (ESEM);
      if (params.filename != stdout)
	{
	  fclose (params.filename);
	}
      return EXIT_FAILURE;
    }

//inicializace semaforu
  if (sem_init (&sem->memory, 1, 1))
    {
      freeShm (shm, shm_id, sem, sem_id);
      printECode (EINIT);
      if (params.filename != stdout)
	{
	  fclose (params.filename);
	}
      return EXIT_FAILURE;
    }
  if (sem_init (&sem->sleep, 1, 0))
    {
      freeShm (shm, shm_id, sem, sem_id);
      printECode (EINIT);
      if (params.filename != stdout)
	{
	  fclose (params.filename);
	}
      return EXIT_FAILURE;
    }
  if (sem_init (&sem->bReady, 1, 0))
    {
      freeShm (shm, shm_id, sem, sem_id);
      printECode (EINIT);
      if (params.filename != stdout)
	{
	  fclose (params.filename);
	}
      return EXIT_FAILURE;
    }
  if (sem_init (&sem->cReady, 1, 0))
    {
      freeShm (shm, shm_id, sem, sem_id);;
      printECode (EINIT);
      if (params.filename != stdout)
	{
	  fclose (params.filename);
	}
      return EXIT_FAILURE;
    }
  if (sem_init (&sem->cServe, 1, 0))
    {
      freeShm (shm, shm_id, sem, sem_id);
      printECode (EINIT);
      if (params.filename != stdout)
	{
	  fclose (params.filename);
	}
      return EXIT_FAILURE;
    }
  if (sem_init (&sem->seat, 1, 1))
    {
      freeShm (shm, shm_id, sem, sem_id);
      printECode (EINIT);
      if (params.filename != stdout)
	{
	  fclose (params.filename);
	}
      return EXIT_FAILURE;
    }
  if (sem_init (&sem->finish, 1, 0))
    {
      freeShm (shm, shm_id, sem, sem_id);
      printECode (EINIT);
      if (params.filename != stdout)
	{
	  fclose (params.filename);
	}
      return EXIT_FAILURE;
    }

//vytvoreni procesu holice
  int holic_pid = fork ();

  if (holic_pid == 0)
    {
      barber (&params, sem, shm);
      return EXIT_SUCCESS;
    }
  else if (holic_pid < 0)
    {
      freeShm (shm, shm_id, sem, sem_id);
      printECode (EPROC);
      if (params.filename != stdout)
	{
	  fclose (params.filename);
	}
      return EXIT_FAILURE;
    }

  srand (time (NULL));

//vytvoreni procesu zakazniku
  for (int i = 0; i < params.customers; i++)
    {
      usleep (rand () % ((params.Q * 1000) + 1));
      pid = fork ();
      waitpid (-1, 0, WNOHANG);

      if (pid == 0)
	{
	  customer (&params, sem, shm);
	  return EXIT_SUCCESS;
	}

      else if (pid < 0)
	{
	  params.customers = i;
	  printECode (EPROC);
	  if (params.filename != stdout)
	    {
	      fclose (params.filename);
	    }
	  break;
	}
    }

//uklidime :-)
  sem_wait (&sem->finish);
  if (params.filename != stdout)
    {
      fclose (params.filename);
    }
  freeShm (shm, shm_id, sem, sem_id);

  return EXIT_SUCCESS;
}
