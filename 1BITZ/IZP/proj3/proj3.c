/*
 * Soubor:  proj3.c
 * Datum:   2011/11/25
 * Autor:   Katerina Zaklova, xzaklo00@fit.vutbr.cz
 * Projekt: Maticove operace, projekt c. 3 pro predmet IZP
 * Popis:   Program provadi operace s maticemi jako maticovy soucet,
 * maticovy soucin, transpozice matice nebo maticovy vyraz. Dale program
 * umi vodni tok - vyhledava body s nejmensi hodnotou v okoli zadaneho
 * bodu, a kulecnik - projde matici stejnou cestou, jako by prosla koule
 * pri odpalu ve hre. Program take zvlada testovat spravnost vstupniho
 * souboru. Mezi jednotlivymi operacemi se prepiname prepinaci uvedenymi
 * v napovede. Ke spusteni operaci potrebujeme vstupni soubory. Napoveda
 * se vytiskne zadanim parametru -h.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

// specialni navratova hodnota fce pro detekci "false" vystupu
#define EFALSE 8
#define FALSE "false\n"
// sudy radek
#define EVEN i % 2 == 0
// lichy radek
#define ODD i % 2 == 1

/** Smery pohybu po sestiuhelnikove matici */
enum directions
{
	SZ = 0,      /**< Nahore nalevo*/
	SV,          /**< Nahore napravo*/
	V,           /**< Napravo */
	JV,          /**< Dole napravo*/
	JZ,          /**< Dole nalevo*/
	Z,           /**< Nalevo */
};

/** Kody chyb programu */
enum tecodes
{
	EOK = 0,     /**< Bez chyby */
	EHELP,       /**< Chybne parametry */
	EINPUT,      /**< Chyba na vstupu */
	EFILE,       /**< Chybny vstupni soubor */
	EMAT,        /**< Chybny format matice */
	EOVER,       /**< Souradnice mimo matici */ 
	EALLOC,      /**< Chyba pri alokaci */
	EUNKNOWN,    /**< Neznama chyba */
};

/** Stavove kody programu */
enum tstates
{
	CHELP,      /**< Napoveda */
	CTEST,      /**< Kontrola dat ve vstupnim souboru */
	CADD,       /**< Soucet matic*/
	CMULT,      /**< Soucin matic */
	CTRANS,     /**< Transpozice matice */
	CEXPR,      /**< Maticovy vyraz */
	CWATER,     /**< Vodni tok */
	CCAROM,     /**< Kulecnik */
};


/** Chybova hlaseni odpovidajici chybovym kodum. */
const char *ECODEMSG[] =
{
	/* EOK */
	"Vse v poradku.\n",
	/* EHELP */
	"Chybne parametry prikazoveho radku, pro napovedu zadejte parametr -h!\n",
	/* EINPUT */
	"Chybny vstup!\n",
	/* EFILE */
	"Chyba pri nacitani vstupniho souboru!\n",
	/* EMAT */
	"Chybny format matice!\n",
	/* EOVER */
	"Zadany bod lezi mimo matici!\n",
	/* EALLOC */
	"Chyba pri alokaci!\n",
	/* EUNKNOWN */
	"Neznama chyba!\n",

};

