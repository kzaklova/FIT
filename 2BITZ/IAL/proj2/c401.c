
/* c401.c: **********************************************************}
{* Téma: Rekurzivní implementace operací nad BVS
**                                         Vytvoøil: Petr Pøikryl, listopad 1994
**                                         Úpravy: Andrea Nìmcová, prosinec 1995
**                                                      Petr Pøikryl, duben 1996
**                                                   Petr Pøikryl, listopad 1997
**                                  Pøevod do jazyka C: Martin Tuèek, øíjen 2005
**                                         Úpravy: Bohuslav Køena, listopad 2009
**
** Implementujte rekurzivním zpùsobem operace nad binárním vyhledávacím
** stromem (BVS; v angliètinì BST - Binary Search Tree).
**
** Klíèem uzlu stromu je jeden znak (obecnì jím mù¾e být cokoliv, podle
** èeho se vyhledává). U¾iteèným (vyhledávaným) obsahem je zde integer.
** Uzly s men¹ím klíèem le¾í vlevo, uzly s vìt¹ím klíèem le¾í ve stromu
** vpravo. Vyu¾ijte dynamického pøidìlování pamìti.
** Rekurzivním zpùsobem implementujte následující funkce:
**
**   BSTInit ...... inicializace vyhledávacího stromu
**   BSTSearch .... vyhledávání hodnoty uzlu zadaného klíèem
**   BSTInsert .... vkládání nové hodnoty
**   BSTDelete .... zru¹ení uzlu se zadaným klíèem
**   BSTDispose ... zru¹ení celého stromu
**
** ADT BVS je reprezentován koøenovým ukazatelem stromu (typ tBSTNodePtr).
** Uzel stromu (struktura typu tBSTNode) obsahuje klíè (typu char), podle
** kterého se ve stromu vyhledává, vlastní obsah uzlu (pro jednoduchost
** typu int) a ukazatel na levý a pravý podstrom (LPtr a RPtr). 
** Pøesnou definici typù naleznete v souboru c401.h.
**
** Pozor! Je tøeba správnì rozli¹ovat, kdy pou¾ít dereferenèní operátor *
** (typicky pøi modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem 
** (napø. pøi vyhledávání). V tomto pøíkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, pou¾ijeme dereferenci.
**/

#include "c401.h"
int solved;

void BSTInit (tBSTNodePtr *RootPtr) {
/*   -------
** Funkce provede poèáteèní inicializaci stromu pøed jeho prvním pou¾itím.
**
** Ovìøit, zda byl ji¾ strom pøedaný pøes RootPtr inicializován, nelze,
** proto¾e pøed první inicializací má ukazatel nedefinovanou (tedy libovolnou)
** hodnotu. Programátor vyu¾ívající ADT BVS tedy musí zajistit, aby inicializace
** byla volána pouze jednou, a to pøed vlastní prací s BVS. Provedení
** inicializace nad neprázdným stromem by toti¾ mohlo vést ke ztrátì pøístupu
** k dynamicky alokované pamìti (tzv. "memory leak").
**	
** V¹imnìte si, ¾e se v hlavièce objevuje typ ukazatel na ukazatel.	
** Proto je tøeba pøi pøiøazení pøes RootPtr pou¾ít dereferenèní operátor *.
** Ten bude pou¾it i ve funkcích BSTDelete, BSTInsert a BSTDispose.
**/
	*RootPtr = NULL;
}

int BSTSearch (tBSTNodePtr RootPtr, char K, int *Content)	{
/*  ---------
** Funkce vyhledá uzel v BVS s klíèem K.
**
** Pokud je takový nalezen, vrací funkce hodnotu TRUE a v promìnné Content se
** vrací obsah pøíslu¹ného uzlu.´Pokud pøíslu¹ný uzel není nalezen, vrací funkce
** hodnotu FALSE a obsah promìnné Content není definován (nic do ní proto
** nepøiøazujte).
**
** Pøi vyhledávání v binárním stromu bychom typicky pou¾ili cyklus ukonèený
** testem dosa¾ení listu nebo nalezení uzlu s klíèem K. V tomto pøípadì ale
** problém øe¹te rekurzivním volání této funkce, pøièem¾ nedeklarujte ¾ádnou
** pomocnou funkci.
**/
	// neprazdny strom
	if (RootPtr != NULL)
	{
		// nasli jsme klic
		if (RootPtr->Key == K) 
		{
			// vracime hodnotu obsahu uzlu
			*Content = RootPtr->BSTNodeCont;
			return TRUE;
		}
		else
		{
			// hledany klic je vetsi, hledame vpravo
			if (RootPtr->Key < K) return BSTSearch(RootPtr->RPtr, K, Content);
			// hledany klic je mensi, hledame vlevo
			else return BSTSearch(RootPtr->LPtr, K, Content);
		}
	}
	// prazdny strom
	else return FALSE;
} 


