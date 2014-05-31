/* ******************************** ial.c *********************************** */
/* Soubor:              ial.c - Funkce pro predmet algotirmy                  */
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
/* ************************************************************************** */

#include "ial.h"

/**
* Funkce naalokuje zadany retezec a ulozi do struktury
* @param *attr struktura s retezcem
* @param *sort struktura s polem
*/
int init(string *attr, Tsort *sort)
{
	sort->delka = ((attr->length)-1);
	sort->Pole = malloc(((attr->length+1)*sizeof(char)));
	if ((sort->Pole) == NULL)
		return ProgErr;
	for (int i = 0; i <= sort->delka; i++)
		sort->Pole[i] = attr->str[i];
	return 0;
}


/**
* Funkce provadi znovuustaveni haldy
* @param *Heap struktura s polem
* @param Left ukazuje na prvek v poli
* @param Right ukazuje na jiny prvek v poli
*/
// fce provadi znovuustaveni haldy
void siftDown(Tsort *Heap, int Left, int Right)
{
	int i,j,Temp;
	bool Cont;
	i = Left;
	j = 2 * i;
	Temp = Heap->Pole[i]; // docasne ulozeni obsahu
	Cont = j <= Right; // jestli nejsme na konci=true
	while(Cont)
	{
		if (j < Right)
			// pokud je predchozi prvek mensi nez nasledujici prejdem na nasledujici
			if (Heap->Pole[j] < Heap->Pole[j+1])
				j = j + 1;
		// hodnota v ulozene promene je vetsi - konec cyklu
		if (Temp >= Heap->Pole[j])
			Cont = false;
			else
			{  // do puvodniho obsahu ulozime novy // puvodni ulozen v temp
				Heap->Pole[i] = Heap->Pole[j];
				i = j;
				j = 2*i;
				Cont = j <= Right;
			}
	}
	// ulozeni puvodni promene
	Heap->Pole[i] = Temp;

}


/**
* Funkce se samotnym radicim algoritmem, seradi znaky v retezci podle
* ordinalni hodnoty
* @param *Heap struktura s polem
*/
void heapS(Tsort *Heap)
{
	int i, Left, Right;
	int pom;

	// ustaveni hromady
	Left = Heap->delka / 2;
	Right = Heap->delka;

	for (i = Left; i >= 0; i--)
	{
		siftDown(Heap,i,Right);
	}
	// HEAP sort

	for (Right = Heap->delka; Right >= 1; Right--)
	{
		pom = Heap->Pole[0];  // vymena korene s aktualnim prvkem
		Heap->Pole[0] = Heap->Pole[Right];
		Heap->Pole[Right] = pom;
		siftDown(Heap,0,Right-1);	// znovu ustaveni hromady
	}
}

/**
* Funkce rozkouskuje retezec do pole a zpetne zmeni ukazatel
* retezce
* @param mat1 *attr struktura s retezcem
*/
int heapSort(string *attr)
{
	char *clear;
	clear = attr->str;

	Tsort sort =
	{
		.delka = 0,
		.Pole = NULL
	};

	// prevedeni retezce na pole
	if (init(attr,&sort) == ProgErr)
		return ProgErr;
	// serazeni
	heapS(&sort);
	// ukonceni retezce
	sort.Pole[sort.delka+1] = '\0';
	// prepis ukazatele
	attr->str = sort.Pole;

	// uvolneni puvodniho
	free(clear);
	return 0;
}