const char *HELPMSG =
	"  ________________\n"
	"||                ||\n"
	"||MATICOVE OPERACE||\n"
	"||________________||\n\n"
	"Autor: Katerina Zaklova (c) 2010\n"
	"-----------------------------------------------------------------\n"
	"Program provadi maticove operace - maticovy soucet, soucin,\n"
	"transpozici, maticovy vyraz a \"hry\" kulecnik a vodni tok.\n"
	"-----------------------------------------------------------------\n"
	"Pouziti:\n"
	"proj3 -h                            - vytiskne napovedu\n"
	"proj3 --test data.txt               - zkontroluje vstupni data a\n"
	"                                      vytiskne na vystup matici\n"
	"proj3 --add A.txt B.txt             - secte dve matice\n"
	"proj3 --mult A.txt B.txt            - vynasobi dve matice\n"
	"proj3 --trans A.txt                 - provede transpozici matice\n"
	"proj3 --expr A.txt B.txt            - vypocita maticovy vyraz\n" 
	"                                      (A*B(T) + A)\n"
	"proj3 --water r s M.txt             - provede simulaci vodniho\n"
	"                                      toku - najde body s nejnizsi\n"
	"                                      hodnotou v okoli zadaneho\n"
	"                                      bodu\n"
	"proj3 --carom r s dir power M.txt   - provede odpal kulecnikove\n"
	"                                      koule a vypise jeji pruchod\n"
	"                                      matici\n"
	"Popis parametru:\n"
	"soubor.txt                          - vstupni soubor\n"
	"r                                   - x-ova souradnice bodu,\n"
	"                                      nezaporne cele cislo\n"
	"s                                   - y-ova souradnice bodu,\n"
	"                                      nezaporne cele cislo\n"
	"dir                                 - smer odpalu (V,Z,JV,JZ,SV,SZ)\n"
	"power                               - sila odpalu, nezaporne \n"
	"                                      cele cislo\n";


// Struktura - parametry prikazove radky
typedef struct params
{
	char *filename;		/**< Vstupni soubor */
	char *filename2;	/**< Vstupni soubor 2 */
	int r, s;			/**< Vstupni souradnice */
	unsigned int power;	/**< Sila odrazu*/
	char *dir[6];		/**< Pole retezcu se smerem*/
	int direc;			/**< Vysledny smer */
	int ecode;			/**< Chybovy kod programu, odpovida vyctu tecodes. */
	int state;			/**< Stavovy kod programu, odpovida vyctu tstates. */
} TParams;

// Vytiskne hlaseni odpovidajici chybovemu kodu
void printECode(int ecode)
{
	if (ecode < EOK || ecode > EALLOC)
	{ ecode = EUNKNOWN; }

	fprintf(stderr, "%s", ECODEMSG[ecode]);
}

// Zpracuje argumenty prikazoveho radku a vrati je ve strukture TParams. 
TParams getParams(int argc, char *argv[])
{
	TParams result =
	{
	.filename = NULL,
	.filename2 = NULL,
	.r = 0,
	.s = 0,
	.power = 0,
	.dir = {"SZ", "SV", "V", "JV", "JZ", "Z"},
	// promennou smeru nemuzeme inicializovat na 0, protoze 0 urcuje smer
	.direc = -1,
	.ecode = EOK,
	.state = CTEST,
	};
/* Nacitani parametru */
	// napoveda
	if (argc == 2)
	{
		if (strcmp ("-h", argv[1]) == 0) result.state = CHELP;
		else result.ecode = EHELP;
	}
	// test a transpozice
	else if (argc == 3)
	{
		if (strcmp ("--test", argv[1]) == 0)
		{
			result.state = CTEST;
			result.filename = argv[2];
		}
		else if (strcmp ("--trans", argv[1]) == 0)
		{
			result.state = CTRANS;
			result.filename = argv[2];
		}
		else result.ecode = EHELP;
	}
	// scitani, nasobeni a maticovy vyraz
	else if (argc == 4)
	{
		if(strcmp ("--add", argv[1]) == 0)
		{
			result.state = CADD;
			result.filename = argv[2];
			result.filename2 = argv[3];
		}
		else if (strcmp ("--mult", argv[1]) == 0)
		{
			result.state = CMULT;
			result.filename = argv[2];
			result.filename2 = argv[3];
		}
		else if (strcmp ("--expr", argv[1]) == 0)
		{
			result.state = CEXPR;
			result.filename = argv[2];
			result.filename2 = argv[3];
		}
		else result.ecode = EHELP;
	}
	// vodni tok
	else if (argc == 5 && strcmp ("--water", argv[1]) == 0)
	{
		result.state = CWATER;
		char *pAux,*pAux2;
		// souradnice r, s
		result.r = strtol((argv[2]), &pAux, 10);
		if (*pAux != '\0' || (result.r > INT_MAX)) result.ecode = EHELP;
		else result.ecode = EOK;
		result.s = strtol((argv[3]), &pAux2, 10);
		if (*pAux2 != '\0' || (result.s > INT_MAX)) result.ecode = EHELP;
		else result.ecode = EOK;
		result.filename = argv[4];
	}
	// kulecnik
	else if (argc == 7 && strcmp ("--carom", argv[1]) == 0)
	{
		result.state = CCAROM;
		for (int i = 0; i < 6; i++)
		{
			// hledame smer
			if((strcmp (result.dir[i], argv[4])) == 0)
			result.direc = i;
		}
		if (result.direc != -1)
		{
			// souradnice r, s
			char *pAux,*pAux2, *pAux3;
			result.r = strtol((argv[2]), &pAux, 10);
			if (*pAux != '\0' || (result.r > INT_MAX)) result.ecode = EHELP;
			else result.ecode = EOK;
			result.s = strtol((argv[3]), &pAux2, 10);
			if (*pAux2 != '\0' || (result.s > INT_MAX)) result.ecode = EHELP;
			else result.ecode = EOK;
			// sila odpalu
			result.power = strtoul((argv[5]), &pAux3, 10);
			if (*pAux3 != '\0' || (result.power > UINT_MAX)) result.ecode = EHELP;
			else result.ecode = EOK;
			result.filename = argv[6];
		}
	}
	else
	{
		result.ecode = EHELP;
	}

	return result;
}

