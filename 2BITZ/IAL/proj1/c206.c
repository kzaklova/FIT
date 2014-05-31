	
/* c206.c **********************************************************}
{* Téma: Dvousmìrnì vázaný lineární seznam
**
**                   Návrh a referenèní implementace: Bohuslav Køena, øíjen 2001
**                            Pøepracované do jazyka C: Martin Tuèek, øíjen 2004
**                                            Úpravy: Bohuslav Køena, øíjen 2010
**
** Implementujte abstraktní datový typ dvousmìrnì vázaný lineární seznam.
** U¾iteèným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován promìnnou
** typu tDLList (DL znamená Double-Linked a slou¾í pro odli¹ení
** jmen konstant, typù a funkcí od jmen u jednosmìrnì vázaného lineárního
** seznamu). Definici konstant a typù naleznete v hlavièkovém souboru c206.h.
**
** Va¹ím úkolem je implementovat následující operace, které spolu
** s vý¹e uvedenou datovou èástí abstrakce tvoøí abstraktní datový typ
** obousmìrnì vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu pøed prvním pou¾itím,
**      DLDisposeList ... zru¹ení v¹ech prvkù seznamu,
**      DLInsertFirst ... vlo¾ení prvku na zaèátek seznamu,
**      DLInsertLast .... vlo¾ení prvku na konec seznamu, 
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek, 
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku, 
**      DLDeleteFirst ... zru¹í první prvek seznamu,
**      DLDeleteLast .... zru¹í poslední prvek seznamu, 
**      DLPostDelete .... ru¹í prvek za aktivním prvkem,
**      DLPreDelete ..... ru¹í prvek pøed aktivním prvkem, 
**      DLPostInsert .... vlo¾í nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vlo¾í nový prvek pøed aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... pøepí¹e obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na dal¹í prvek seznamu,
**      DLPred .......... posune aktivitu na pøedchozí prvek seznamu, 
**      DLActive ........ zji¹»uje aktivitu seznamu.
**
** Pøi implementaci jednotlivých funkcí nevolejte ¾ádnou z funkcí
** implementovaných v rámci tohoto pøíkladu, není-li u funkce
** explicitnì uvedeno nìco jiného.
**
** Nemusíte o¹etøovat situaci, kdy místo legálního ukazatele na seznam 
** pøedá nìkdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodnì komentujte!
**
** Terminologická poznámka: Jazyk C nepou¾ívá pojem procedura.
** Proto zde pou¾íváme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornìní na to, ¾e do¹lo k chybì.
** Tato funkce bude volána z nìkterých dále implementovaných operací.
**/	
    printf ("*ERROR* Chyba pøi práci se seznamem.\n");
    errflg = TRUE;             /* globální promìnná -- pøíznak o¹etøení chyby */
    return;
}

void DLInitList (tDLList *L)	{
/*
** Provede inicializaci seznamu L pøed jeho prvním pou¾itím (tzn. ¾ádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádìt nad ji¾ inicializovaným
** seznamem, a proto tuto mo¾nost neo¹etøujte. V¾dy pøedpokládejte,
** ¾e neinicializované promìnné mají nedefinovanou hodnotu.
**/
    
  L->First = NULL;
  L->Last = NULL;
  L->Act = NULL;
}

void DLDisposeList (tDLList *L)	{
/*
** Zru¹í v¹echny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Ru¹ené prvky seznamu budou korektnì
** uvolnìny voláním operace free. 
**/
	
  while (L->First != NULL)
  {
    // pomocna promenna
    tDLElemPtr aux;
    aux = L->First;
    // nasmerujeme zacatek na dalsi prvek seznamu
    L->First = L->First->rptr;
    // uvolnime prvek
    free(aux);
  }
  // "uklidime" ukazatele :-)
  L->Last = NULL;
  L->Act = NULL;
}