/**
* Samotny vyhledavaci algoritmus
* @param *str struktura s retezcem
* @param *substr struktura s podretezcem
* @param.*succ vraci pripadnou chybu alokace
* @return vraci pozici nalezeneho podretezce, pokud podretezec nenajde,
*
*/
int kmpFind(Tsort *str, Tsort *substr, int* succ)
{
	int i = 0; // index retezce
	int j = 0; // index podretezce
	int *aux, k, l; // pomocny vektor a indexy

	aux = malloc((str->delka+substr->delka)*sizeof(int));
	if (aux == NULL)
		{
			*succ = ProgErr;
			return 0;
		}

	aux[0] = -1;

	//vytvorime si pomocny vektor, ktery nam urci,
	//na jakem dalsim miste je mozno zacit vyhledavat
	for (k = 1; k <= substr->delka; k++)
	{
		l = aux[k-1];
		while (l > 0 && substr->Pole[l] != substr->Pole[k-1])
			l = aux[l];
		aux[k] = l + 1;
	}

	// KMP algoritmus
	while (i <= str->delka && j <= substr->delka)
	{
		if (j == -1 || (str->Pole[i]) == (substr->Pole[j]))
		{
			i++;
			j++;
		}
		else
			j = aux[j];

	}
	free (aux);
	if (j > substr->delka)
		return (i - substr->delka); // nasel vzorek

	else
		// 2 pricitame jelikoz cislovani retezce je od jednicky
		return (str->delka + 2 ); // nenasel nic

}

/**
* Funkce zpracuje vstupni retezec a podretezec a zavola funkci
* s vyhledavacim algoritmem
* @param *text zadany retezec, ve kterem mame hledat
* @param *pattern vzorek, ktery mame vyhledat v retezci
* @param *succ vraci pozici
*/
int kmp(string *text, string *pattern, double *succ)
{
	int pom,pom2;
	pom2=0;
	Tsort retezec =
	{
		.delka = 0,
		.Pole = NULL
	};

	// volame funkci na inicializaci retezce
	if (init(text,&retezec) == ProgErr)
		return ProgErr;

	Tsort podretezec =
	{
		.delka = 0,
		.Pole = NULL
	};

	// volame funkci na inicializaci podretezce
	if (init(pattern,&podretezec) == ProgErr)
		{
			free(retezec.Pole);
			return ProgErr;
		}

	//volame vyhledavaci algoritmus
	if ((pom=kmpFind(&retezec,&podretezec,&pom2)) <= ((retezec.delka)+1))
		*succ = pom;
	else
		*succ = -1; // ukonceni retezce

	// uvolneni naalokovane pameti
	free(retezec.Pole);
	free(podretezec.Pole);

	if (pom2!=0)
		return pom2;
	return 0;
}


/* Modul implementujuci binarny vyhladavaci strom */


char reservedWord[23][9] = {"do",    "while",
                            "if",    "false",
                            "then",  "read",
                            "else",  "write",
                            "local", "function",
                            "true",  "return",
                            "end",   "and",
                            "nil",   "break",
                            "in",    "elseif",
                            "not",   "for",
                            "or",    "repeat",
                            "until"
                                                };


/** Funkcie operujuce nad oboma stromami */

bool TreeInit (tFuncTreeItem **RootPtr)
{ /* Funkcia vykona prvotnu inicializaciu */
	(*RootPtr)= NULL;
	if (!insertBuiltIn(RootPtr)) {
		AddError(line, ProgErr);
		return false;
	}
	return true;
}

/** Funkcie operujuce nad stromom premennych */

int VarTreeSearch (tVarTreeItem *RootPtr, string *identif)
{ /* Funkcia prehladava binarny strom a v pripade najdenia polozky s identifikatorom identif vracia 1 */

if(RootPtr != NULL)
  {
  /* podla kluca volim dalsiu cestu */
  int smer;

  smer = strCmpString(identif, RootPtr->key);

  if(smer > 0) return VarTreeSearch(RootPtr->RightRoot, identif);     /* Kluc ma vyssiu ordinalnu hodnotu posun v pravo */
  if(smer < 0) return VarTreeSearch(RootPtr->LeftRoot, identif);     /* Kluc ma nizsiu ordinalnu hodnotu posun v lavo*/
  if(smer == 0) return TREE_EXIST;                   /* Zadany identifikator uz v tabulke je oznamujem to vyssie */
  }

return TREE_FAIL;
}

