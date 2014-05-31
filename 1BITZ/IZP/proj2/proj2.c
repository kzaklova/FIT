/*
 * Soubor:  proj2.c
 * Datum:   2011/11/01
 * Autor:   Katerina Zaklova, xzaklo00@fit.vutbr.cz
 * Projekt: Iteracni vypocty, projekt c. 2 pro predmet IZP
 * Popis:   Program zpracovava vstupni poloupnosti ciselnych hodnot a provadi vypocet matematickych a
 * geometrickych funkci - arcsin, logaritmus, delka cary s odchylkou i bez.
 * --------------------------------------------------------------------------------------------------
 * pozn. V zadani je uvedeno, ze knihovni funkce math.h muzeme pouzivat jen v oduvodnenych pripadech.
 * Z duvodu malo presneho a neprilis efektivniho algoritmu pro arcsin jsem implementovala tuto fci
 * pomoci Taylorova rozvoje pro arctan. Touto metodou jsem dosahla rychlejsiho a presnejsiho vypoctu,
 * se kterym pri testovani nebyl jediny problem. Tim padem jsem ale musela prevest hodnotu x pro
 * arcsin na ekvivalentni hodnotu pro arctan, coz bylo mozne jedine za pouziti knihovni funkce "sqrt".
 * Dale jsem zde pri doporucenych upravach z matematicke literatury musela vyuzit take funkci "fabs".
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <limits.h>
#include <errno.h>
#include <float.h>
#include <math.h>

//konstanty ze stranky zadani projektu
const double IZP_E = 2.7182818284590452354;
const double IZP_PI = 3.14159265358979323846;
const double IZP_2PI = 6.28318530717958647692;
const double IZP_PI_2 = 1.57079632679489661923;
const double IZP_PI_4 = 0.78539816339744830962;
const double IZP_PI_6 = 0.52359877559829887307;

// tisk
#define PRINT "%.10e\n"
// navratova hodnota fci na zpracovani vstupu
#define OK 0
//horni mez logaritmu
#define HB 1.5
//dolni mez logaritmu
#define LB 0.5

/** Kody chyb programu */
enum tecodes
{
	EOK = 0,     /**< Bez chyby */
	ECLWRONG,    /**< Chybny prikazovy radek. */
	EHELP,       /**< Malo parametru */
	ESIG,        /**< Chybne sigdig */
	ELOG,        /**< Chybny zaklad logaritmu */
	EER,        /**< Chybne zadani relativni chyby */
	EUNKNOWN,    /**< Neznama chyba */
};

/** Stavove kody programu */
enum tstates
{
	CHELP,       /**< Napoveda */
	CLBL,        /**< Lomena cara */
	CLBE,        /**< Lomena cara s chybou */
	CARS,        /**< Arcus sinus */
	CLOG,        /**< Logaritmus */
};

/** Chybova hlaseni odpovidajici chybovym kodum. */
const char *ECODEMSG[] =
{
	/* EOK */
	"Vse v poradku.\n",
	/* ECLWRONG */
	"Chybne parametry prikazoveho radku!\n",
	/* EHELP */
	"Chybne parametry prikazoveho radku, pro napovedu zadejte parametr -h\n",
	/* ESIG */
	"Chybny parametr urcujici presnost!\n",
	/* ELOG */
	"Chybny zaklad logaritmu!\n",
	/* EER */
	"Chybne zadani relativni chyby, musi byt nezaporne cislo!\n",
	/* EUNKNOWN */
	"Neznama chyba!\n",

};

const char *HELPMSG =
	"  ________________________\n"
	"||                        ||\n"
	"||Program Iteracni vypocty||\n"
	"||________________________||\n\n"
	"Autor: Katerina Zaklova (c) 2011\n"
	"-----------------------------------------------------------------\n"
	"Program provadi vypocty matematickych funkci arcus-sinus a obecny\n"
	"logaritmus, dale geometrickych funkci - vypocita delku lomene cary\n"
	"spojujici posloupnost zadanych bodu, a to bud s relativni chybou\n"
	"(jako nejdelsi a nejkratsi moznou caru) nebo bez chyby. \n"
	"-----------------------------------------------------------------\n"
	"Pouziti:\n"
	"proj2  -h                vytiskne napovedu\n"
	"proj2 --lbl              vypocita delku lomene cary bez chyby\n"
	"proj2 --lble ERR         vypocita delku lomene cary s chybou,\n"
	"                         ERR je parametr urcujici chybu\n"
	"proj2 --arcsin sigdig    vypocita arcus-sinus zadane hodnoty,\n"
	"                         sigdig je parametr urcujici presnost vypoctu\n"
	"proj2 --logax sigdig a   vypocita obecny logaritmus zadane hodnoty,\n"
	"                         sigdig je parametr urcujici presnost vypoctu,\n"
	"                         a je zaklad logaritmu\n";