void DLInsertFirst (tDLList *L, int val)	{
/*
** Vlo¾í nový prvek na zaèátek seznamu L.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/

  // novy prvek
  tDLElemPtr new = NULL;
  // alokujeme pamet pro novy prvek
  if ((new = malloc(sizeof(struct tDLElem))) == NULL)
    DLError();
  else
  {
    // priradime hodnotu
    new->data = val;
    // vsuneme polozku na zacatek
    new->lptr = NULL;
    new->rptr = L->First;
    // pokud vkladame prvni polozku, musi na ni ukazovat i konec seznamu
    if (L->First == NULL)
      L->Last = new;
    else
      L->First->lptr = new;
      // nasmerujeme zacatek na novou polozku
      L->First = new;
  } 
}

void DLInsertLast(tDLList *L, int val)	{
/*
** Vlo¾í nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/ 	

  // novy prvek
  tDLElemPtr new = NULL;
  // alokujeme pamet pro novy prvek
  if ((new = malloc(sizeof(struct tDLElem))) == NULL)
    DLError();
  else
  {
    // priradime hodnotu
    new->data = val;
    // "prilepime" polozku na konec seznamu
    new->lptr = L->Last;
    new->rptr = NULL;
    // pokud vkladame prvni polozku, musi na ni ukazovat i zacatek seznamu
    if (L->First == NULL)
      L->First = new;
    else
      L->Last->rptr = new;
    // nasmerujeme konec na novou polozku
    L->Last = new;
  }
}

void DLFirst (tDLList *L)	{
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný pøíkaz (nepoèítáme-li return),
** ani¾ byste testovali, zda je seznam L prázdný.
**/
	
  L->Act = L->First;
}

void DLLast (tDLList *L)	{
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný pøíkaz (nepoèítáme-li return),
** ani¾ byste testovali, zda je seznam L prázdný.
**/
	
  L->Act = L->Last;
}

void DLCopyFirst (tDLList *L, int *val)	{
/*
** Vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

  // prazdny seznam
  if (L->First == NULL)
    DLError();
  // vracime hodnotu
  else
    *val = L->First->data;
}

void DLCopyLast (tDLList *L, int *val)	{
/*
** Vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

  // prazdny seznam
  if (L->First == NULL)
    DLError();
  // vracime hodnotu
  else
    *val =  L->Last->data;
}

void DLDeleteFirst (tDLList *L)	{
/*
** Zru¹í první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se nedìje.
**/
	
  if (L->First != NULL)
  {
    // pomocna promenna
    tDLElemPtr aux;
    aux = L->First;
    // aktivni prvek - zrusime aktivitu
    if (L->First == L->Act)
      L->Act = NULL;
    // jediny prvek seznamu - "uklidime" ukazatele
    if (L->First == L->Last)
    {
      L->First = NULL;
      L->Last = NULL;
    }
    // vice prvku seznamu
    else
    {
      L->First = L->First->rptr;
      L->First->lptr = NULL;
    }
    // zrusime prvek
    free(aux);
  }
}	

void DLDeleteLast (tDLList *L)	{
/*
** Zru¹í poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se nedìje.
**/ 
	
	
  if (L->First != NULL)
  {
    // pomocna promenna
    tDLElemPtr aux;
    aux = L->Last;
    // aktivni prvek - zrusime aktivitu
    if (L->Last == L->Act)
      L->Act = NULL;
    // jediny prvek seznamu - "uklidime" ukazatele
    if (L->First == L->Last)
    {
      L->First = NULL;
      L->Last = NULL;
    }
    // vice prvku seznamu
    else
    {
      L->Last = L->Last->lptr;
      L->Last->rptr = NULL;
    }
    // zrusime prvek
    free(aux);
  }
}

void DLPostDelete (tDLList *L)	{
/*
** Zru¹í prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se nedìje.
**/

  // pomocna promenna
  tDLElemPtr aux;
  // seznam je aktivni, posledni prvek seznamu neni aktivni
  if (L->Act != NULL && L->First != NULL && L->Act != L->Last)
  {
    aux =  L->Act->rptr;
    // ukazatel na prvek, ktery chceme rusit
    L->Act->rptr =  aux->rptr;
    // za rusenou polozkou uz neni dalsi, aktivni je posledni
    if (aux == L->Last)
      L->Last =  L->Act;
    // rusena polozka neni posledni, polozku za provazeme s aktivni
    else
      aux->rptr->lptr =  L->Act;
    // zrusime prvek
    free(aux);   
  }   
}