int VarTreeInsert(tVarTreeItem **RootPtr, string *identif, tVarValue *newVar)
{ /* Funkcia  vlozi do stromu premennych novu polozku v pripade ze zadana polozka uz existuje prepise jej obsah */

if((*RootPtr) != NULL)
  {
  /* podla kluca volim dalsiu cestu */
  int smer;

  smer = strCmpString(identif, (*RootPtr)->key);

  if(smer > 0) return VarTreeInsert(&((*RootPtr)->RightRoot), identif, newVar);     /* Kluc ma vyssiu ordinalnu hodnotu posun v pravo */
  if(smer < 0) return VarTreeInsert(&((*RootPtr)->LeftRoot), identif, newVar);     /* Kluc ma nizsiu ordinalnu hodnotu posun v lavo*/
  if(smer == 0) /* Polozka s danym nazvom uz existuje */
    {
    if(newVar != NULL) /* Pokial sa vklada nejaka hodnota */
      {
       if((*RootPtr)->data == NULL) /* Pokial este dana premenna nebola inicializovana */
        {
        if(((*RootPtr)->data = malloc(sizeof(tVarValue))) == NULL)
          {
          AddError(0 , ProgErr);
          return TREE_ERROR;
          } /* Vytvorim novu polozku pre hodnotu */
        }
      (*RootPtr)->data->type = newVar->type;
      switch(newVar->type)
        {
        case T_NIL: break;

        case T_INTEGER: (*RootPtr)->data->real = newVar->real;
                        break;

        case T_REAL: (*RootPtr)->data->real = newVar->real;
                     break;

        case T_STRING: if((*RootPtr)->data->stringVar == NULL)
                         {
                         if(((*RootPtr)->data->stringVar=malloc(sizeof(string))) == NULL )
                           {
                           AddError(0 , ProgErr);
                           return TREE_ERROR;
                           }
                         if(strInit(((*RootPtr)->data->stringVar)) == 1)
                           {
                           AddError(0 , ProgErr);
                           return TREE_ERROR;
                           }
                         }

                      if(newVar->stringVar == NULL)
                         {
                         strClear((*RootPtr)->data->stringVar);
                         }
                      else strCopyString((*RootPtr)->data->stringVar , newVar->stringVar);
                      break;

        case T_BOOLEAN: (*RootPtr)->data->boolean = newVar->boolean;
                      break;

        default:      break;
        }
      return TREE_SUCCESS;
      }
    }
  }
else /* Dosiel som na koniec stromu zadany uzol neexistuje */
  {
  (*RootPtr) = malloc(sizeof(tVarTreeItem)); /* Vytvorim novy list */
  if((*RootPtr) == NULL)
     {
     AddError(0 , ProgErr);
     return TREE_ERROR;
     }

  (*RootPtr)->key = malloc(sizeof(string));  /* Vytvorim a skopirujem kluc */
  if((*RootPtr)->key == NULL)
    {
    AddError(0 , ProgErr);
    return TREE_ERROR;
    }
  if(strInit(((*RootPtr)->key)) == 1)
    {
    AddError(0 , ProgErr);
    return TREE_ERROR;
    }
  strCopyString((*RootPtr)->key , identif);

  if(((*RootPtr)->data = malloc(sizeof(tVarValue))) == NULL)
    {
    AddError(0 , ProgErr);
    return TREE_ERROR;
    } /* Vytvorim novu polozku pre hodnotu */
	(*RootPtr)->data->stringVar = NULL;
	(*RootPtr)->data->type = T_NIL;

  if(newVar != NULL) /* Pokial sa vklada nejaka hodnota */
      {
      (*RootPtr)->data->type = newVar->type;
      switch(newVar->type)
        {
        case T_NIL: break;

        case T_INTEGER: (*RootPtr)->data->real = newVar->real;
                        break;

        case T_REAL: (*RootPtr)->data->real = newVar->real;
                     break;

        case T_STRING: if(((*RootPtr)->data->stringVar = malloc(sizeof(string))) == NULL )
                          {
                          AddError(0 , ProgErr);
                          return TREE_ERROR;
                          }
                       if(strInit(((*RootPtr)->data->stringVar)) == 1)
                          {
                          AddError(0 , ProgErr);
                          return TREE_ERROR;
                          }
                       if(newVar->stringVar == NULL)
                         {
                         strClear((*RootPtr)->data->stringVar);
                         }
                      else strCopyString((*RootPtr)->data->stringVar , newVar->stringVar);
                      break;

        case T_BOOLEAN: (*RootPtr)->data->boolean = newVar->boolean;
                      break;

        default:      break;
        }
      }

  (*RootPtr)->LeftRoot = NULL;
  (*RootPtr)->RightRoot = NULL;
  return TREE_SUCCESS;
  }
return TREE_FAIL;
}

