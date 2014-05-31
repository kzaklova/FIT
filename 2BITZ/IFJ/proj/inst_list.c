 /* ************************ inst_list.c ************************************* */
/* Soubor:              inst_list.c - Instrukcni list                         */
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

#include "inst_list.h"

int MakeInst(int type, void *addr1, void *addr2, void *addr3, int FuncCounter)
{

tInst *p_new_inst;
if((p_new_inst=malloc(sizeof(tInst))) == NULL)
  {
  AddError(line, ProgErr);
  return 1;
  }
*p_new_inst= InstCreate(type, addr1,addr2,addr3); /* Vytvorim instrukciu */

InsertLast(&InstList,p_new_inst);

if(ErrorFlag != NoErr) return 1; // Doslo k chybe

if(type == I_DECL_FUNC)
  {
  if(funcArray != NULL)
    {
    if((FuncCounter % FUNC_ARRAY_SIZE) == 0)
      {
      tFunc *p_temp= NULL;
      int pocet = FuncCounter / FUNC_ARRAY_SIZE;
      if((p_temp=realloc(funcArray, (sizeof(tFunc) * FUNC_ARRAY_SIZE * (pocet*2)))) == NULL)
        {
        AddError(line, ProgErr);
        return 1;
        }
      for(int i = (pocet * FUNC_ARRAY_SIZE);(i < (pocet * 2 * FUNC_ARRAY_SIZE)); i++)
        {
        funcArray[i].function =NULL;
        funcArray[i].identif = NULL;
        funcArray[i].VarTree = NULL;
        }
      }
    }
  else /* Este nebolo vytvorene pole funkcii */
    {
    if((funcArray = malloc(sizeof(tFunc) * FUNC_ARRAY_SIZE)) == NULL)
      {
      AddError(line, ProgErr);
      return 1;
      }
    for(int i=0;i<FUNC_ARRAY_SIZE;i++)
      {
      funcArray[i].function =NULL;
      funcArray[i].identif = NULL;
      funcArray[i].VarTree = NULL;
      }
    }

    funcArray[FuncCounter-1].function= InstList.Last; /* Priradim ukazatel na zaciatok funkcie */

    if((funcArray[FuncCounter-1].identif= malloc(sizeof(string))) == NULL)
      {
      AddError(line, ProgErr);
      return 1;
      }
    if(strInit(funcArray[FuncCounter-1].identif) == 1)
      {
      AddError(line, ProgErr);
      return 1;
      }

    if(strCopyString(funcArray[FuncCounter-1].identif , (string *)addr1))
      {
      AddError(line, ProgErr);
      return 1;
      }
  }

 if(type == I_END_DECL_FUNC)
  {
  tVarTreeItem **p_var_help = NULL;

  if((getVarTree(&(p_var_help), &(*STable), control.function )) == TREE_ERROR)
    {/* Ziskavam pointer na strom hodnot funkcie s control->control */
    AddError(0, ProgErr);
    return TREE_ERROR;
    }

  if((CopyTree(p_var_help, &(funcArray[FuncCounter-1].VarTree)) == TREE_ERROR))
    { /* Vytvorim do funcArray[FuncCounter-1].VarTree ulozim kopiu stromu hodnot */
    AddError(0, ProgErr);
    return TREE_ERROR;
    }
  }

return 0;
}



tInst InstCreate(int type, void *addr1, void *addr2, void *addr3)
{/* Vytvori a inicializuje polozku zonamu */

 tInst instrukcia={type,addr1,addr2,addr3};
 return instrukcia;
}


void InitInstList (tInstList *L)
{ /* Inicializuje zoznam */
 L->First = NULL;
 L->Last = NULL;
 L->Active = NULL;
 return;
}

void DisposeList (tInstList *L)
{ /* Odstrani cely zoznam */
 tPolozkaList *ptrTemp;
 while(L->First != NULL)
   {
   ptrTemp = L->First->next; /* Ukladam si pointer na dalsiu polozku */
   free(L->First->data);
   free(L->First);
   L->First = ptrTemp;
   }
 L->Active = NULL;
 return;
}

void InsertFirst (tInstList *L,  tInst *new_instr)
{ /* Vlozi prvok na zaciatok zoznamu */
tPolozkaPointer ptrTemp;

ptrTemp = (tPolozkaPointer) malloc(sizeof(tPolozkaList)); /* Alokujem priestor pre strukturu typu tElem */
if(ptrTemp == NULL)
	  { /* Chyba, ohlasim ju a uvolnujem zoznam  */
      AddError(0,ProgErr);
      DisposeList(L);
      }
ptrTemp->data = new_instr;
ptrTemp->next = L->First;

if((L->Last == NULL) || (L->First == NULL)) L->Last=ptrTemp;
/* Je pridavany prvy prvok do prazdneho zoznamu ,tj je zaroven prvym aj poslendym */

L->First=ptrTemp;
return;
}

