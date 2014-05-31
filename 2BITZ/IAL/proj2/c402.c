
/* c402.c: ********************************************************************}
{* Téma: Nerekurzivní implementace operací nad BVS 
**                                     Implementace: Petr Pøikryl, prosinec 1994
**                                           Úpravy: Petr Pøikryl, listopad 1997
**                                                     Petr Pøikryl, kvìten 1998
**			  	                        Pøevod do jazyka C: Martin Tuèek, srpen 2005
**                                         Úpravy: Bohuslav Køena, listopad 2009
**
** S vyu¾itím dynamického pøidìlování pamìti, implementujte NEREKURZIVNÌ
** následující operace nad binárním vyhledávacím stromem (pøedpona BT znamená
** Binary Tree a je u identifikátorù uvedena kvùli mo¾né kolizi s ostatními
** pøíklady):
**
**     BTInit .......... inicializace stromu
**     BTInsert ........ nerekurzivní vlo¾ení nového uzlu do stromu
**     BTPreorder ...... nerekurzivní prùchod typu pre-order
**     BTInorder ....... nerekurzivní prùchod typu in-order
**     BTPostorder ..... nerekurzivní prùchod typu post-order
**     BTDisposeTree ... zru¹ v¹echny uzly stromu
**
** U v¹ech funkcí, které vyu¾ívají nìkterý z prùchodù stromem, implementujte
** pomocnou funkci pro nalezení nejlevìj¹ího uzlu v podstromu.
**
** Pøesné definice typù naleznete v souboru c402.h. Uzel stromu je typu tBTNode,
** ukazatel na nìj je typu tBTNodePtr. Jeden uzel obsahuje polo¾ku int Cont,
** která souèasnì slou¾í jako u¾iteèný obsah i jako vyhledávací klíè 
** a ukazatele na levý a pravý podstrom (LPtr a RPtr).
**
** Pøíklad slou¾í zejména k procvièení nerekurzivních zápisù algoritmù
** nad stromy. Ne¾ zaènete tento pøíklad øe¹it, prostudujte si dùkladnì
** principy pøevodu rekurzivních algoritmù na nerekurzivní. Programování
** je pøedev¹ím in¾enýrská disciplína, kde opìtné objevování Ameriky nemá
** místo. Pokud se Vám zdá, ¾e by nìco ¹lo zapsat optimálnìji, promyslete
** si v¹echny detaily Va¹eho øe¹ení. Pov¹imnìte si typického umístìní akcí
** pro rùzné typy prùchodù. Zamyslete se nad modifikací øe¹ených algoritmù
** napøíklad pro výpoèet poètu uzlù stromu, poètu listù stromu, vý¹ky stromu
** nebo pro vytvoøení zrcadlového obrazu stromu (pouze popøehazování ukazatelù
** bez vytváøení nových uzlù a ru¹ení starých).
**
** Pøi prùchodech stromem pou¾ijte ke zpracování uzlu funkci BTWorkOut().
** Pro zjednodu¹ení práce máte pøedem pøipraveny zásobníky pro hodnoty typu
** bool a tBTNodePtr. Pomocnou funkci BTWorkOut ani funkce pro práci
** s pomocnými zásobníky neupravujte 
**
** Pozor! Je tøeba správnì rozli¹ovat, kdy pou¾ít dereferenèní operátor *
** (typicky pøi modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem 
** (napø. pøi vyhledávání). V tomto pøíkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, pou¾ijeme dereferenci.
**/

#include "c402.h"
int solved;

void BTWorkOut (tBTNodePtr Ptr)		{
/*   ---------
** Pomocná funkce, kterou budete volat pøi prùchodech stromem pro zpracování
** uzlu urèeného ukazatelem Ptr. Tuto funkci neupravujte.
**/
			
	if (Ptr==NULL) 
    printf("Chyba: Funkce BTWorkOut byla volána s NULL argumentem!\n");
  else 
    printf("Výpis hodnoty daného uzlu> %d\n",Ptr->Cont);
}
	
/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu tBTNodePtr. Tyto funkce neupravujte.
**/

void SInitP (tStackP *S)  
/*   ------
** Inicializace zásobníku.
**/
{
	S->top = 0;  
}	