void DisposeVarTree(struct VarTreeItem **mainVarRoot)
{
if(*mainVarRoot != NULL) /* Narazil som na list? */
  {
  DisposeVarTree(&((*mainVarRoot)->LeftRoot)); /* Rekurzivne volanie funkcie */
  DisposeVarTree(&((*mainVarRoot)->RightRoot));

  free((*mainVarRoot)->key->str); /* Uvolnujem retazec v strukture string */
  free((*mainVarRoot)->key); /* Uvolnujem strukturu string */
  if((*mainVarRoot)->data != NULL)
     {
     if((*mainVarRoot)->data->stringVar != NULL)
       {
       free((*mainVarRoot)->data->stringVar->str); /* Uvolnujem retazec */
       free((*mainVarRoot)->data->stringVar);
       }
     free((*mainVarRoot)->data);
     }
  free((*mainVarRoot)); /* Uvolnujem polozku stromu */
  (*mainVarRoot) = NULL; /* Nulujem pointer (nastavujem ho ako prazdny aby ho dalsie funkcie nepouzivali */
  }
}




int getVarTree(tVarTreeItem ***found, tFuncTreeItem *RootPtr, string *identif)
{
if(RootPtr != NULL)
  {
  /* podla kluca volim dalsiu cestu */
  int smer;

  smer = strCmpString(identif, RootPtr->key);

  if(smer > 0) return getVarTree(found,RootPtr->RightRoot, identif);     /* Kluc ma vyssiu ordinalnu hodnotu posun v pravo */
  if(smer < 0) return getVarTree(found,RootPtr->LeftRoot, identif);     /* Kluc ma nizsiu ordinalnu hodnotu posun v lavo*/
  if(smer == 0)
    {
    (*found) = &(RootPtr->rootVarTree);
    return TREE_SUCCESS;
    }
  }

return TREE_FAIL;
}


int GetVarPointer(tVarTreeItem **found, tVarTreeItem *RootPtr, string *identif)
{ /* Funkcia prehladava binarny strom a v pripade najdenia polozky s identifikatorom vracia ukazatel na nu*/

if(RootPtr != NULL)
  {
  /* podla kluca volim dalsiu cestu */
  int smer;

  smer = strCmpString(identif, RootPtr->key);

  if(smer > 0) return GetVarPointer(found, RootPtr->RightRoot, identif);     /* Kluc ma vyssiu ordinalnu hodnotu posun v pravo */
  if(smer < 0) return GetVarPointer(found, RootPtr->LeftRoot, identif);     /* Kluc ma nizsiu ordinalnu hodnotu posun v lavo*/
  if(smer == 0)
    {/* Nasiel som danu polozku */
    (*found)= (RootPtr);
    return TREE_SUCCESS;
    }
  return TREE_FAIL;
  }

return TREE_FAIL;
}

/** Funkcie operujuce nad stromom  funkcii */

int FuncTreeSearch (tFuncTreeItem *RootPtr, string *identif)
{ /* Funkcia prehladava binarny strom a v pripade najdenia polozky s identifikatorom identif vracia 1 */

if(RootPtr != NULL)
  {
  /* podla kluca volim dalsiu cestu */
  int smer;

  smer = strCmpString(identif, RootPtr->key);

  if(smer > 0) return FuncTreeSearch(RootPtr->RightRoot, identif);     /* Kluc ma vyssiu ordinalnu hodnotu posun v pravo */
  if(smer < 0) return FuncTreeSearch(RootPtr->LeftRoot, identif);     /* Kluc ma nizsiu ordinalnu hodnotu posun v lavo*/
  if(smer == 0) return TREE_EXIST;                   /* Zadany identifikator uz v tabulke je oznamujem to vyssie */
  }

return TREE_FAIL;
}