void InsertLast (tInstList *L, tInst *new_instr)
{/* Vlozi novu polozku na koniec zonznamu */
tPolozkaPointer ptrTemp;

ptrTemp = (tPolozkaPointer) malloc(sizeof(tPolozkaList)); /* Alokujem priestor pre strukturu typu tElem */
if(ptrTemp == NULL)
	  { /* Chyba, ohlasim ju a uvolnujem zoznam  */
      AddError(0,ProgErr);
      DisposeList(L);
      }
ptrTemp->line_number = line;
ptrTemp->data = new_instr;
ptrTemp->next = NULL;

if(L->Last != NULL) L->Last->next = ptrTemp; /* Zretazujem predposledny a posledny prvok */

if((L->Last == NULL) || (L->First == NULL)) L->First=ptrTemp;
/* Je pridavany prvy prvok do prazdneho zoznamu ,tj je zaroven prvym aj poslendym */

L->Last=ptrTemp;
return;
}

void First (tInstList *L)
{ /* Nastavy aktivitu zoznamu na jeho prvy prvok */
 L->Active=L->First;
}

void CopyFirst (tInstList *L, tInst *new_instr)
{ /* Zkopiruje hodnotu prveho prvku zoznamu */
  if(L->First != NULL)
    {
    new_instr->action = L->First->data->action;
    new_instr->addr1 = L->First->data->addr1;
    new_instr->addr2 = L->First->data->addr2;
    new_instr->addr3 = L->First->data->addr3;
    }
  return;
}

void DeleteFirst (tInstList *L)
{ /* Zrusi prvy prvok zoznamu */
 if(L->First != NULL)
   {
   tPolozkaPointer ptrTemp;

   if(L->Active == L->First) L->Active = NULL;
   ptrTemp = L->First;
   L->First = L->First->next; /* Do first ukladam adresu dalsej polozky zoznamu */
   free(ptrTemp);
   }

 return;
  }

void PostDelete (tInstList *L)
{ /* Zrusi prvok zoznamu za aktivnym prvkom */
 if((L->Active != NULL) && (L->Active->next != NULL))
   {
   tPolozkaPointer ptrTemp;

   ptrTemp = L->Active->next;

   L->Active->next = ptrTemp->next; /* Pomocou pomocneho pointru zretazim aktualnu polozku s nasledovnikom rusenej polozky */
   free(ptrTemp);
   }
 return;

}

void PostInsert (tInstList *L, tInst *new_instr)
{  /*  Vlozi novy prvok za aktivny prvok */
 if(L->Active != NULL)
   {
   tPolozkaPointer ptrTemp;

   ptrTemp = malloc(sizeof(tPolozkaList)); /* Alokujem priestor pre strukturu typu tElem */
   if(ptrTemp == NULL)
     {
      AddError(0,ProgErr);
      DisposeList(L);
     }
   ptrTemp->next = L->Active->next;
   ptrTemp->data = new_instr;
   L->Active->next = ptrTemp;
   }
return;
}

void Copy (tInstList *L, tInst *new_instr)
{ /* Vrati hodnotu aktivneho  prvku zoznamu */
 if(L->Active == NULL)
   {
   //AddError(0,ProgErr);
   //DisposeList(L);
   }
 else new_instr = L->Active->data;
 return;
}

void Actualize (tInstList *L, tInst *new_instr)
{ /* Prepise data aktivneho prvku zoznamu */
 if(L->Active != NULL) L->Active->data = new_instr;
 return;
 }

void Succ (tInstList *L)
{ /*  Posunie aktivitu na nasledujuci prvok zoznamu */
 if(L->Active != NULL)
   {
   if(L->Active->next == NULL) L->Active = NULL; /* Activnym bol posledny prvok -> strata aktivity */

   else L->Active = L->Active->next;
   }
 return;
}

int Active (tInstList *L)
{ /* Pokial je zoznam aktivny vrati true */
return (L->Active != NULL);
}

void ListJump(tInstList *L, tPolozkaList *jump_instr)
{ /* Nastavy ako aktivny prvok,prvok so danou adresou */
L->Active=(tPolozkaList *)jump_instr;
}