void BSTInsert (tBSTNodePtr* RootPtr, char K, int Content)	{	
/*   ---------
** Vlo¾í do stromu RootPtr hodnotu Content s klíèem K.
**
** Pokud ji¾ uzel se zadaným klíèem ve stromu existuje, bude obsah uzlu
** s klíèem K nahrazen novou hodnotou. Pokud bude do stromu vlo¾en nový
** uzel, bude vlo¾en v¾dy jako list stromu.
**
** Funkci implementujte rekurzivnì. Nedeklarujte ¾ádnou pomocnou funkci.
**
** Rekurzivní implementace je ménì efektivní, proto¾e se pøi ka¾dém
** rekurzivním zanoøení ukládá na zásobník obsah uzlu (zde integer).
** Nerekurzivní varianta by v tomto pøípadì byla efektivnìj¹í jak z hlediska
** rychlosti, tak z hlediska pamì»ových nárokù. Zde jde ale o ¹kolní
** pøíklad, na kterém si chceme ukázat eleganci rekurzivního zápisu.
**/
	// vkladame koren do prazdneho stromu
	if (*RootPtr == NULL)
	{
		tBSTNodePtr new;
		if ((new = malloc(sizeof(struct tBSTNode))) != NULL)
		{
			// vytvorime klic
			new->Key = K;
			// naplnime obsah
			new->BSTNodeCont = Content;
			// ukazatele jsou NULL (pravy ani levy podstrom neni)
			new->LPtr = NULL;
			new->RPtr = NULL;
			// pripojime uzel do stromu
			(*RootPtr) = new;
		}
	}
	// vkladame podkoren do neprazdneho stromu
	else
	{
		// "vkladany" klic je vetsi nez aktualni, vkladame doprava
		if((*RootPtr)->Key < K) BSTInsert(&(*RootPtr)->RPtr, K, Content);
		// "vkladany" klic je mensi nez aktualni, vkladame doleva
		else if ((*RootPtr)->Key > K) BSTInsert(&(*RootPtr)->LPtr, K, Content);
		// vkladame hodnotu
		else (*RootPtr)->BSTNodeCont = Content;
	}

}

void ReplaceByRightmost (tBSTNodePtr PtrReplaced, tBSTNodePtr *RootPtr) {
/*   ------------------
** Pomocná funkce pro vyhledání, pøesun a uvolnìní nejpravìj¹ího uzlu.
**
** Ukazatel PtrReplaced ukazuje na uzel, do kterého bude pøesunuta hodnota
** nejpravìj¹ího uzlu v podstromu, který je urèen ukazatelem RootPtr.
** Pøedpokládá se, ¾e hodnota ukazatele RootPtr nebude NULL (zajistìte to
** testováním pøed volání této funkce). Tuto funkci implementujte rekurzivnì. 
**
** Tato pomocná funkce bude pou¾ita dále. Ne¾ ji zaènete implementovat,
** pøeètìte si komentáø k funkci BSTDelete(). 
**/
	// pomocna promenna
	tBSTNodePtr aux;
	aux = NULL;

	if ((*RootPtr) != NULL)
	{
		// dosli jsme uplne doprava
		if ((*RootPtr)->RPtr == NULL)
		{
			// nahradime klic
			PtrReplaced->Key = (*RootPtr)->Key;
			// nahradime obsah uzlu
			PtrReplaced->BSTNodeCont = (*RootPtr)->BSTNodeCont;
			// musime zachovat puvodni ukazatel na koren
			aux = (*RootPtr);
			// pripojime do stromu levy podstrom
			*RootPtr = (*RootPtr)->LPtr;
			// uvolnime uzel
			free (aux);
		}
		// stale nejsme uplne vpravo, posuneme se
		else ReplaceByRightmost(PtrReplaced, &(*RootPtr)->RPtr);
	}
}

void BSTDelete (tBSTNodePtr *RootPtr, char K) 		{
/*   ---------
** Zru¹í uzel stromu, který obsahuje klíè K.
**
** Pokud uzel se zadaným klíèem neexistuje, nedìlá funkce nic. 
** Pokud má ru¹ený uzel jen jeden podstrom, pak jej zdìdí otec ru¹eného uzlu.
** Pokud má ru¹ený uzel oba podstromy, pak je ru¹ený uzel nahrazen nejpravìj¹ím
** uzlem levého podstromu. Pozor! Nejpravìj¹í uzel nemusí být listem.
**
** Tuto funkci implementujte rekurzivnì s vyu¾itím døíve deklarované
** pomocné funkce ReplaceByRightmost.
**/
	//pomocna promenna
	tBSTNodePtr aux;
	aux = NULL;
	// rusime jen, pokud mame co rusit (strom neni prazdny)
	if ((*RootPtr) != NULL)
	{
		// hledany klic je vetsi nez aktualni, jdeme doprava
		if ((*RootPtr)->Key < K) BSTDelete(&(*RootPtr)->RPtr, K);
		// hledany klic je mensi nez aktualni, jdeme doleva
		else if((*RootPtr)->Key > K) BSTDelete(&(*RootPtr)->LPtr, K);
		// nasli jsme prvek k odstraneni
		else
		{
			aux = (*RootPtr);
			// nemame pravy uzel
			if ((*RootPtr)->RPtr == NULL)
			{
				// pripojime levy podstrom a zrusime uzel
				(*RootPtr) = aux->LPtr;
				free(aux);
			}
			// nemame levy uzel
			else if((*RootPtr)->LPtr == NULL)
			{
				// pripojime pravy podstrom a zrusime uzel
				(*RootPtr) = aux->RPtr;
				free(aux);
			}
			// odstranovany ma oba podstromy
			else ReplaceByRightmost(*RootPtr, &(*RootPtr)->LPtr);
		}
	}
} 

void BSTDispose (tBSTNodePtr *RootPtr) {	
/*   ----------
** Zru¹í celý binární vyhledávací strom a korektnì uvolní pamì».
**
** Po zru¹ení se bude BVS nacházet ve stejném stavu, jako se nacházel po
** inicializaci. Tuto funkci implementujte rekurzivnì bez deklarování pomocné
** funkce.
**/
	if ((*RootPtr) != NULL)
	{
		// uvolnime leve uzly
		BSTDispose (&(*RootPtr)->LPtr);
		// uvolnime prave uzly
		BSTDispose (&(*RootPtr)->RPtr);
		// uvolnime koren
		free (*RootPtr);
		// inicializujeme
		*RootPtr = NULL;
	}
}

/* konec c401.c */