//struktura - polozky matice
typedef struct tmatrix
{
	int rows;		/**< Pocet radku */
	int columns;	/**< Pocet sloupcu */
	int **matrix;	/**< Matice */
} TMatrix;

/**
 * Funkce naalokuje matici na hromade
 * @param mat matice ktera se bude alokovat
 * @return vraci true, kdyz vsechno probehne v poradku, pri chybe vraci false
 */
bool allocMatrix(TMatrix * mat)
{
	mat->matrix = (int**) malloc(mat->rows * sizeof(int *));
	if (mat->matrix == NULL) return false;

	for (int i = 0; i < mat->rows; i++)
	{
		mat->matrix[i] = (int*) malloc(mat->columns * sizeof(int));
		if (mat->matrix[i] == NULL)
		{
			for (; i >= 0; i--) free (mat->matrix[i]);
			free (mat->matrix);
			return false;
		}
	}
	return true;
}

/**
 * Funkce uvolni naalokovnou matici z pameti
 * @param mat TMatrix matice ktera se bude uvolnovat
 */
void freeMatrix(TMatrix * mat)
{
	for (int i = 0; i < mat->rows; i++) free (mat->matrix[i]);
	free (mat->matrix);
}

/**
 * Funkce otevre soubor a nacte z neho vstupni data
 * @param mat matice ktera se bude nacitat
 * @param filename jmeno souboru
 * @return vraci OK, kdyz probehlo vsechno v poradku, jinak vraci kod chyby
 */
int loadMatrix(TMatrix * mat, char * filename)
{
	FILE *myfile;
	myfile = fopen(filename, "r");
	if (myfile == NULL) return EFILE;

	//nacte rozmery
	if (fscanf(myfile, "%d %d", &mat->rows, &mat->columns) != 2)
	{
		if (fclose(myfile) == EOF) return EFILE;
		return EMAT;
	}
	if(mat->rows == 0 || mat->columns == 0) return EMAT;
	if (allocMatrix(mat) == false)
	{
		if (fclose(myfile) == EOF) return EFILE;
		return EALLOC;
	}
	//nacte matici
	for (int i = 0; i < mat->rows; i++)
	{
		for (int j = 0; j < mat->columns; j++)
		{
			int a = fscanf(myfile, "%d", &mat->matrix[i][j]);
			if (a != 1)
			{
				if (fclose(myfile) == EOF) return EFILE;
				freeMatrix(mat);
				return EMAT;
			}
		}
	}
	if (fscanf(myfile, "%*d") != EOF) return EFILE;

	if (fclose(myfile) == EOF)
	{
		freeMatrix(mat);
		return EFILE;
	}
	return EOK;
}