// Struktura - parametry prikazove radky
typedef struct params
{
	unsigned int sigdig;	/**< Presnost vypoctu */
	double a;		/**< Zaklad logaritmu */
	double err;		/**< Chyba mereni lomene cary*/
	int ecode;		/**< Chybovy kod programu, odpovida vyctu tecodes. */
	int state;		/**< Stavovy kod programu, odpovida vyctu tstates. */
} TParams;

// Struktury - souradnice bodu
typedef struct prev
{
	double x, y;		/**< Souradnice predchoziho bodu */
} TPrev;

typedef struct next
{
	double x, y;		/**< Souradnice dalsiho bodu */
} TNext;

// Struktura - vysledky lomene cary
typedef struct result
{
	double res1;		/**< Nejmensi delka */
	double res2;		/**< Nejvetsi delka */
} TRes;

// Vytiskne hlaseni odpovidajici chybovemu kodu
void printECode(int ecode)
{
	if (ecode < EOK || ecode > EER)
	{ ecode = EUNKNOWN; }

	fprintf(stderr, "%s", ECODEMSG[ecode]);
}

// Zpracuje argumenty prikazoveho radku a vrati je ve strukture TParams. 
TParams getParams(int argc, char *argv[])
{
	TParams result =
	{ // inicializace struktury
		.sigdig = 0,
		.a = 0,
		.err = 0,
		.ecode = EOK,
		.state = CLBL,
	};
	if (argc == 1) result.ecode = EHELP;
	if (argc == 2)
	{	// tisk napovedy
		if (strcmp ("-h", argv[1]) == 0) result.state = CHELP;
		// lomena cara bez chyby
		else if (strcmp ("--lbl", argv[1]) == 0) result.state = CLBL;
		else result.ecode = EHELP;
	}

	// lomena cara s chybou
	else if (argc == 3 && strcmp ("--lble", argv[1]) == 0 )
	{
		// parametr odchylky
		char *pAux;
		result.err = strtod((argv[2]), &pAux);
		if (*pAux != '\0' || (result.err < 0) || (result.err > DBL_MAX)) result.ecode = EER;
		else result.state = CLBE;
	}

	else if ((argc == 3 && strcmp ("--lble", argv[1]) != 0)|| argc == 4)
	{
		// parametr presnosti
		char *pAux2;
		result.sigdig =  strtol((argv[2]), &pAux2, 10);
		if (*pAux2 != '\0' || (result.sigdig < 1) || (result.sigdig > DBL_DIG)) result.ecode = ESIG;
		else result.ecode = EOK;
		// arcus-sinus
		if (argc == 3 && strcmp ("--arcsin", argv [1]) == 0) result.state = CARS;
		// logaritmus
		else if (argc == 4 && strcmp ("--logax", argv [1]) == 0)
		{
			// parametr a
			char *pAux3;
			result.a = strtod((argv[3]), &pAux3);
			if (*pAux2 != '\0' || result.a <= 0 || result.a == 1) result.ecode = ELOG;
			else result.state = CLOG;
			}
		else result.ecode = EHELP;
	}

	else result.ecode = ECLWRONG;

	return result;
}

/**
 *!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *APROXIMACE FUNKCI - ARCSIN, LOGAX
 *!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */

/**
 * Popis funkce
 * Funkce getInput nacita hodnoty double ze vstupu a uklada do polozky struktury
 * k dalsimu zpracovani
 * @param *x je polozka struktury, kam nacitame vstupni hodnotu
 * @return funkce vraci hodnotu OK(=0), pokud vse probehlo v poradku, pokud je
 * vstupem NAN, pak vraci NAN, kdyz narazi na konec vstupu, vraci EOF
 */
double getInput (double *x)
{
	int getin;
	getin = scanf ("%lf", x);
	if (getin == 0)
	{
		scanf ("%*s");
		*x = NAN;
		return *x;
	}
	if (getin != EOF)
	{
		if(getin == 0)
		{
			scanf ("%*s");
			*x = NAN;
			return *x;
		}
	}
	else if(getin == EOF) return getin;
	return OK;
}