void DLPreDelete (tDLList *L)	{
/*
** Zru¹í prvek pøed aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se nedìje.
**/

  // pomocna promenna
  tDLElemPtr aux;
  // seznam je aktivni, prvni prvek seznamu neni aktivni
  if (L->Act != NULL && L->First != NULL && L->Act != L->First)
  {
    aux =  L->Act->lptr;
    // ukazatel na prvek, ktery chceme rusit
    L->Act->lptr =  aux->lptr;
    // pred rusenou polozkou uz nic neni, aktivni je prvni
    if (aux == L->First)
      L->First =  L->Act;
    // rusena polozka neni prvni, polozku pred provazeme s aktivni
    else
      aux->lptr->rptr =  L->Act;
    // zrusime prvek
    free(aux); 
  }
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vlo¾í prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
	
  if (L->Act != NULL)
  {
    // novy prvek
    tDLElemPtr new = NULL;
    // alokujeme pamet pro novy prvek
    if ((new = malloc(sizeof(struct tDLElem))) == NULL)
      DLError();
    else
    {
      // priradime hodnotu
      new->data =  val;
      // ukazatele na predchozi a dalsi
      new->lptr = L->Act;
      new->rptr =  L->Act->rptr;
      //navazeme polozku do seznamu
      L->Act->rptr = new;
      // navazeme zprava novou polozku
      if (L->Act != L->Last)
	new->rptr->lptr = new;
      // nastavime novou posledni polozku
      else
	L->Last = new;
    } 
  }
}


void DLPreInsert (tDLList *L, int val)		{
/*
** Vlo¾í prvek pøed aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
	
  if (L->Act != NULL)
  {
    // novy prvek
    tDLElemPtr new = NULL;
    // alokujeme pamet pro novy prvek
    if ((new = malloc(sizeof(struct tDLElem))) == NULL)
      DLError();
    else
    {
      // priradime hodnotu
      new->data =  val;
      // ukazatele na predchozi a dalsi
      new->rptr = L->Act;
      new->lptr =  L->Act->lptr;
      // navazeme polozku do seznamu
      L->Act->lptr = new;
      // navazeme zleva novou polozku
      if (L->Act != L->First)
	new->lptr->rptr = new;
      // nastavime novou prvni polozku
      else
	L->First = new;
    } 
  }
}

void DLCopy (tDLList *L, int *val)	{
/*
** Vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/

  // seznam neni aktivni, volame chybu
  if (L->Act == NULL)
    DLError();
  // seznam je aktivni, vratime hodnotu
  else
    *val = L->Act->data;
}

void DLActualize (tDLList *L, int val) {
/*
** Pøepí¹e obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedìlá nic.
**/

  // seznam je aktivni, prepiseme data
  if (L->Act != NULL)
  {
    L->Act->data = val;
  }
}

void DLSucc (tDLList *L)	{
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedìlá nic.
** V¹imnìte si, ¾e pøi aktivitì na posledním prvku se seznam stane neaktivním.
**/

  // seznam je aktivni
  if (L->Act != NULL)
  {
    // aktivni prvek neni posledni, posouvame
    if (L->Act != L->Last)
      L->Act = L->Act->rptr;
    // ztracime aktivitu
    else
      L->Act = NULL;
  }
}


void DLPred (tDLList *L)	{
/*
** Posune aktivitu na pøedchozí prvek seznamu L.
** Není-li seznam aktivní, nedìlá nic.
** V¹imnìte si, ¾e pøi aktivitì na prvním prvku se seznam stane neaktivním.
**/

  // seznam je aktivni
  if (L->Act != NULL)
  {
    // aktivni prvek neni prvni, posouvame
    if (L->Act != L->First)
      L->Act = L->Act->lptr;
    // ztracime aktivitu
    else
      L->Act = NULL;
  }
}

int DLActive (tDLList *L) {		
/*
** Je-li seznam aktivní, vrací true. V opaèném pøípadì vrací false.
** Funkci implementujte jako jediný pøíkaz.
**/
	
  return (L->Act != NULL ? TRUE : FALSE);
}

/* Konec c206.c*/