/**
 * Funkce vytiskne zadanou matici
 * @param mat matice ktera se bude tisknout
 */
void printMatrix(TMatrix mat)
{
//rozmery
	printf("%d %d\n", mat.rows, mat.columns);

//tisk matice samotne
	for(int i = 0; i < mat.rows; i++)
	{
		for(int j = 0; j < mat.columns; j++)
		{
			printf("%d", mat.matrix[i][j]);
			if (j == mat.columns-1) printf ("\n");
			else printf(" ");
		}
	}
}

/**
 * Funkce udela transpozici zadane matice
 * @param mat1 matice, ktera bude transponovana
 * @param mat2 vysledna matice
 * @return vraci OK, kdyz probehlo vsechno v poradku, jinak vraci kod chyby
 */
int transMatrix(TMatrix * mat1, TMatrix * mat2)
{
	mat2->rows = mat1->columns;
	mat2->columns = mat1->rows;
	if (allocMatrix(mat2) == (false || NULL)) return EALLOC;
	
	for (int i = 0; i < mat1->rows; i++)
	{
		for (int j = 0; j < mat1->columns; j++)
			mat2->matrix[j][i] = mat1->matrix[i][j];
	}

	return EOK;
}

/**
 * Funkce secte dve zadane matice a jejich vysledek ulozi do treti
 * @param mat1 vstupni matice - scitanec
 * @param mat2 vstupni matice - scitanec
 * @param mat3 vysledna matice - soucet
 * @return vraci OK, kdyz probehlo vsechno v poradku, jinak vraci kod chyby,
 * popr. EFALSE pri nelegalni operaci
 */
int addMatrix(TMatrix * mat1, TMatrix * mat2, TMatrix * mat3)
{
	if ((mat1->columns != mat2->columns) || (mat1->columns != mat2->columns))
		return EFALSE;
	mat3->rows = mat1->rows;
	mat3->columns = mat1->columns;

	if (allocMatrix(mat3) == (false || NULL)) return EALLOC;

	for (int i = 0; i < mat1->rows; i++)
	{
		for (int j = 0; j < mat1->columns; j++)
			mat3->matrix[i][j] = mat1->matrix[i][j] + mat2->matrix[i][j];
	}

	return EOK;
}

/**
 * Funkce nasobi dve matice a vysledek ulozi do treti
 * @param mat1 vstupni matice - cinitel
 * @param mat2 vstupni matice - cinitel
 * @param mat3 vysledna matice - soucin
 * @return vraci OK, kdyz probehlo vsechno v poradku, jinak vraci kod chyby,
 * popr. EFALSE pri nelegalni operaci
 */
int multMatrix(TMatrix * mat1, TMatrix * mat2, TMatrix * mat3)
{
	if (mat1->columns != mat2->rows)
	{
		mat3->rows = 0;
		mat3->columns = 0;
		return EFALSE;
	}

	mat3->rows = mat1->rows;
	mat3->columns = mat2->columns;

	if (allocMatrix(mat3) == (false || NULL)) return EALLOC;

	for (int i = 0; i < mat1->rows; i++)
	{
		for(int j = 0; j < mat2->columns; j++)
		{
			mat3->matrix[i][j] = 0;
			for (int k = 0; k < mat1->columns; k++)
			{
				mat3->matrix[i][j] += mat1->matrix[i][k] * mat2->matrix[k][j];
			}
		}
	}

	return EOK;
}