/**
 * Popis funkce
 * Funkce logar zpracovava vstupni posloupnost znaku s parametrem sigdig (urcuje
 * presnost vypoctu) jako podil funkci prirozenych logaritmu ln(x) a ln(a).
 * Fce je volana dvakrat a zpracovana v main jako podil ln x/ln a.
 * @param x je hodnota typu double, ktera udava argument
 * @param eps je hodnota typu double, ktera udava "presnost", se kterou je
 * nutne logax vypocitat
 * @return funkce vraci hodnotu typu double, coz je prirozeny logaritmus dane
 * vstupni hodnoty
 * pozn. byl pouzit kod z meho projektu 2 z IZP z lonskeho roku
 */
double logar (double x, double eps)
{
	double res = 0;

	// osetreni mimoradnych vstupnich hodnot
	if (x < 0)
	{
		res = NAN;
		return res;
	}
	if (x == 0)
	{
		res = -INFINITY;
		return res;
	} 
	if (x == INFINITY)
	{
		res = INFINITY;
		return res;
	}

	double hb = HB; // horni mez
	double lb = LB; // dolni mez
	double av = 0; // pomocna promenna

	// heuristika
	if (x > hb)
	{
		while (x > hb)
		{
			x /= hb;
			av += logar (hb, eps);
		}
	}
	if (x < lb)
	{
		while (x < lb)
		{
			x /= lb;
			av += logar (lb, eps);
		}
	}

	// prirozeny logaritmus - implemetace Taylorova rozvoje
	double k = 1;
	double item_ln = 1;
	double sum_ln = item_ln; 
	double t = (x - 1) / (x + 1);
	double pw = t * t;

	while (fabs (item_ln) >= eps*fabs (sum_ln))
	{
		k += 2;
		item_ln = item_ln * (pw * (k - 2) / k);
		sum_ln += item_ln;
	}
	sum_ln = 2 * t * sum_ln;
	// pokud se provadela heuristika
	sum_ln = sum_ln + av;
	res = sum_ln;

	return res;
}

/**
 * Popis funkce
 * Funkce arsin zpracovava vstupni hodnoty v rozmezi (-1) - 1 a pocita arcsin(x).
 * Funkce byla implementovana pomoci Taylorova rozvoje pro arctan, z duvodu
 * presnejsiho a rychlejsiho vypoctu nez maji samotne rady pro arcsin.
 * @param x je hodnota typu double, ktera udava argument
 * @param eps je hodnota typu double, ktera udava "presnost", se kterou je nutne
 * arcsin vypocitat
 * @return funkce vraci hodnotu typu double, coz je arcus sinus dane vstupni
 * hodnoty
 */
double arsin(double x, double eps)
{
	double res = 0;
	// detekce hodnot pro heuristiku - jestli je x v danem intervalu
	int sq = 0;
	int one = 0;
	// detekce zaporneho vstupu
	int neg = 0;

	// arcsin (-x) == -arcsin(x)
	if (x < 0) 
	{
		x = fabs(x);
		neg = 1;
	}
	// hodnoty mimo interval funkce
	if (x > 1)
	{
		res = NAN;
		return res;
	}

	// prevod kvuli vypoctu arctan
	x = x / sqrt((1 - x) * (1 + x));

	// heuristika podle arctan
	if (x > 1)
	{
		x = 1 / x;
		one = 1;
	}
	if(x > (2 - sqrt(3)) && x <= 1)
	{
		sq = 1;
		x = ((sqrt(3) * x - 1) / (sqrt(3) + x));
	}

	// samotna implementace Taylorovy rady pro arctan
	double i = 1;
	double item_as = x;
	double pw = x * x;
	double sum_as = item_as;
	double prev_as = 0;
	while (fabs (prev_as - sum_as) >= eps)
	{
		i += 2;
		item_as = (-item_as * pw * (i - 2)) / i;
		prev_as = sum_as;
		sum_as += item_as;
	}
	// pokud bylo x (2-sqrt(3);1>
	if (sq == 1) sum_as = IZP_PI_6 + sum_as;
	// pokud bylo x (1;inf)
	if (one == 1) sum_as = IZP_PI_2 - sum_as;
	res = sum_as;
	// pokud bylo x zaporne
	if (neg == 1) res = -(res);
	if (res == -0) res = 0;

	return res;
}

/**
 *!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *LOMENA CARA + LOMENA CARA S CHYBOU
 *!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */


/**
 * Popis funkce
 * Funkce zpracovava vstupni souradnice bodu a pocita prubeznou delku lomenne cary
 * s relativni chybou (parametr err je nenulovy), popr. delku bez chyby (err je 0)
 * @param *prev struktura se souradnicemi predchoziho bodu
 * @param *next struktura se souradnicemi nasledujiciho bodu
 * @param *res struktura s vysledky
 * @param err relativni chyba
 */
void countLbe (TPrev *prev, TNext *next, TRes *res, double err)
{
	double w, h, wmin, hmin, wmax, hmax, tmp1, tmp2;
	// mezivysledky
	tmp1 = 0, tmp2 = 0;
	// "sirka"
	w = fabs(next->x - prev->x);
	// "vyska"
	h = fabs(next->y - prev->y);
	// pomocne vypocty
	wmin = w - 2*err;
	hmin = h - 2*err;
	wmax = w + 2*err;
	hmax = h + 2*err;
	/* Nejmensi vzdalenost */
	// body lezi "na sobe"
	if (w == 0 && h == 0) tmp1 = 0;
	// body jsou "nad sebou v rade"
	else if (w == 0 && h != 0) tmp1 = hmin;
	// body jsou "vedle sebe v rade"
	else if (w != 0 && h == 0) tmp1 = wmin;
	// vypocet pro bezny stav
	else tmp1 = sqrt((wmin*wmin) + (hmin*hmin));
	if (wmin < 0 && hmin < 0) tmp1 = 0;
	res->res1 = res->res1 + tmp1;
	/* Nejvetsi vzdalenost */
	// body jsou na sobe
	if (w == 0 && h == 0) tmp2 = 0;
	// vypocet pro bezny stav
	else tmp2 = sqrt((wmax*wmax) + (hmax*hmax));
	res->res2 = res->res2 + tmp2;

	// priradime novejsi souradnice na misto starsich
	prev->x = next->x;
	prev->y = next->y;

}

/**
 * Popis funkce
 * Funkce zpracovava parametry *x a y a pocita mocninu
 * @param *x zaklad
 * @param y mocnitel
 * @return funkce vraci hodnotu mocniny
 */
double power (double x, double y)
{
	int i;
	double j;
	if (y == 0) x = 1;
	j = x;
	for (i = 2; i <= y; i++)
	{
		x = x*j;
	}
	return x;
}
/////////////////////////////////////////////////////////////////
/**                                                           **/
/*                       Hlavni program                        */
/**                                                           **/
/////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	TParams params = getParams(argc, argv);
	if (params.ecode != EOK)
	{
		printECode(params.ecode);
		return EXIT_FAILURE;
	}

	if (params.state == CHELP)
	{
		printf("%s", HELPMSG);
		return EXIT_SUCCESS;
	}

	// inicializace promennych a struktur
	TPrev prev;
	prev.x = 0;
	prev.y = 0;
	TNext next;
	next.x = 0;
	next.y = 0;
	TRes res;
	res.res1 = 0;
	res.res2 = 0;

	int ecode = EOK;
	double eps = 0;
	double x = 0.1;
	double fakeerr = 0;
	double aux = 0;

	if(params.state == CLBL || params.state == CLBE)
	{
		getInput(&prev.x);
		getInput(&prev.y);
		if (isnan(prev.x) || isnan(prev.y)) printf (PRINT, NAN);
		else printf(PRINT, res.res1);
		if (params.state == CLBE)
		{
			if (isnan(prev.x) || isnan(prev.y)) printf (PRINT, NAN);
			else printf(PRINT, res.res1);
		}
		while (getInput(&next.x) != EOF)
		{
			aux = getInput(&next.y);
			if (aux == EOF) next.y = NAN;
			if (params.state == CLBL)
			{
				countLbe(&prev, &next, &res, fakeerr);
				printf(PRINT, res.res1);
			}
			else if (params.state == CLBE)
			{
				countLbe(&prev, &next, &res, params.err);
				printf(PRINT, res.res1);
				printf(PRINT, res.res2);
			}
		}
	}
	else if(params.state == CARS || params.state == CLOG)
	{
		eps = power(x,(params.sigdig + 1));
		while(getInput(&prev.x) != EOF)
		{
			if(params.state == CLOG)
			{
				next.x = params.a;
				printf (PRINT, (logar(prev.x, eps)/logar(next.x, eps)));
			}
			else if(params.state == CARS)
			{
				printf(PRINT, arsin(prev.x, eps));
			}
		}
	}

	if (ecode != EOK)
	{
		printECode(ecode);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