int FuncTreeInsert(tFuncTreeItem **RootPtr,string *identif)
{ /* Vlozi novu polozku do stromu funkcii */

if((*RootPtr) != NULL)
  {
  /* podla kluca volim dalsiu cestu */
  int smer;

  smer = strCmpString(identif, (*RootPtr)->key);

  if(smer > 0) return FuncTreeInsert(&((*RootPtr)->RightRoot), identif);     /* Kluc ma vyssiu ordinalnu hodnotu posun v pravo */
  if(smer < 0) return FuncTreeInsert(&((*RootPtr)->LeftRoot), identif);     /* Kluc ma nizsiu ordinalnu hodnotu posun v lavo*/
  if(smer == 0) return TREE_FAIL;/* Polozka s danym nazvom uz existuje */
  }
else
  {
  if(((*RootPtr)= malloc(sizeof(tFuncTreeItem))) == NULL )
     {
     AddError(0, ProgErr);
     return TREE_ERROR;
     }
  if(((*RootPtr)->key= malloc(sizeof(string))) == NULL)
     {
     AddError(0, ProgErr);
     return TREE_ERROR;
     }
  if(strInit((*RootPtr)->key) == 1)
     {
     AddError(0, ProgErr);
     return TREE_ERROR;
     }
  (*RootPtr)->rootVarTree = NULL;
  (*RootPtr)->LeftRoot = NULL;
  (*RootPtr)->RightRoot = NULL;
  strCopyString((*RootPtr)->key, identif);
  }
return TREE_SUCCESS;  //insert uspesny
}


void DisposeFuncTree(tFuncTreeItem **mainRoot)
{
if((*mainRoot) != NULL)
  {
  DisposeFuncTree(&((*mainRoot)->LeftRoot)); /* Rekurzivne prechadzam strom */
  DisposeFuncTree(&((*mainRoot)->RightRoot));

  DisposeVarTree(&((*mainRoot)->rootVarTree)); /* Uvolnujem strom hodnot */

  free((*mainRoot)->key->str); /* Uvolnujem retazec v strukture string */
  free((*mainRoot)->key); /* Uvolnujem strukturu string */
  free((*mainRoot)); /* Uvolnujem polozku stromu */
  (*mainRoot) = NULL; /* Nulujem pointer (nastavujem ho ako prazdny aby ho dalsie funkcie nepouzivali */
  }
}

int MergeOrigBackup(tVarTreeItem **BackUp, tVarTreeItem **Original)
{ /* Aktualizuje hodnoty v Original na BackUP */
int retValue = TREE_FAIL;

if((*BackUp) != NULL)
  {
  retValue = VarTreeInsert(Original, (*BackUp)->key , (*BackUp)->data);
  if(retValue != TREE_ERROR)
    { /* Skopiruj syny */
    if((*BackUp)->LeftRoot != NULL) return MergeOrigBackup(&((*BackUp)->LeftRoot), Original);
    if((*BackUp)->RightRoot != NULL) return MergeOrigBackup(&((*BackUp)->RightRoot), Original);
    }
  else
    {/* Doslo k vnutornej chybe programu */
    AddError(0, ProgErr);
    return retValue;
    }
  }
return retValue;
}

/**Ostatne funkcie */


int CheckResWord(string *identif)
{ /* Funkcia skontroluje ci sa nahodov identifikator nezhoduje s nejakym rezervovanym slovom */
for(int i=0;i<23; i++)
  {
  if(strcmp(reservedWord[i],identif->str) == 0)
    {
    return TREE_EXIST;
    }
  }
return TREE_FAIL;
}





int CopyTree (tVarTreeItem **Source, tVarTreeItem **BackUp)
{
int retValue = TREE_FAIL;

if((*Source) != NULL)
  {
  retValue = VarTreeInsert(BackUp, (*Source)->key , (*Source)->data);
  if(retValue != TREE_ERROR)
    { /* Skopiruj syny */
    if((*Source)->LeftRoot != NULL) return CopyTree(&((*Source)->LeftRoot), BackUp);
    if((*Source)->RightRoot != NULL) return CopyTree(&((*Source)->RightRoot), BackUp);
    }
  else
    {/* Doslo k vnutornej chybe programu */
    DisposeVarTree(BackUp);
    AddError(0, ProgErr);
    return retValue;
    }
  }
return retValue;
}