/**
 * Funkce provede maticovy vyraz A*B(T) + A
 * @param mat1 vstupni matice A
 * @param mat2 vstupni matice B
 * @param mat3 vysledek transpozice
 * @param mat4 vysledek nasobeni
 * @param mat5 vysledna matice
 * @return vraci navratovou hodnotu volanych fci nebo OK, kdyz vse
 * probehne v poradku
 */
int exprMatrix(TMatrix * mat1, TMatrix * mat2, TMatrix * mat3, TMatrix * mat4, TMatrix *mat5)
{
	int aux;
	if ((aux = transMatrix(mat2, mat3)) != EOK)
	{
		freeMatrix(mat3);
		return aux;
	}
	if ((aux = multMatrix(mat1, mat3, mat4)) != EOK) 
	{
		freeMatrix(mat3);
		return aux;
	}
	if ((aux = addMatrix(mat1, mat4, mat5)) != EOK)
	{
		freeMatrix(mat4);
		freeMatrix(mat3);
		return aux;
	}
	return EOK;
}

/**
 * Funkce pripravi pomocnou matici pro funkci vodniho toku - naalokuje matici
 * o stejne velikosti jako vstupni matice a naplni ji jednickami
 * @param mat1 vstupni matice
 * @param mat2 pomocna matice
 * @return vraci OK, kdyz probehlo vsechno v poradku, jinak vraci kod chyby
 */
int prepareWater (TMatrix mat1, TMatrix * mat2)
{
	mat2->rows = mat1.rows;
	mat2->columns = mat1.columns;
	if (allocMatrix(mat2) == (false || NULL)) return EMAT;
	for (int a = 0; a < mat2->rows; a++)
	{
		for (int b = 0; b < mat2->columns; b++)
		mat2->matrix[a][b] = 1;
	}
	return EOK;
}
/**
 * Funkce provadi simulaci vodniho toku, najde nejnizsi body a rozlije do nich 0
 * @param mat1 vstupni matice
 * @param mat2 pomocna matice na 1 a 0
 * @param i vstupni souradnice
 * @param j vstupni souradnice
 * @return vraci OK, kdyz vse probehlo v poradku, pri zadanem prvku mimo
 * matici vraci FALSE
 */