void SPushP (tStackP *S, tBTNodePtr ptr)
/*   ------
** Vlo¾í hodnotu na vrchol zásobníku.
**/
{ 
                 /* Pøi implementaci v poli mù¾e dojít k pøeteèení zásobníku. */
  if (S->top==MAXSTACK) 
    printf("Chyba: Do¹lo k pøeteèení zásobníku s ukazateli!\n");
  else {  
		S->top++;  
		S->a[S->top]=ptr;
	}
}	

tBTNodePtr STopPopP (tStackP *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a souèasnì vrátí jeho hodnotu.
**/
{
                            /* Operace nad prázdným zásobníkem zpùsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Do¹lo k podteèení zásobníku s ukazateli!\n");
		return(NULL);	
	}	
	else {
		return (S->a[S->top--]);
	}	
}

bool SEmptyP (tStackP *S)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
  return(S->top==0);
}	

/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu bool. Tyto funkce neupravujte.
*/

void SInitB (tStackB *S) {
/*   ------
** Inicializace zásobníku.
**/

	S->top = 0;  
}	

void SPushB (tStackB *S,bool val) {
/*   ------
** Vlo¾í hodnotu na vrchol zásobníku.
**/
                 /* Pøi implementaci v poli mù¾e dojít k pøeteèení zásobníku. */
	if (S->top==MAXSTACK) 
		printf("Chyba: Do¹lo k pøeteèení zásobníku pro boolean!\n");
	else {
		S->top++;  
		S->a[S->top]=val;
	}	
}

bool STopPopB (tStackB *S) {
/*   --------
** Odstraní prvek z vrcholu zásobníku a souèasnì vrátí jeho hodnotu.
**/
                            /* Operace nad prázdným zásobníkem zpùsobí chybu. */
	if (S->top==0) {
		printf("Chyba: Do¹lo k podteèení zásobníku pro boolean!\n");
		return(NULL);	
	}	
	else {  
		return(S->a[S->top--]); 
	}	
}

bool SEmptyB (tStackB *S) {
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** Následuje jádro domácí úlohy - funkce, které máte implementovat. 
*/

void BTInit (tBTNodePtr *RootPtr)	{
/*   ------
** Provede inicializaci binárního vyhledávacího stromu.
**
** Inicializaci smí programátor volat pouze pøed prvním pou¾itím binárního
** stromu, proto¾e neuvolòuje uzly neprázdného stromu (a ani to dìlat nemù¾e,
** proto¾e pøed inicializací jsou hodnoty nedefinované, tedy libovolné).
** Ke zru¹ení binárního stromu slou¾í procedura BTDisposeTree.
**	
** V¹imnìte si, ¾e zde se poprvé v hlavièce objevuje typ ukazatel na ukazatel,	
** proto je tøeba pøi práci s RootPtr pou¾ít dereferenèní operátor *.
**/
	(*RootPtr) = NULL;
}

void BTInsert (tBTNodePtr *RootPtr, int Content) {
/*   --------
** Vlo¾í do stromu nový uzel s hodnotou Content.
**
** Z pohledu vkládání chápejte vytváøený strom jako binární vyhledávací strom,
** kde uzly s hodnotou men¹í ne¾ má otec le¾í v levém podstromu a uzly vìt¹í
** le¾í vpravo. Pokud vkládaný uzel ji¾ existuje, neprovádí se nic (daná hodnota
** se ve stromu mù¾e vyskytnout nejvý¹e jednou). Pokud se vytváøí nový uzel,
** vzniká v¾dy jako list stromu. Funkci implementujte nerekurzivnì.
**/
	// ukazatel na aktualni uzel
	tBTNodePtr aux = (*RootPtr);
	// ukazatel na otce
	tBTNodePtr faux = NULL;
	// novy uzel
	tBTNodePtr new;

	// vkladame do prazdneho stromu
	if (aux == NULL)
	{
		// pokud se nam zdari alokace, vkladame
		if ((new = malloc(sizeof(struct tBTNode))) != NULL)
		{
			// ulozime obsah uzlu
			new->Cont = Content;
			// pravy ani levy podstrom neni
			new->LPtr = NULL;
			new->RPtr = NULL;
			// pripojime uzel do stromu
			(*RootPtr) = new;
		}
	}
	// vkladame do neprazdneho stromu
	else
	{
		// dokud nenajdeme misto, kam bychom mohli vkladat
		while (aux != NULL)
		{
			// vkladany obsah je vetsi, jdeme doleva
			if (aux->Cont > Content)
			{
				faux = aux;
				aux = aux->LPtr;
			}
			// vkladany obsah je mensi, jdeme doprava
			else if (aux->Cont < Content)
			{
				faux = aux;
				aux = aux->RPtr;
			}
			// vkladany obsah je stejny
			else return;
		}

		// pokud se nam zdari alokace, vkladame
		if ((new = malloc(sizeof(struct tBTNode))) != NULL)
		{
			// ulozime obsah uzlu
			new->Cont = Content;
			// pravy ani levy podstrom neni
			new->LPtr = NULL;
			new->RPtr = NULL;
			// pripojujeme zleva
			if (faux->Cont > Content) faux->LPtr = new;
			// pripojujeme zprava
			else faux->RPtr = new;
		}
	}
}
/*                                  PREORDER                                  */

void Leftmost_Preorder (tBTNodePtr ptr, tStackP *Stack)	{
/*   -----------------
** Jde po levì vìtvi podstromu, dokud nenarazí na jeho nejlevìj¹í uzel.
**
** Pøi prùchodu Preorder nav¹tívené uzly zpracujeme voláním funkce BTWorkOut()
** a ukazatele na nì is ulo¾íme do zásobníku.
**/
	// dokud neprojdeme vsechny pripustne uzly
	while(ptr != NULL)
	{
		// dame si ukazatel na uzel na zasobnik
		SPushP(Stack, ptr);
		// zpracovujeme uzel
		BTWorkOut(ptr);
		// postupujeme doleva
		ptr = ptr->LPtr;
	}

}

void BTPreorder (tBTNodePtr RootPtr)	{
/*   ----------
** Prùchod stromem typu preorder implementovaný nerekurzivnì s vyu¾itím funkce
** Leftmost_Preorder a zásobníku ukazatelù. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut(). 
**/
	// zasobnik ukazatelu
	tStackP StackP;
	SInitP(&StackP);
	// pomocny ukazatel
	tBTNodePtr aux;
	aux = RootPtr;
	// zpracujeme vsechny nejlevejsi uzly
	Leftmost_Preorder(aux, &StackP);
	// dokud nevyprazdnime zasobnik
	while(!SEmptyP(&StackP))
	{
		// nacteme jiz zpracovany uzel
		aux = STopPopP(&StackP);
		// zpracujeme nejlevejsi uzly jeho praveho podstromu
		Leftmost_Preorder(aux->RPtr, &StackP);
	}
}


/*                                  INORDER                                   */ 

void Leftmost_Inorder(tBTNodePtr ptr, tStackP *Stack)		{
/*   ----------------
** Jde po levì vìtvi podstromu, dokud nenarazí na jeho nejlevìj¹í uzel.
**
** Pøi prùchodu Inorder ukládáme ukazatele na v¹echny nav¹tívené uzly do
** zásobníku. 
**/
	// dokud neprojdeme vsechny pripustne uzly
	while(ptr != NULL)
	{
		// dame si ukazatel na uzel na zasobnik
		SPushP(Stack, ptr);
		// postupujeme doleva
		ptr = ptr->LPtr;
	}
}

void BTInorder (tBTNodePtr RootPtr)	{
/*   ---------
** Prùchod stromem typu inorder implementovaný nerekurzivnì s vyu¾itím funkce
** Leftmost_Inorder a zásobníku ukazatelù. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut(). 
**/
	// zasobnik ukazatelu
	tStackP StackP;
	SInitP(&StackP);
	// pomocny ukazatel
	tBTNodePtr aux;
	aux = RootPtr;
	// ulozime vsechny nejlevejsi uzly
	Leftmost_Inorder(aux, &StackP);
	// dokud nevyprazdnime zasobnik
	while(!SEmptyP(&StackP))
	{
		// nacteme uzel ze zasobniku
		aux = STopPopP(&StackP);
		// zpracujeme uzel
		BTWorkOut(aux);
		// ulozime nejlevejsi uzly jeho praveho podstromu
		Leftmost_Inorder(aux->RPtr, &StackP);
	}
}

/*                                 POSTORDER                                  */ 

void Leftmost_Postorder (tBTNodePtr ptr, tStackP *StackP, tStackB *StackB) {
/*           --------
** Jde po levì vìtvi podstromu, dokud nenarazí na jeho nejlevìj¹í uzel.
**
** Pøi prùchodu Postorder ukládáme ukazatele na nav¹tívené uzly do zásobníku
** a souèasnì do zásobníku bool hodnot ukládáme informaci, zda byl uzel
** nav¹tíven poprvé a ¾e se tedy je¹tì nemá zpracovávat. 
**/
	// dokud neprojdeme vsechny pripustne uzly
	while(ptr != NULL)
	{
		// dame si ukazatel na uzel na zasobnik
		SPushP(StackP, ptr);
		// navstiveni uzlu, ulozime priznak
		SPushB(StackB, true);
		// postupujeme doleva
		ptr = ptr->LPtr;
	}
}

void BTPostorder (tBTNodePtr RootPtr)	{
/*           -----------
** Prùchod stromem typu postorder implementovaný nerekurzivnì s vyu¾itím funkce
** Leftmost_Postorder, zásobníku ukazatelù a zásobníku hotdnot typu bool.
** Zpracování jednoho uzlu stromu realizujte jako volání funkce BTWorkOut(). 
**/
	// zasobnik ukazatelu
	tStackP StackP;
	SInitP(&StackP);
	// zasobnik priznaku
	tStackB StackB;
	SInitB(&StackB);
	// pomocny ukazatel
	tBTNodePtr aux;
	aux = RootPtr;
	// ulozime vsechny nejlevejsi uzly a priznaky
	Leftmost_Postorder(aux, &StackP, &StackB);
	// dokud nevyprazdnime zasobnik
	while(!SEmptyP(&StackP))
	{
		// podivame se na vrchol zasobniku a zase vrchol vratime
		aux = STopPopP(&StackP);
		SPushP(&StackP, aux);
		// prisli jsme k uzlu podruhe
		if (STopPopB(&StackB) == true)
		{
			// priste uz budeme uzel zpracovavat
			SPushB(&StackB, false);
			// zpracujeme nejlevejsi uzly jeho praveho podstromu
			Leftmost_Postorder(aux->RPtr, &StackP, &StackB);
		}
		// prisli jsme k uzlu potreti
		else
		{
			// odebereme uzel ze zasobniku
			STopPopP(&StackP);
			// zpracujeme uzel
			BTWorkOut(aux);
		}
	}
}


void BTDisposeTree (tBTNodePtr *RootPtr)	{
/*   -------------
** Zru¹í v¹echny uzly stromu a korektnì uvolní jimi zabranou pamì».
**
** Funkci implementujte nerekurzivnì s vyu¾itím zásobníku ukazatelù.
**/
	// zasobnik ukazatelu
	tStackP StackP;
	SInitP(&StackP);

	// pomocne ukazatele na uzly
	tBTNodePtr aux;
	tBTNodePtr auxx;
	aux = *RootPtr;
	auxx = NULL;

	// pokud strom neni prazdny
	if (aux != NULL)
	{
		do {
			// pokud nemame co zrusit, odebereme ukazatel
			if (aux == NULL && !(SEmptyP(&StackP))) aux = STopPopP(&StackP);
			// pokud mame co rusit
			else
			{
				// ulozime ukazatel na pravy strom na zasobnik
				if (aux->RPtr != NULL) SPushP(&StackP,aux->RPtr);
				// ruseny prvek
				auxx = aux;
				// posuneme se doleva
				aux = aux->LPtr;
				// uvolnime uzel
				free(auxx);
			}
		// dokud neodstranime strom
		} while ((aux != NULL) || (!SEmptyP(&StackP)));
	}

	// inicializace
	*RootPtr = NULL;
}

/* konec c402.c */