void DisposeFirstBackUp(void)
{ /* Funkcia odstrani posledny zaznam z pomocneho zoznamu funkcii */
tFuncTreeItem *p_temp = NULL;

if(STbackUp != NULL)
  {
  p_temp = STbackUp;
  STbackUp=STbackUp->RightRoot;

  free(p_temp->key->str);
  free(p_temp->key);

  DisposeVarTree(&((p_temp)->rootVarTree));
  free((p_temp));
  }



BackUpCounter--;
}


/** Funkcia handlera */

int tableHandler(tFuncTreeItem **mainRoot, tTreeCont *handle)
{ /* Ovlada tabulku symbolov a koriguje operacie nad nou */
int returnValue = TREE_FAIL;


tVarTreeItem *p_variable = NULL;
tVarTreeItem **p_var_help=NULL;


switch(handle->operation)
   {
   case O_CHECK: /* Handler zkontroluje VarTree a FuncTree ci sa dany identifikator niekde nachadza */
                if(CheckResWord(handle->identif) == TREE_EXIST) break; /* Kontrolujem zhodu s rezervovanymi slovami */
                if((returnValue=FuncTreeSearch ((*mainRoot), handle->identif )) == TREE_EXIST) break;
                if((getVarTree(&(p_var_help), (*mainRoot), handle->function )) != TREE_FAIL) /* Ziskavam pointer na strom hodnot funkcie s nazvom handle->function */
                   {
                   if((*p_var_help) != NULL)
                     {
                     if((returnValue = VarTreeSearch((*p_var_help), handle->identif)) == TREE_EXIST) break;  /* Hladam identifikator v strome hodnot danej premennej */
                     }
                   }
                break;

   case O_ADD: /* Prida novu polozku */
              if((handle->variable) == NULL) /* Pridavam funkciu */
                {
                returnValue=FuncTreeInsert(&(*mainRoot), handle->function);
                break;
                }
              else if ((FuncTreeSearch(*mainRoot, handle->variable)) != TREE_EXIST)
               	{
       	        if((getVarTree((&p_var_help), (*mainRoot), handle->function )) != TREE_FAIL) /* Ziskavam pointer na strom hodnot funkcie s nazvom handle->function */
                   {
                   returnValue = VarTreeInsert(p_var_help, handle->variable, handle->varData);
                   }
                }
               else
               	returnValue = TREE_FAIL;
              break;

   case O_IDENTIFY_ID: /* Zisti ci identifikator je FID alebo ci dana funkcia ma VID s menom identif */
                     if((returnValue = FuncTreeSearch ((*mainRoot), handle->identif ) == TREE_EXIST))
                        {
                        returnValue = T_FID;
                        break;
                        }
                     if((getVarTree(&(p_var_help), (*mainRoot), handle->function )) != TREE_FAIL) /* Ziskavam pointer na strom hodnot funkcie s nazvom handle->function */
                       {
                       if((*p_var_help) != NULL)
                         {
                         if((returnValue = VarTreeSearch((*p_var_help), handle->identif)) == TREE_EXIST)
                           {
                           returnValue = T_VID;
                           break;  /* Hladam identifikator v strome hodnot danej premennej */
                           }
                         }
                      returnValue = TREE_FAIL;
                      }
                      break;

   case O_VAR_TYPE: /* Funkcia zisti aky typ ma premenna s danym identifikatorom */
                   if((getVarTree(&(p_var_help), (*mainRoot), handle->function )) != TREE_FAIL) /* Ziskavam pointer na strom hodnot funkcie s nazvom handle->function */
                      {
                      if((*p_var_help) != NULL)
                        {
                        if((GetVarPointer(&(p_variable),(*p_var_help), handle->variable) ) == TREE_SUCCESS) /* Ak som uspesne  nasiel premennu mam pointer na nu */
                          {
                          if((p_variable) != NULL)
                            {
                            if(((p_variable)->data) != NULL) return (p_variable)->data->type;
                            }
                          }
                        }
                     returnValue = TREE_FAIL;
                     }
                   break;

   case O_COPY_TREE: /* Funkcia vytvori kopiu zaznamu zo stromu funkcii (pouzitie pre volanie funkcii) funkcie specifikovane v handle->identif */
                    {
                    tFuncTreeItem *p_temp = NULL;
                    tVarTreeItem **p_var_help = NULL;

                    if(((p_temp)= malloc(sizeof(tFuncTreeItem))) == NULL)
                      {/* Vytvorim novy zasnam */
                      AddError(0, ProgErr);
                      return TREE_ERROR;
                      }

                    if(((p_temp)->key= malloc(sizeof(string))) == NULL)
                      {/* Vytvorim novy zasnam */
                      AddError(0, ProgErr);
                      return TREE_ERROR;
                      }

                    if(strInit((p_temp)->key) == 1)
                      {
                      AddError(0, ProgErr);
                      return TREE_ERROR;
                      }

                    (p_temp)->rootVarTree = NULL;
                    (p_temp)->LeftRoot = NULL;
                    (p_temp)->RightRoot = NULL;

                    if((strAddChar((p_temp)->key, (BackUpCounter+1)) == 1))
                      { /* Identifikator polozky v strome funkcii je pocet zaznamov */
                      AddError(0, ProgErr);
                      return TREE_ERROR;
                      }

                    if((getVarTree(&(p_var_help), (*mainRoot), handle->identif )) == TREE_ERROR)
                      {/* Ziskavam pointer na strom hodnot funkcie s nazvom handle->identif */
                      AddError(0, ProgErr);
                      return TREE_ERROR;
                      }

                    if((CopyTree (p_var_help, &((p_temp)->rootVarTree))) == TREE_ERROR)
                      { /* Vytvorim do p_temp ulozim kopiu stromu hodnot */
                      AddError(0, ProgErr);
                      return TREE_ERROR;
                      }

                    p_temp->RightRoot = STbackUp; /* Spojim novy prvok so zvyskom zoznamu */
                    STbackUp = p_temp;

                    returnValue = TREE_SUCCESS;

                    BackUpCounter++;
                    break;
                    }

   case O_ACTUALIZE: /* Funkcia aktualizuje hodnotu premennej s identif variable podla premennej obsahu varData alebo STitem */
                   {
                   tVarTreeItem **p_var_help = NULL;

                   if((getVarTree(&(p_var_help), (*mainRoot), handle->identif )) == TREE_FAIL)
                     {/* Ziskavam pointer na strom hodnot funkcie s nazvom handle->identif */
                     AddError(0, ProgErr);
                     return TREE_ERROR;
                     }

                   if(handle->STitem != NULL)
                     { /* Parameter funkcie je predavany premennou */

                     if((returnValue = VarTreeInsert(p_var_help, handle->variable, handle->STitem)) == TREE_ERROR)
                        {/* Funkcia aktualizuje hodnotu premennej variable podla obsahu STitem */
                        AddError(0, ProgErr);
                        return TREE_ERROR;
                        }
                     }
                   else
                     { /* Parameter bol predany  priamo (tj. cislom, stringom....) */

                     if((returnValue = VarTreeInsert(p_var_help, handle->variable, handle->varData)) == TREE_ERROR)
                        {/* Funkcia aktualizuje hodnotu premennej variable podla obsahu STitem */
                        AddError(0, ProgErr);
                        return TREE_ERROR;
                        }

                     }
                   }
                   break;


    case O_FETCH_VALUE: /* Do handle STitem ulozi premennu handle->variable ulozenu v strome funkcii handle->function */
                      if((handle->variable) != NULL) /* Pridavam funkciu */
                        {
                        if((getVarTree(&(p_var_help), (*mainRoot), handle->function )) != TREE_FAIL) /* Ziskavam pointer na strom hodnot funkcie s nazvom handle->function */
                          {
                          if((*p_var_help) != NULL)
                            {
                            if((GetVarPointer(&(p_variable),(*p_var_help), handle->variable) ) == TREE_SUCCESS) /* Ak som uspesne  nasiel premennu mam pointer na nu */
                              {
                              if((p_variable) != NULL)
                                {
                                if(((p_variable)->data) != NULL)
                                  {
                                  handle->STitem = p_variable->data;
                                  return TREE_SUCCESS;
                                  }
                                 }
                               }
                             returnValue = TREE_FAIL;
                             }
                            }
                           }
                         break;

    case O_MERGE_ORIG: /* Prekopiruje do stromu hodnot handle->identif hodnoty ulozene v backup */
                     {
                     tVarTreeItem **p_var_origin = NULL;
                     tVarTreeItem **p_var_backup = NULL;

                     p_var_backup = &(STbackUp->rootVarTree);

                     if((getVarTree(&(p_var_origin), (*mainRoot), handle->identif )) == TREE_ERROR)
                       {/* Ziskavam pointer na strom hodnot funkcie s nazvom handle->identif */
                       AddError(0, ProgErr);
                       return TREE_ERROR;
                       }

                     if((MergeOrigBackup(p_var_backup, p_var_origin)) == TREE_ERROR)
                       { /* Vytvorim do p_temp ulozim kopiu stromu hodnot */
                       AddError(0, ProgErr);
                       return TREE_ERROR;
                       }

                     DisposeFirstBackUp(); /* Odstranujem zaznam z backup stromu */

                     }

                     break;
   }

return returnValue;
}