int water(TMatrix *mat1, TMatrix *mat2, int i, int j)
{
	if (i < 0 || j < 0 || i > mat1->rows-1 || j > mat1->columns-1)
		return EOVER;

	int x = mat1->matrix[i][j];
	// pole, kam ukladame polozky matice
	int arr[6] = {0, 0, 0, 0, 0, 0};
	// pomocne pole pro detekci 1/0 v pomocne matici
	bool aux[6] = {false, false, false, false, false, false};
	// dalsi pomocne pole (kvuli "zbytkum" z predchozich volani v arr[])
	bool auxx[6];
	// pokud muzeme, prohledavame okoli
	// doleva
	if (j > 0)
	{
		arr[0] = mat1->matrix[i][j-1];
		if (mat2->matrix[i][j-1] == 1) aux[0] = true;
		else aux[0] = false;
		auxx[0] = true;
	}
	else auxx[0] = false;
	// doprava
	if(j< mat1->columns-1)
	{
		arr[1] = mat1->matrix[i][j+1];
		if (mat2->matrix[i][j+1] == 1) aux[1] = true;
		else aux[1] = false;
		auxx[1] = true;
	}
	else auxx[1] = false;
	// sude radky
	if(EVEN)
	{	// nahoru doprava
		if(i > 0)
		{
			arr[2] = mat1->matrix[i-1][j];
			if (mat2->matrix[i-1][j] == 1) aux[2] = true;
			else aux[2] = false;
			auxx[2] = true;
		}
		else auxx[2] = false;
		// dolu doprava
		if(i < mat1->rows-1)
		{
			arr[3] = mat1->matrix[i+1][j];
			if (mat2->matrix[i+1][j] == 1) aux[3] = true;
			else aux[3] = false;
			auxx[3] = true;
		}
		else auxx[3] = false;
		// nahoru doleva
		if(i > 0 && j > 0)
		{
			arr[4] = mat1->matrix[i-1][j-1];
			if (mat2->matrix[i-1][j-1] == 1) aux[4] = true;
			else aux[4] = false;
			auxx[4] = true;
		}
		else auxx[4] = false;
		// dolu doleva
		if(i < mat1->rows-1 && j > 0)
		{
			arr[5] = mat1->matrix[i+1][j-1];
			if (mat2->matrix[i+1][j-1] == 1) aux[5] = true;
			else aux[5] = false;
			auxx[5] = true;
		}
		else auxx[5] = false;
	}
	// liche radky
	else
	{	// nahoru doprava
		if(i > 0 && j < mat1->columns-1)
		{
			arr[2] = mat1->matrix[i-1][j+1];
			if (mat2->matrix[i-1][j+1] == 1) aux[2] = true;
			else aux[2] = false;
			auxx[2] = true;
		}
		else auxx[2] = false;
		// dolu doprava
		if(i < mat1->rows-1 && j < mat1->columns-1)
		{
			arr[3] = mat1->matrix[i+1][j+1];
			if (mat2->matrix[i+1][j+1] == 1) aux[3] = true;
			else aux[3] = false;
			auxx[3] = true;
		}
		else auxx[3] = false;
		// nahoru doleva
		if(i > 0)
		{
			arr[4] = mat1->matrix[i-1][j];
			if (mat2->matrix[i-1][j] == 1) aux[4] = true;
			else aux[4] = false;
			auxx[4] = true;
		}
		else auxx[4] = false;
		// dolu doleva
		if(i < mat1->rows-1)
		{
			arr[5] = mat1->matrix[i+1][j];
			if (mat2->matrix[i+1][j] == 1)aux[5] = true;
			else aux[5] = false;
			auxx[5] = true;
		}
		else auxx[5] = false;
	}

	for (int h = 0; h < 6; h++)
	{
		// hledame nejmensi hodnotu
		if (arr[h] <= x && auxx[h] == true) x = arr[h];
	}
	mat2->matrix[i][j] = 0;

// volame rekurzivne funkci
	if(aux[0] == true && x == arr[0]) water(mat1, mat2, i, j-1);
	if(aux[1] == true && x == arr[1]) water(mat1, mat2, i, j+1);
 
	if (EVEN)
	{
		if(aux[2] == true && x == arr[2]) water(mat1, mat2, i-1, j);
		if(aux[3] == true && x == arr[3]) water(mat1, mat2, i+1, j);
		if(aux[4] == true && x == arr[4]) water(mat1, mat2, i-1, j-1);
		if(aux[5] == true && x == arr[5]) water(mat1, mat2, i+1, j-1);
	}
	else
	{
		if(aux[2] == true && x == arr[2]) water(mat1, mat2, i-1, j+1);
		if(aux[3] == true && x == arr[3]) water(mat1, mat2, i+1, j+1);
		if(aux[4] == true && x == arr[4]) water(mat1, mat2, i-1, j);
		if(aux[5] == true && x == arr[5]) water(mat1, mat2, i+1, j);
	}
	
	return EOK;
}

/**
 * Funkce provadi simulaci kulecniku, koule prochazi zadanym smerem zadany
 * pocet poli a popr. se odrazi od sten
 * @param mat vstupni matice
 * @param i vstupni souradnice
 * @param j vstupni souradnice
 * @param N sila uderu (pocet poli)
 * @param *dir smer uderu
 * @return vraci OK, kdyz vse probehlo v poradku, pokud je zadany prvek
 * mimo matici, vraci FALSE
 */
int carom(TMatrix * mat, int i, int j, unsigned int N, int dir, int *a)
{

//	  0  1
//	5      2
//	  4  3
	if (i > mat->rows-1 || i < 0 || j > mat->columns-1 || j < 0)
		return EOVER;

	unsigned int x = 0;
	for (x = 0; x < N; x++)
	{
		// neni prvni krok, zadany prvek jiz mame ve vektoru prirazen
		if (x != 0)
		{
/* Posouvani koule*/
			//doprava
			if(dir == V)
			{
				if (j < mat->columns-1)
				{
					a[x] = mat->matrix[i][j+1];
					j++;
				}
				else a[x] = mat->matrix[i][j];
			}
			//doleva
			else if(dir == Z)
			{
				if (j > 0)
				{
					a[x] = mat->matrix[i][j-1];
					j--;
				}
				else a[x] = mat->matrix[i][j];
			}
			// nahoru doprava
			else if(dir == SV)
			{
				if(EVEN && i > 0)
				{
					a[x] = mat->matrix[i-1][j];
					i--;
				}
				else if(ODD && i > 0 && j < mat->columns-1)
				{
					a[x] = mat->matrix[i-1][j+1];
					i--;
					j++;
				}
				else a[x] = mat->matrix[i][j];
			}
			// nahoru doleva
			else if(dir == SZ)
			{
				if (EVEN && i > 0 && j > 0)
				{
					a[x] = mat->matrix[i-1][j-1];
					i--;
					j--;
				}
				else if(ODD && i > 0)
				{
					a[x] = mat->matrix[i-1][j];
					i--;
				}
				else a[x] = mat->matrix[i][j];
			}
			// dolu doprava
			else if(dir == JV)
			{
				if (EVEN && i < mat->rows-1)
				{
					a[x] = mat->matrix[i+1][j];
					i++;
				}
				else if(ODD && i < mat->rows-1 && j < mat->columns-1)
				{
					a[x] = mat->matrix[i+1][j+1];
					i++;
					j++;
				}
				else a[x] = mat->matrix[i][j];
			}
			// dolu doleva
			else if(dir == JZ)
			{
				if (EVEN && i < mat->rows-1 && j > 0)
				{
					a[x] = mat->matrix[i+1][j-1];
					i++;
					j--;
				}
				else if(ODD && i < mat->rows-1)
				{
					a[x] = mat->matrix[i+1][j];
					i++;
				}
				else a[x] = mat->matrix[i][j];
			}
/* Zmena smeru */
		//rohy
			//levy horni
			if (i == 0 && j == 0)
			{
				if (dir == SZ) dir = JV;
				else if (dir == Z) dir = V;
			}
			// pravy horni
			else if (i == 0 && j == mat->columns-1)
			{
				if (dir == V) dir = Z;
				else if (dir == SV) dir = JV;
				else if (dir == SZ) dir = JZ;
			}
			// levy dolni
			else if (i == mat->rows-1 && j == 0)
			{
				if (dir == Z) dir = V;
				if (ODD)
				{
					if (dir == JZ) dir = SZ;
					else if(dir == JV) dir = SV;
				}
				else
					if (dir == JZ) dir = SV;
			}
			// pravy dolni
			else if (i == mat->rows-1 && j == mat->columns-1)
			{
				if (dir == V) dir = Z;
				if (ODD)
				{
					if (dir == JV) dir = SZ;
				}
				else
				{
					if (dir == JV) dir = SV;
					else if (dir == JZ) dir = SZ;
				}
			}
		// steny
			else
			{	// horni okraj
				if (i == 0)
				{
					if (dir == SV) dir = JV;
					else if (dir == SZ) dir = JZ;
				}
				// dolni okraj
				if (i == mat->rows-1)
				{
					if (dir == JV) dir = SV;
					else if (dir == JZ) dir = SZ;
				}
				// levy okraj
				if (j == 0)
				{
					if (dir == Z) dir = V;
					else if (dir == V) dir = Z;
					if (EVEN)
					{
						if (dir == JZ) dir = JV;
						else if (dir == SZ) dir = SV;
					}
				}
				// pravy okraj
				if (j == mat->columns-1)
				{
					if (dir == Z) dir = V;
					else if (dir == V) dir = Z;
					if (ODD)
					{
						if (dir == SV) dir = SZ;
						else if (dir == JV) dir = JZ;
					}
				}
			}
		}
		else a[x] = mat->matrix[i][j];
	}
	return EOK;
}