bool insertBuiltIn(tFuncTreeItem **rootPtr) {
	char *funcIdentifs[] = {"substr", "sort", "type", "find"};
	char *varIdentifs[] = {"right", "left", "string", "string", "id",
															"string", "substr"};
	//vytvori staticka pole do kterych konvertuje char * retezce na string
	string *funcs[4] = {NULL};
	string *vars[7] = {NULL};

	//provede konverzi
	for (int i = 0; i < 4; i++) {
		if (!toString(&(funcs[i]), funcIdentifs[i]))
			return false;
	}
	for (int i = 0; i < 7; i++) {
		if (!toString(&(vars[i]), varIdentifs[i]))
			return false;
	}
	//nyni zavola vlozeni identifikatoru vestavenych fci do stromu fci
	for (int i = 0; i < 4; i++) {
			if (FuncTreeInsert(rootPtr, funcs[i]) != TREE_SUCCESS)
				return false;
	}
	//prida do fci jejich argumenty -- prvne ziska jejich strom promennych do
	//pomocne promenne a pak do nej vlozi novou polozku
	tVarTreeItem **found;
	//a ted trocha cerne magie ;)
	for (int i = 0; i < 4; i++) {
		getVarTree(&found, *rootPtr, funcs[i]);
		switch (i) {
			case 0:
				for (int j = i; j < 3; j++)
					VarTreeInsert(found, vars[j], NULL);
				break;
			case 1:
				VarTreeInsert(found, vars[i + 2], NULL);
				break;
			case 2:
				VarTreeInsert(found, vars[i + 2], NULL);
				break;
			case 3:
				for (int j = i + 2; j < 7; j++)
					VarTreeInsert(found, vars[j], NULL);
				break;
		}
	}

	for (int i = 0; i < 4; i++)
		disposeString(&(funcs[i]));
	for (int i = 0; i < 7; i++)
		disposeString(&(vars[i]));

	return true;
}

//funkce vytvori promennou typu string z ceckoveho retzce
//RETURN VALUE: true -- uspech, false -- neuspech
//EXPECTED: string **, char *       [!!! pole znaku musi byt uknoceno '\0' !!!]
bool toString(string ** dest, char * src) {
	bool result = true;
	//alokuje novy string
	if ((*dest = malloc(sizeof(string))) == NULL)
		result = false;
	else if (strInit(*dest)) {
		free(*dest);
		*dest = NULL;
		result = false;
	}
	//pokud se alokace zdarila kopiruje znak po znaku do destination
	if (result) {
		int c = src[0], i = 1;
		while (c != '\0') {
			if (strAddChar(*dest, c)) {
				strFree(*dest);
				free(*dest);
				*dest = NULL;
				result = false;
				break;
			}
			c = src[i];
			i++;
		}
	}
	return result;
}

//funkce uvolni promennou typu string
//RETURN VALUE: void
//EXPECTED: string **
void disposeString(string **str)
{
	//osetreni spatneho volani disposeString
	if (*str != NULL)
        {
		strFree(*str);
		free(*str);
		*str = NULL;
        }
	else
      {
      AddError(line,ProgErr);
      return;
      }

}