////////////
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
///Volani fce
  TMatrix mat1 = {.rows = 0, .columns = 0, .matrix = NULL};
  TMatrix mat2 = {.rows = 0, .columns = 0, .matrix = NULL};
  TMatrix mat3 = {.rows = 0, .columns = 0, .matrix = NULL};
  TMatrix mat4 = {.rows = 0, .columns = 0, .matrix = NULL};
  TMatrix mat5 = {.rows = 0, .columns = 0, .matrix = NULL};

	int ecode = EOK;
	int *array;

	if (params.state == CHELP) printf("%s", HELPMSG);
	else
	{
		if ((ecode = loadMatrix(&mat1, params.filename)) == EOK)
		{
			switch (params.state)
			{
				case CTEST:
				  printMatrix(mat1);
				  freeMatrix(&mat1);
				  break;
				case CADD:
				  if ((ecode = loadMatrix(&mat2, params.filename2)) == EOK)
				  {
				  	if ((ecode = addMatrix(&mat1, &mat2, &mat3)) == EOK)
				  	{
						printMatrix(mat3);
						freeMatrix(&mat3);
					}
				  	freeMatrix(&mat2);
				  }
				  freeMatrix(&mat1);
				  break;
				case CMULT:
				  if ((ecode = loadMatrix(&mat2, params.filename2)) == EOK)
				  {
				  	if ((ecode = multMatrix(&mat1, &mat2, &mat3)) == EOK)
				  	{
						printMatrix(mat3);
						freeMatrix(&mat3);
					}
					freeMatrix(&mat2);
				  }
				  freeMatrix(&mat1);
				  break;
				case CTRANS:
				  if ((ecode = transMatrix(&mat1, &mat2)) == EOK)
				  {
					printMatrix(mat2);
					freeMatrix(&mat2);
				  }
				  freeMatrix(&mat1);
				  break;
				case CEXPR:
				  if ((ecode = loadMatrix(&mat2, params.filename2)) == EOK)
				  {
				  	if ((ecode = exprMatrix(&mat1, &mat2, &mat3, &mat4, &mat5)) == EOK)
					{
						printMatrix(mat5);
						freeMatrix(&mat5);
						freeMatrix(&mat4);
						freeMatrix(&mat3);
				  	}
				  	freeMatrix(&mat2);
				  }
				  freeMatrix(&mat1);
				  break;
				case CWATER:
				  if ((ecode = prepareWater(mat1, &mat2)) == EOK)
				  {
				  	if((ecode = water(&mat1, &mat2, params.r, params.s)) == EOK)
						printMatrix(mat2);
				  	freeMatrix(&mat2);
				  }
				  freeMatrix(&mat1);
				  break;
				case CCAROM:
				  array = malloc(params.power *sizeof(int));
				  if ((ecode = carom(&mat1, params.r, params.s, params.power, params.direc, array)) == EOK)
				  {
					for (unsigned int bla = 0; bla < params.power; bla++)
					{
						printf("%d", array[bla]);
						if (bla == params.power-1) printf("\n");
						else printf(" ");
					}
				  }
				  free(array);
				  freeMatrix(&mat1);
				  break;
			}
		}
	}

	if (ecode != EOK && ecode != EFALSE)
	{
		printECode(ecode);
		return EXIT_FAILURE;
	}
	else if (ecode == EFALSE) printf(FALSE);

	return EXIT_SUCCESS;
}
