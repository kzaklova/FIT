/* ************************* precedence.c *********************************** */
/* Soubor:              precedence.c - Precedencni analyza                    */
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
/* ****************************************************************************/

#include "precedence.h"

tVarValue *intermResult;

//definice pravidel - zapsany prevracene, protoze tak ve skutcenosti budou na
//zasobniku ;)
static int rule39[3] = {SA_EXPR, SA_EQUAL, SA_EXPR};
static int rule40[3] = {SA_EXPR, SA_NOTEQUAL, SA_EXPR};
static int rule41[3] = {SA_EXPR, SA_LESSEQUAL, SA_EXPR};
static int rule42[3] = {SA_EXPR, SA_MOREEQUAL, SA_EXPR};
static int rule43[3] = {SA_EXPR, SA_MORE, SA_EXPR};
static int rule44[3] = {SA_EXPR, SA_LESS, SA_EXPR};
static int rule45[3] = {SA_EXPR, SA_CONCATE, SA_EXPR};
static int rule46[3] = {SA_EXPR, SA_ADD, SA_EXPR};
static int rule47[3] = {SA_EXPR, SA_SUB, SA_EXPR};
static int rule48[3] = {SA_EXPR, SA_MODULO, SA_EXPR};
static int rule49[3] = {SA_EXPR, SA_MUL, SA_EXPR};
static int rule50[3] = {SA_EXPR, SA_DIV, SA_EXPR};
static int rule51[3] = {SA_EXPR, SA_POWER, SA_EXPR};
static int rule52[3] = {SA_EXPR, SA_LENGTH, 0};
//static int rule53[3] = {SA_SUB, SA_EXPR, 0};
static int rule54[3] = {SA_IDENTIFIER, 0, 0};
static int rule55[3] = {SA_RBRACE, SA_EXPR, SA_LBRACE};
static int rule56[3] = {SA_DATATYPE, 0, 0};

//precedencni tabulka - implicitne inicializovana vsude na nulu
static int precedenceTable[21][21];

//fce inicializujici precedencni tabulku
void precedenceTableInit(int (*precedenceTable)[21])	{
	//PRECEDENCE TABLE BEGIN
	precedenceTable[SA_EQUAL][SA_CONCATE] = PT_LESS;
	precedenceTable[SA_EQUAL][SA_ADD] = PT_LESS;
	precedenceTable[SA_EQUAL][SA_SUB] = PT_LESS;
	precedenceTable[SA_EQUAL][SA_MUL] = PT_LESS;
	precedenceTable[SA_EQUAL][SA_DIV] = PT_LESS;
	precedenceTable[SA_EQUAL][SA_MODULO] = PT_LESS;
	precedenceTable[SA_EQUAL][SA_LENGTH] = PT_LESS;
	precedenceTable[SA_EQUAL][SA_UNARYMINUS] = PT_LESS;
	precedenceTable[SA_EQUAL][SA_POWER] = PT_LESS;
	precedenceTable[SA_EQUAL][SA_LBRACE] = PT_LESS;
	precedenceTable[SA_EQUAL][SA_IDENTIFIER] = PT_LESS;
	precedenceTable[SA_EQUAL][SA_DATATYPE] = PT_LESS;
	precedenceTable[SA_NOTEQUAL][SA_CONCATE] = PT_LESS;
	precedenceTable[SA_NOTEQUAL][SA_ADD] = PT_LESS;
	precedenceTable[SA_NOTEQUAL][SA_SUB] = PT_LESS;
	precedenceTable[SA_NOTEQUAL][SA_MUL] = PT_LESS;
	precedenceTable[SA_NOTEQUAL][SA_DIV] = PT_LESS;
	precedenceTable[SA_NOTEQUAL][SA_MODULO] = PT_LESS;
	precedenceTable[SA_NOTEQUAL][SA_LENGTH] = PT_LESS;
	precedenceTable[SA_NOTEQUAL][SA_UNARYMINUS] = PT_LESS;
	precedenceTable[SA_NOTEQUAL][SA_POWER] = PT_LESS;
	precedenceTable[SA_NOTEQUAL][SA_LBRACE] = PT_LESS;
	precedenceTable[SA_NOTEQUAL][SA_IDENTIFIER] = PT_LESS;
	precedenceTable[SA_NOTEQUAL][SA_DATATYPE] = PT_LESS;
	precedenceTable[SA_LESSEQUAL][SA_CONCATE] = PT_LESS;
	precedenceTable[SA_LESSEQUAL][SA_ADD] = PT_LESS;
	precedenceTable[SA_LESSEQUAL][SA_SUB] = PT_LESS;
	precedenceTable[SA_LESSEQUAL][SA_MUL] = PT_LESS;
	precedenceTable[SA_LESSEQUAL][SA_DIV] = PT_LESS;
	precedenceTable[SA_LESSEQUAL][SA_MODULO] = PT_LESS;
	precedenceTable[SA_LESSEQUAL][SA_LENGTH] = PT_LESS;
	precedenceTable[SA_LESSEQUAL][SA_UNARYMINUS] = PT_LESS;
	precedenceTable[SA_LESSEQUAL][SA_POWER] = PT_LESS;
	precedenceTable[SA_LESSEQUAL][SA_LBRACE] = PT_LESS;
	precedenceTable[SA_LESSEQUAL][SA_IDENTIFIER] = PT_LESS;
	precedenceTable[SA_LESSEQUAL][SA_DATATYPE] = PT_LESS;
	precedenceTable[SA_MOREEQUAL][SA_CONCATE] = PT_LESS;
	precedenceTable[SA_MOREEQUAL][SA_ADD] = PT_LESS;
	precedenceTable[SA_MOREEQUAL][SA_SUB] = PT_LESS;
	precedenceTable[SA_MOREEQUAL][SA_MUL] = PT_LESS;
	precedenceTable[SA_MOREEQUAL][SA_DIV] = PT_LESS;
	precedenceTable[SA_MOREEQUAL][SA_MODULO] = PT_LESS;
	precedenceTable[SA_MOREEQUAL][SA_LENGTH] = PT_LESS;
	precedenceTable[SA_MOREEQUAL][SA_UNARYMINUS] = PT_LESS;
	precedenceTable[SA_MOREEQUAL][SA_POWER] = PT_LESS;
	precedenceTable[SA_MOREEQUAL][SA_LBRACE] = PT_LESS;
	precedenceTable[SA_MOREEQUAL][SA_IDENTIFIER] = PT_LESS;
	precedenceTable[SA_MOREEQUAL][SA_DATATYPE] = PT_LESS;
	precedenceTable[SA_LESS][SA_CONCATE] = PT_LESS;
	precedenceTable[SA_LESS][SA_ADD] = PT_LESS;
	precedenceTable[SA_LESS][SA_SUB] = PT_LESS;
	precedenceTable[SA_LESS][SA_MUL] = PT_LESS;
	precedenceTable[SA_LESS][SA_DIV] = PT_LESS;
	precedenceTable[SA_LESS][SA_MODULO] = PT_LESS;
	precedenceTable[SA_LESS][SA_LENGTH] = PT_LESS;
	precedenceTable[SA_LESS][SA_UNARYMINUS] = PT_LESS;
	precedenceTable[SA_LESS][SA_POWER] = PT_LESS;
	precedenceTable[SA_LESS][SA_LBRACE] = PT_LESS;
	precedenceTable[SA_LESS][SA_IDENTIFIER] = PT_LESS;
	precedenceTable[SA_LESS][SA_DATATYPE] = PT_LESS;
	precedenceTable[SA_MORE][SA_CONCATE] = PT_LESS;
	precedenceTable[SA_MORE][SA_ADD] = PT_LESS;
	precedenceTable[SA_MORE][SA_SUB] = PT_LESS;
	precedenceTable[SA_MORE][SA_MUL] = PT_LESS;
	precedenceTable[SA_MORE][SA_DIV] = PT_LESS;
	precedenceTable[SA_MORE][SA_MODULO] = PT_LESS;
	precedenceTable[SA_MORE][SA_LENGTH] = PT_LESS;
	precedenceTable[SA_MORE][SA_UNARYMINUS] = PT_LESS;
	precedenceTable[SA_MORE][SA_POWER] = PT_LESS;
	precedenceTable[SA_MORE][SA_LBRACE] = PT_LESS;
	precedenceTable[SA_MORE][SA_IDENTIFIER] = PT_LESS;
	precedenceTable[SA_MORE][SA_DATATYPE] = PT_LESS;
	precedenceTable[SA_CONCATE][SA_ADD] = PT_LESS;
	precedenceTable[SA_CONCATE][SA_SUB] = PT_LESS;
	precedenceTable[SA_CONCATE][SA_MUL] = PT_LESS;
	precedenceTable[SA_CONCATE][SA_DIV] = PT_LESS;
	precedenceTable[SA_CONCATE][SA_MODULO] = PT_LESS;
	precedenceTable[SA_CONCATE][SA_LENGTH] = PT_LESS;
	precedenceTable[SA_CONCATE][SA_UNARYMINUS] = PT_LESS;
	precedenceTable[SA_CONCATE][SA_POWER] = PT_LESS;
	precedenceTable[SA_CONCATE][SA_LBRACE] = PT_LESS;
	precedenceTable[SA_CONCATE][SA_IDENTIFIER] = PT_LESS;
	precedenceTable[SA_CONCATE][SA_DATATYPE] = PT_LESS;
	precedenceTable[SA_ADD][SA_MUL] = PT_LESS;
	precedenceTable[SA_ADD][SA_DIV] = PT_LESS;
	precedenceTable[SA_ADD][SA_MODULO] = PT_LESS;
	precedenceTable[SA_ADD][SA_LENGTH] = PT_LESS;
	precedenceTable[SA_ADD][SA_UNARYMINUS] = PT_LESS;
	precedenceTable[SA_ADD][SA_POWER] = PT_LESS;
	precedenceTable[SA_ADD][SA_LBRACE] = PT_LESS;
	precedenceTable[SA_ADD][SA_IDENTIFIER] = PT_LESS;
	precedenceTable[SA_ADD][SA_DATATYPE] = PT_LESS;
	precedenceTable[SA_SUB][SA_MUL] = PT_LESS;
	precedenceTable[SA_SUB][SA_DIV] = PT_LESS;
	precedenceTable[SA_SUB][SA_MODULO] = PT_LESS;
	precedenceTable[SA_SUB][SA_LENGTH] = PT_LESS;
	precedenceTable[SA_SUB][SA_UNARYMINUS] = PT_LESS;
	precedenceTable[SA_SUB][SA_POWER] = PT_LESS;
	precedenceTable[SA_SUB][SA_LBRACE] = PT_LESS;
	precedenceTable[SA_SUB][SA_IDENTIFIER] = PT_LESS;
	precedenceTable[SA_SUB][SA_DATATYPE] = PT_LESS;
	precedenceTable[SA_MUL][SA_LENGTH] = PT_LESS;
	precedenceTable[SA_MUL][SA_UNARYMINUS] = PT_LESS;
	precedenceTable[SA_MUL][SA_POWER] = PT_LESS;
	precedenceTable[SA_MUL][SA_LBRACE] = PT_LESS;
	precedenceTable[SA_MUL][SA_IDENTIFIER] = PT_LESS;
	precedenceTable[SA_MUL][SA_DATATYPE] = PT_LESS;
	precedenceTable[SA_DIV][SA_LENGTH] = PT_LESS;
	precedenceTable[SA_DIV][SA_UNARYMINUS] = PT_LESS;
	precedenceTable[SA_DIV][SA_POWER] = PT_LESS;
	precedenceTable[SA_DIV][SA_LBRACE] = PT_LESS;
	precedenceTable[SA_DIV][SA_IDENTIFIER] = PT_LESS;
	precedenceTable[SA_DIV][SA_DATATYPE] = PT_LESS;
	precedenceTable[SA_MODULO][SA_LENGTH] = PT_LESS;
	precedenceTable[SA_MODULO][SA_UNARYMINUS] = PT_LESS;
	precedenceTable[SA_MODULO][SA_POWER] = PT_LESS;
	precedenceTable[SA_MODULO][SA_LBRACE] = PT_LESS;
	precedenceTable[SA_MODULO][SA_IDENTIFIER] = PT_LESS;
	precedenceTable[SA_MODULO][SA_DATATYPE] = PT_LESS;
	precedenceTable[SA_LENGTH][SA_POWER] = PT_LESS;
	precedenceTable[SA_LENGTH][SA_LBRACE] = PT_LESS;
	precedenceTable[SA_LENGTH][SA_IDENTIFIER] = PT_LESS;
	precedenceTable[SA_LENGTH][SA_DATATYPE] = PT_LESS;
	precedenceTable[SA_UNARYMINUS][SA_POWER] = PT_LESS;
	precedenceTable[SA_UNARYMINUS][SA_LBRACE] = PT_LESS;
	precedenceTable[SA_UNARYMINUS][SA_IDENTIFIER] = PT_LESS;
	precedenceTable[SA_UNARYMINUS][SA_DATATYPE] = PT_LESS;
	precedenceTable[SA_POWER][SA_POWER] = PT_LESS;
	precedenceTable[SA_POWER][SA_LBRACE] = PT_LESS;
	precedenceTable[SA_POWER][SA_IDENTIFIER] = PT_LESS;
	precedenceTable[SA_POWER][SA_DATATYPE] = PT_LESS;
	precedenceTable[SA_LBRACE][SA_EQUAL] = PT_LESS;
	precedenceTable[SA_LBRACE][SA_NOTEQUAL] = PT_LESS;
	precedenceTable[SA_LBRACE][SA_LESSEQUAL] = PT_LESS;
	precedenceTable[SA_LBRACE][SA_MOREEQUAL] = PT_LESS;
	precedenceTable[SA_LBRACE][SA_LESS] = PT_LESS;
	precedenceTable[SA_LBRACE][SA_MORE] = PT_LESS;
	precedenceTable[SA_LBRACE][SA_CONCATE] = PT_LESS;
	precedenceTable[SA_LBRACE][SA_ADD] = PT_LESS;
	precedenceTable[SA_LBRACE][SA_SUB] = PT_LESS;
	precedenceTable[SA_LBRACE][SA_MUL] = PT_LESS;
	precedenceTable[SA_LBRACE][SA_DIV] = PT_LESS;
	precedenceTable[SA_LBRACE][SA_MODULO] = PT_LESS;
	precedenceTable[SA_LBRACE][SA_LENGTH] = PT_LESS;
	precedenceTable[SA_LBRACE][SA_UNARYMINUS] = PT_LESS;
	precedenceTable[SA_LBRACE][SA_POWER] = PT_LESS;
	precedenceTable[SA_LBRACE][SA_LBRACE] = PT_LESS;
	precedenceTable[SA_LBRACE][SA_RBRACE] = PT_EQUAL;
	precedenceTable[SA_LBRACE][SA_IDENTIFIER] = PT_LESS;
	precedenceTable[SA_LBRACE][SA_DATATYPE] = PT_LESS;
	precedenceTable[SA_LBRACE][SA_TERMINAL] = PT_NULL;
	precedenceTable[SA_RBRACE][SA_LENGTH] = PT_NULL;
	precedenceTable[SA_RBRACE][SA_UNARYMINUS] = PT_NULL;
	precedenceTable[SA_RBRACE][SA_LBRACE] = PT_NULL;
	precedenceTable[SA_RBRACE][SA_IDENTIFIER] = PT_NULL;
	precedenceTable[SA_RBRACE][SA_DATATYPE] = PT_NULL;
	precedenceTable[SA_IDENTIFIER][SA_LENGTH] = PT_NULL;
	precedenceTable[SA_IDENTIFIER][SA_UNARYMINUS] = PT_NULL;
	precedenceTable[SA_IDENTIFIER][SA_LBRACE] = PT_NULL;
	precedenceTable[SA_IDENTIFIER][SA_IDENTIFIER] = PT_NULL;
	precedenceTable[SA_IDENTIFIER][SA_DATATYPE] = PT_NULL;
	precedenceTable[SA_DATATYPE][SA_LENGTH] = PT_NULL;
	precedenceTable[SA_DATATYPE][SA_UNARYMINUS] = PT_NULL;
	precedenceTable[SA_DATATYPE][SA_LBRACE] = PT_NULL;
	precedenceTable[SA_DATATYPE][SA_IDENTIFIER] = PT_NULL;
	precedenceTable[SA_DATATYPE][SA_DATATYPE] = PT_NULL;
	precedenceTable[SA_IDENTIFIER][SA_DATATYPE] = PT_NULL;
	precedenceTable[SA_TERMINAL][SA_RBRACE] = PT_NULL;
	precedenceTable[SA_TERMINAL][SA_TERMINAL] = PT_NULL;
	precedenceTable[SA_TERMINAL][SA_EQUAL] = PT_LESS;
	precedenceTable[SA_TERMINAL][SA_NOTEQUAL] = PT_LESS;
	precedenceTable[SA_TERMINAL][SA_LESSEQUAL] = PT_LESS;
	precedenceTable[SA_TERMINAL][SA_MOREEQUAL] = PT_LESS;
	precedenceTable[SA_TERMINAL][SA_LESS] = PT_LESS;
	precedenceTable[SA_TERMINAL][SA_MORE] = PT_LESS;
	precedenceTable[SA_TERMINAL][SA_CONCATE] = PT_LESS;
	precedenceTable[SA_TERMINAL][SA_ADD] = PT_LESS;
	precedenceTable[SA_TERMINAL][SA_SUB] = PT_LESS;
	precedenceTable[SA_TERMINAL][SA_MUL] = PT_LESS;
	precedenceTable[SA_TERMINAL][SA_DIV] = PT_LESS;
	precedenceTable[SA_TERMINAL][SA_MODULO] = PT_LESS;
	precedenceTable[SA_TERMINAL][SA_LENGTH] = PT_LESS;
	precedenceTable[SA_TERMINAL][SA_UNARYMINUS] = PT_LESS;
	precedenceTable[SA_TERMINAL][SA_POWER] = PT_LESS;
	precedenceTable[SA_TERMINAL][SA_LBRACE] = PT_LESS;
	precedenceTable[SA_TERMINAL][SA_IDENTIFIER] = PT_LESS;
	precedenceTable[SA_TERMINAL][SA_DATATYPE] = PT_LESS;
	//PRECEDENCE TABLE END
}

//Fce urcujici typ terminalu na zaklade tokenu
int getTermType(char *value, int type) {
	if (type == IDENTIFIER) return SA_IDENTIFIER;
	else if (type == STRING || type == LOGIC || type == INTEGER || type == REAL || type == NIL)
		return SA_DATATYPE;
	//reprezentance tokenu ze vstupu
	else if (!strcmp(value, "==")) return SA_EQUAL;
	else if (!strcmp(value, "~=")) return SA_NOTEQUAL;
	else if (!strcmp(value, "<=")) return SA_LESSEQUAL;
	else if (!strcmp(value, ">=")) return SA_MOREEQUAL;
	else if (!strcmp(value, "<")) return SA_LESS;
	else if (!strcmp(value, ">")) return SA_MORE;
	else if (!strcmp(value, "..")) return SA_CONCATE;
	else if (!strcmp(value, "+")) return SA_ADD;
	else if (!strcmp(value, "-")) return SA_SUB;
	else if (!strcmp(value, "*")) return SA_MUL;
	else if (!strcmp(value, "/")) return SA_DIV;
	else if (!strcmp(value, "%")) return SA_MODULO;
	else if (!strcmp(value, "#")) return SA_LENGTH;
	else if (!strcmp(value, "^")) return SA_POWER;
	else if (!strcmp(value, "(")) return SA_LBRACE;
	else if (!strcmp(value, ")")) return SA_RBRACE;
	else return SA_TERMINAL;
}
//Fce inicializuje zasobnik(nastavi vrchol na -1 a alokuje pro nej pamet)
bool stackInit (tStack* s) {
	s->stackSize = STACK_INIT_SIZE;
	s->top = -1;
	s->arr = malloc(s->stackSize*sizeof(tData *));
	if (s->arr == NULL)
		return false;
	return true;
}

//Fce vraci true pokud je zasobnik prazdny, jinak vraci false
bool stackEmpty (tStack* s) {
	return ((s->top == -1) ? true : false);
}

//Fce vraci true pokud je zasobnik plny, jinak vraci false
bool stackFull (tStack* s) {
	return ((s->top == s->stackSize-1) ? true : false);
}

//Funkce nakopiruje do struktury predane odkazem data struktury na vrcholu zasobniku
void stackTop (tStack* s, tData *data) {
	if(!stackEmpty(s)) {
		//kopiruje hodnoty
		data->entry = (s->arr[s->top])->entry;
		data->expr->type = (s->arr[s->top])->expr->type;
		data->expr->integer = (s->arr[s->top])->expr->integer;
		data->expr->boolean = (s->arr[s->top])->expr->boolean;
		data->expr->real = (s->arr[s->top])->expr->real;
		strCopyString(data->expr->stringVar, (s->arr[s->top])->expr->stringVar);
		}
	else
		fprintf(stderr, "Error ve fci stackTop.\n");//zasobnik je prazdny => chyba
}

//Funkce analogicka ke stackTop, ovsem pracuje pouze s ukazatelem
void stackTopPtr(tStack *stack, tData *data) {
	data->entry = (stack->arr[stack->top])->entry;
	data->expr = (stack->arr[stack->top])->expr;
}

//Funkce odstrani prvek z vrcholu zasobniku
void stackPop (tStack* s) {
	if (!stackEmpty(s)) {
		free((s->arr[s->top])->expr->stringVar->str);
		free((s->arr[s->top])->expr->stringVar);
		free((s->arr[s->top])->expr);
		free(s->arr[s->top]);
		s->top--;
	}
}

//Funkce analogicka ke stackPop, ale pracuje pouze s pointery -> nic neuvolnuje
void stackPopPtr(tStack *stack) {
	if (!stackEmpty(stack)) {
		free(stack->arr[stack->top]);
		stack->arr[stack->top] = NULL;
		stack->top--;
	}
}

//Funkce vlozi prvek na vrchol zasobniku
bool stackPush (tStack* s, tData *newData) {
	if (!stackFull(s)) {
		if (newData->entry == SA_IDENTIFIER) {
			if (!stackPushPtr(s, newData))
				return false;
			else
				return true;
		}
		tData *data = NULL;
		tVarValue *expr = NULL;
		//naalokuje kompletni strukturu tData
		if ((data = malloc(sizeof(tData))) == NULL)
			return false;
		if ((expr = malloc(sizeof(tVarValue))) == NULL) {
			free(data);
			return false;
		}
		if ((expr->stringVar = malloc(sizeof(string))) == NULL) {
			free(expr);
			free(data);
			return false;
		}
		if (strInit(expr->stringVar)) {
			free(expr->stringVar);
			free(expr);
			free(data);
			return false;
		}
		data->expr = expr;
		//naplni naalokovanou hodnotu
		data->entry = newData->entry;
		if (newData->expr != NULL) {
			data->expr->type = newData->expr->type;
			data->expr->integer = newData->expr->integer;
			data->expr->real = newData->expr->real;
			strCopyString(data->expr->stringVar, newData->expr->stringVar);
			data->expr->boolean = newData->expr->boolean;
		}
		//vlozi data na zasobnik
		s->top++;
		s->arr[s->top] = data;
		//a prida ukazatel na data do pointerTrashe
		pointerTrashAdd(pointerTrashPtr, data->expr);
	}
	else {//v pripade ze neni dost mista, realloknem si dalsi a pak pushnem
		tData **helpPtr = NULL;
		s->stackSize *= 2;
		helpPtr = realloc(s->arr, s->stackSize*sizeof(tData *));
		if (helpPtr == NULL) {
			return false;
		}
		s->arr = helpPtr;
		if(!stackPush(s, newData))
			return false;
	}
	return true;
}

//Fce analogicka k stackPush, akorat pracuje pouze s ukazatelem
bool stackPushPtr(tStack *stack, tData *data) {
	//pokud zasobnik neni plny vlozi ukazatel na vrchol
	if (!stackFull(stack)) {
		tData *newData;
		if ((newData = malloc(sizeof(tData))) == NULL)
			return false;
		newData->entry = data->entry;
		newData->expr = data->expr;
		stack->top++;
		stack->arr[stack->top] = newData;
	}
	else {//v pripade ze neni dost mista, realloknem si dalsi a pak pushnem
		tData **helpPtr = NULL;
		stack->stackSize *= 2;
		helpPtr = realloc(stack->arr, stack->stackSize*sizeof(tData *));
		if (helpPtr == NULL) {
			return false;
		}
		stack->arr = helpPtr;
		if(!stackPushPtr(stack, data))
			return false;
	}
	return true;
}

//Fce na zniceni zasobniku
void stackDestroy(tStack *s) {
	if (s != NULL) {
		for (int i = s->top; i > -1; i--)
			free(s->arr[i]);
		free(s->arr);
		free(s);
	}
}

//Fce porovna dve integrova pole delky 3 a vraci true pokud se shoduji nebo
//vrati false pokud se neshoduji
bool compareIntArrays(int *array1, int *array2) {
	for (int i = 0; i < 3; i++) {
		if (array1[i] != array2[i])
			return false;
	}
	return true;
}

//Fce pro overeni pravidla, vraci cislo pravidla ktere se ma pouzit nebo -1
//pri neuspechu
int generateRule(int *rule) {
	//porovnani vstupniho pravidla s vyse definovanymi
	if (compareIntArrays(rule, rule39)) return 39;
	else if (compareIntArrays(rule, rule40)) return 40;
	else if (compareIntArrays(rule, rule41)) return 41;
	else if (compareIntArrays(rule, rule42)) return 42;
	else if (compareIntArrays(rule, rule43)) return 43;
	else if (compareIntArrays(rule, rule44)) return 44;
	else if (compareIntArrays(rule, rule45)) return 45;
	else if (compareIntArrays(rule, rule46)) return 46;
	else if (compareIntArrays(rule, rule47)) return 47;
	else if (compareIntArrays(rule, rule48)) return 48;
	else if (compareIntArrays(rule, rule49)) return 49;
	else if (compareIntArrays(rule, rule50)) return 50;
	else if (compareIntArrays(rule, rule51)) return 51;
	else if (compareIntArrays(rule, rule52)) return 52;
//	else if (compareIntArrays(rule, rule53)) return 53;
	else if (compareIntArrays(rule, rule54)) return 54;
	else if (compareIntArrays(rule, rule55)) return 55;
	else if (compareIntArrays(rule, rule56)) return 56;
	else return -1;
}

//Fce pro naplneni struktury tVarValue odpovidajicimi hodnotami, predanymi jako
//argument. @param1 expr k naplneni, @param2 typ promenne, @param3 hodnota
//integerove promenne, @param4 hodnota realne promenne, @param5 hodnota stringove
//promenne, @param6 hodnota logicke promenne
void fillExpr(tVarValue *expr, int type, int integer, double real,
				  string *stringVar, bool boolean) {
	expr->type = type;
	if (type == T_INTEGER) expr->real = integer;
	else if (type == T_REAL) expr->real = real;
	else if (type == T_STRING) strCopyString(expr->stringVar, stringVar);
	else if (type == T_BOOLEAN) expr->boolean = boolean;
	else if (type == T_NIL);
}

//Fce ktera vraci hodnotu nejvyssiho terminalu na zasobniku
int getTopTerm(tStack *s) {
	int helper = s->top;
	while ((s->arr[helper])->entry == SA_EXPR || (s->arr[helper])->entry == SA_PT_LESS) {
		helper--;
	}
	return (s->arr[helper])->entry;
}

//Fce ktera naplni ze ziskaneho tokenu strukturu tData
bool fillData(string *attr, int *type, FILE *file, tData *data) {
	data->entry = getTermType(attr->str, *type);

	if (data->entry == SA_DATATYPE) {
		if (*type == STRING)
			fillExpr(data->expr, T_STRING, 0, 0, attr, false);
		else if (*type == LOGIC)
			fillExpr(data->expr, T_BOOLEAN, 0, 0, (string *) NULL,
					 (strcmp(attr->str, "true")) ? false : true);
		else if (*type == REAL)
			fillExpr(data->expr, T_REAL, 0, strtod(attr->str, NULL), (string *) NULL, false);
		else if (*type == INTEGER)
			fillExpr(data->expr, T_INTEGER, atoi(attr->str), 0, (string *) NULL, false);
		else if (*type == NIL)
			fillExpr(data->expr, T_NIL, 0, 0, (string *) NULL, false);
	}
	else if (data->entry == SA_IDENTIFIER) {
		/**volam table handlera(snad jiz funkcniho :)) a pak plnim expr*/
		control.operation = O_FETCH_VALUE;
		strCopyString(controlHelper->identif, attr);
		control.variable = controlHelper->identif;
		if (tableHandler(&STable, &control) != TREE_SUCCESS) {
			AddError(line, SemErr);
			return false;
    }
    data->expr = control.STitem;
    control.variable = controlHelper->variable;
	}
	return true;
}

//Fce presype obsah zasobniku @param1 na zasobnik @param2 do prvniho terminalu,
//vyuziva pomocnou promennou @param3
bool transferDataToTerm(tStack *src, tStack *dest, tData *helper) {
	while ((src->arr[src->top])->entry == SA_EXPR ||
		   (src->arr[src->top])->entry == SA_PT_LESS) {
		helper->entry = 0;
		helper->expr = NULL;
		stackTopPtr(src, helper);
		if (!stackPushPtr(dest, helper))
			return false;
		stackPopPtr(src);
	}
	return true;
}

//Fce presype veskery obsah zasobniku @param1 na zasobnik @param2, vyuziva
//pomocnou promennou @param3
bool transferDataAll(tStack *src, tStack *dest, tData *helper)	{
	while (!stackEmpty(src)) {
		stackTopPtr(src, helper);
		if (!stackPushPtr(dest, helper))
			return false;
		stackPopPtr(src);
	}
	return true;
}

//Fce precte pravidlo ze zasobniku. Pouziva k tomu pomocny zasobnik a pomocnou
//strukturu pro terminal. Pokud identifikuje pravidlo, vygeneruje instrukci a
//ulozi vysledek do pomocne struktury, ktera se pak muze pushnout na mainStack.
//Pokud je pravidlo delsi jak tri je to chyba!
int determRuleOnStack(tStack *mainStack, tStack *helperStack, tData *helperTerminal) {
	while ((mainStack->arr[mainStack->top])-> entry != SA_PT_LESS) {
		stackTopPtr(mainStack, helperTerminal);
		if (!stackPushPtr(helperStack, helperTerminal))
			return -2;
		stackPopPtr(mainStack);
	}
	if (helperStack->top > 2)//pravidlo je delsi jak 3
		return -1;
	int ruleOnStack[3] = {0, 0, 0};
	for (int i = helperStack->top; i > -1; i--)
		ruleOnStack[i] = (helperStack->arr[i])->entry;
	return generateRule(ruleOnStack);
}

//Fce pro init struktury tVarValue
void initData(tData *data) {
	data->expr->type = 0;
	data->expr->integer = 0;
	data->expr->real = 0.0;
	data->expr->boolean = false;
	data->entry = 0;
}

//Fce pro uvolneni struktury tData
//ocekava tData * a tVarValue *, aby bylo mozne uvlonovat i samotny expr
void freeData(tData *data, tVarValue *expr) {
	if (expr != NULL) {
		if (expr->stringVar != NULL) {
			strFree(expr->stringVar);
			free(expr->stringVar);
		}
		free(expr);
	}
	free(data);
}

//Fce vytvarejici novou polozku typu tVarValue, prida ukazatel na ni do
//pointerTrashe.
//EXPECTED :: ukazatel na kos, reference na ukazatel cilove adresy
bool exprAlloc(tPointTrashPtr trash, p_VarValue *expr) {

	if ((*expr = malloc(sizeof(tVarValue))) == NULL) {
	//"Do nothing, unsuccessfully."
	}
	else if (((*expr)->stringVar = malloc (sizeof(string))) == NULL) {
		free(*expr);
	}
	else if (strInit((*expr)->stringVar)) {
		free((*expr)->stringVar);
		free(*expr);
	}
	else {//vse probehlo v poradku, inicializuje strukturu
		(*expr)->type = T_NIL;
		(*expr)->integer = 0;
		(*expr)->real = 0;
		(*expr)->boolean = false;
		//prida polozku do pointerTrashe
		if (!pointerTrashAdd(trash, *expr)) {
		//polozku se nezdarilo pridat do kose
			strFree((*expr)->stringVar);
			free((*expr)->stringVar);
			free(*expr);
		}
		else
			return true;//Vse OK
	}

	//alokace se nezdarila
	*expr = NULL;
	return false;
}

//Obalovaci funkce, ktera na zaklade pravidla prida instrukci do instrukcniho
//listu pomoci InstCreate()
//EXPECTED :: pravidlo, ukazatel na destination, zasobnik obsahujici zdrojove op
bool generateInstruction(int rule, tVarValue *dest, tStack *stack) {
	//pomocne promenne pro uchovani adres operadnu
	tVarValue *leftOP = NULL;
	tVarValue *rightOP = NULL;

	switch (rule) {
		case 52:
			leftOP = (stack->arr[stack->top - 1])->expr;
			break;
		case 54:
		case 56:
			rightOP = (stack->arr[stack->top])->expr;
			break;
		default:
			leftOP = (stack->arr[stack->top])->expr;//left
			rightOP = (stack->arr[stack->top - 2])->expr;//right
	}

	switch (rule) {
		case 39:
			if (MakeInst(I_EQUAL, dest, leftOP, rightOP, 0))
				return false;
			break;
		case 40:
			if (MakeInst(I_NOT_EQUAL, dest, leftOP, rightOP, 0))
				return false;
			break;
		case 41:
			if (MakeInst(I_LESS_EQUAL, dest, leftOP, rightOP, 0))
				return false;
			break;
		case 42:
			if (MakeInst(I_MORE_EQUAL, dest, leftOP, rightOP, 0))
				return false;
			break;
		case 43:
			if (MakeInst(I_MORE, dest, leftOP, rightOP, 0))
				return false;
			break;
		case 44:
			if (MakeInst(I_LESS, dest, leftOP, rightOP, 0))
				return false;
			break;
		case 45:
			if (MakeInst(I_CONCATE, dest, leftOP, rightOP, 0))
				return false;
			break;
		case 46:
			if (MakeInst(I_ADD, dest, leftOP, rightOP, 0))
				return false;
			break;
		case 47:
			if (MakeInst(I_SUB, dest, leftOP, rightOP, 0))
				return false;
			break;
		case 48:
			if (MakeInst(I_MODULO, dest, leftOP, rightOP, 0))
				return false;
			break;
		case 49:
			if (MakeInst(I_MULTIPLY, dest, leftOP, rightOP, 0))
				return false;
			break;
		case 50:
			if (MakeInst(I_DIVIDE, dest, leftOP, rightOP, 0))
				return false;
			break;
		case 51:
			if (MakeInst(I_POWER, dest, leftOP, rightOP, 0))
				return false;
			break;
		case 52:
			if (MakeInst(I_LENGHT, dest, leftOP, rightOP, 0))
				return false;
			break;
		case 53://Unarni minus -- nikdy k tomuto nemuze dojit
			break;
		case 55://nikdy by k nemu nemelo dojit, jedna se o redukci zavorek E => (E)
		case 54://nikdy by k nemu nemelo dojit, jedna se o redukci E => vid
			break;
		case 56:
			fillExpr(dest, rightOP->type, rightOP->real, rightOP->real, rightOP->stringVar,
				rightOP->boolean);
	}//switch (rule)

	return true;
}

bool precedenceSA(string *attr, int *type, FILE *file, bool write) {
	//promenna ktera bude ridit switch, nabyva hodnoty zaznamu precedenci tabulky
	//vyhledaneho pomoci klice [terminal na zasobniku, terminal na vstup]
	int determineAction = PT_NULL;
	//promenna do ktere se zapise jake pravidlo ma byt vykonano
	int rule = -1;
	bool errflg = false;



	//priprava hlavni struktury pro ukladani vstupniho tokenu
	tVarValue *expr;
	tData *mainTerminal;
	if ((expr = malloc(sizeof(tVarValue))) == NULL) {
		AddError(line, ProgErr);
		return false;
	}
	if ((expr->stringVar = malloc(sizeof(string))) == NULL) {
		freeData(NULL, expr);
		AddError(line, ProgErr);
		return false;
	}
	if (strInit(expr->stringVar)) {
		freeData(NULL, expr);
		AddError(line, ProgErr);
		return false;
	}
	if ((mainTerminal = malloc(sizeof(tData))) == NULL) {
		freeData(NULL, expr);
		AddError(line, ProgErr);
		return false;
	}
	mainTerminal->expr = expr;
	initData(mainTerminal);

	//priprava pomocne struktury -- ukladaji se do ni jenom ukazatele
	tData helperTerminal = {0, NULL};

	//pripraveni hlaviho zasobniku
	tStack *mainStack;
	if ((mainStack = malloc(sizeof(tStack))) == NULL) {
		freeData(mainTerminal, expr);
		AddError(line, ProgErr);
		return false;
	}
	if (!stackInit(mainStack)) {
		freeData(mainTerminal, expr);
		free(mainStack);
		AddError(line, ProgErr);
		return false;
	}
	helperTerminal.entry = SA_TERMINAL;
	if (!stackPush(mainStack, &helperTerminal)) {
		freeData(mainTerminal, expr);
		stackDestroy(mainStack);
		AddError(line, ProgErr);
		return false;
	}

	//pripraveni pomocneho zasobniku
	tStack *helperStack;
	if ((helperStack = malloc(sizeof(tStack))) == NULL) {
		freeData(mainTerminal, expr);
		stackDestroy(mainStack);
		AddError(line, ProgErr);
		return false;
	}
	if (!stackInit(helperStack)) {
		freeData(mainTerminal, expr);
		stackDestroy(mainStack);
		free(helperStack);
		AddError(line, ProgErr);
		return false;
	}

	//init precedencni tabulky
	precedenceTableInit(precedenceTable);

	fillData(attr, type, file, mainTerminal);
	//jadro precedencni analyzy
	while (!(mainTerminal->entry == SA_TERMINAL && getTopTerm(mainStack) == SA_TERMINAL)
				 && !errflg) {

		/** write je ukoncen ) coz je ale zaroven znak pouzivany ve vyrazech!! Nasledujici
		kod definuje vyjimecny stav, kdy je ) povazovana za konec precedencni anaylzou
		zpracovavaneho vstupu*/
		if (write && getTopTerm(mainStack) == SA_TERMINAL && mainTerminal->entry
						== SA_RBRACE)
					break;
		determineAction = precedenceTable[getTopTerm(mainStack)][mainTerminal->entry];
		switch (determineAction) {
		//podle toho jaky symbol obsahuje chlivek precedencni tabulky:
			case PT_LESS:// <
				//odebereme prvky do nejhornejsiho terminalu na mainStacku
				if (!transferDataToTerm(mainStack, helperStack, &helperTerminal)) {
					AddError(line, ProgErr);
					errflg = true;
					continue;
				}
				//vlozime za nej '<'
				helperTerminal.expr = NULL;
				helperTerminal.entry = SA_PT_LESS;
				if (!stackPush(mainStack, &helperTerminal)) {
					AddError(line, ProgErr);
					errflg = true;
					continue;
				}
				//vratime zpet co jsme odebrali
				if (!transferDataAll(helperStack, mainStack, &helperTerminal)) {
					AddError(line, ProgErr);
					errflg = true;
					continue;
				}
				//vlozime aktulani terminal ze vstupu na zasobnik
				if (!stackPush(mainStack, mainTerminal)) {
					AddError(line, ProgErr);
					errflg = true;
					continue;
				}
				if (getNextToken(attr, type, file))
					errflg = true;
				break;
			case PT_MORE:// >
				//vygenerujeme pravidlo
				rule = determRuleOnStack(mainStack, helperStack, &helperTerminal);
				if(rule == -1) {
					AddError(line, SynErr);
					errflg = true;
					continue;
				}
				else if(rule == -2) {
					AddError(line, ProgErr);
					errflg = true;
					continue;
				}
				else {
					//odebere prvek '<' z mainStacku
					stackPopPtr(mainStack);
					if (rule != 55) {
					//pravidlo 55: E = (E) -- v promenne intermResult je ukazatel na
					//spravnou hodnotu, takze ji zpatky vratime na mainStack a vycistime
					//helperStack
						if (rule != 54) {
						//jedna se o redukci E => vid -- "mezivysledek" jiz je ulozen v
						//tabulce symbolu, neni tedy treba mu alokovat novy prostor
							if (!exprAlloc(pointerTrashPtr, &intermResult)) {
								AddError(line, ProgErr);
								errflg = true;
								continue;
							}
							//zavola obalovaci fci, ktera na zaklade redukcniho pravidla vola
							//InstCreate()
							if (!generateInstruction(rule, intermResult, helperStack)) {
								errflg = true;
								continue;
							}
						}
						else//if (rule != 54)
						//do intermResult se priradi adresa do STable
							intermResult = (helperStack->arr[helperStack->top])->expr;
					}

					//vlozi ukazatel na vypocitanou hodnotu zpatky na mainStack
					helperTerminal.entry = SA_EXPR;
					helperTerminal.expr = intermResult;

					//******************************************************************//
					if (!stackPushPtr(mainStack, &helperTerminal)) {
						AddError(line, ProgErr);
						errflg = true;
						continue;
					}

					while (!stackEmpty(helperStack))
						stackPopPtr(helperStack);
				}
				break;
			case PT_EQUAL:// =
				if (!stackPush(mainStack, mainTerminal)) {
					AddError(line, ProgErr);
					errflg = true;
					continue;
				}
				if (getNextToken(attr, type, file))
					errflg = true;
				break;
			case PT_NULL:
				errflg = true;
				break;
		}//switch (determineAction)

		//prevence ztraceni ukazatele na struktury mainTerminal
		mainTerminal->expr = expr;
		//naplnime data struktury mainTerminal, novym terminalem ze vstupu
		fillData(attr, type, file, mainTerminal);
	}

	//uvolneni pouzitych promennych
	stackDestroy(mainStack);
	stackDestroy(helperStack);
	freeData(mainTerminal, expr);
	return (errflg) ? false : true;
}

bool pointerTrashInit(tPointTrashPtr *trash) {
	if ((*trash = malloc(sizeof(struct PointerTrash))) == NULL)
		return false;
	if (((*trash)->arr = malloc(STACK_INIT_SIZE * sizeof(tVarValue *))) == NULL) {
		free(*trash);
		*trash = NULL;
		return false;
	}
	(*trash)->size = STACK_INIT_SIZE;
	(*trash)->top = 0;
	return true;
}

bool pointerTrashAdd(tPointTrashPtr trash, tVarValue *item) {
	//pokud kos neni plny, prida polozku
	if (trash->top != trash->size - 1) {
		trash->arr[trash->top] = item;
		trash->top++;
	}
	else {
	//jinak kos zvetsi
		tVarValue **helperArr;
		trash->size *= 2;
		if ((helperArr = realloc(trash->arr, sizeof(tVarValue *) * trash->size)) == NULL) {
		//zvetseni kose se nepodarilo => nedela nic, indikuje chybu
			trash->size /= 2;
			return false;
		}
		else {
		//kos je zvetsen, nyni lze polozku pridat
			trash->arr = helperArr;
			pointerTrashAdd(trash, item);
		}
	}
	return true;
}

void pointerTrashDump(tPointTrashPtr *trash) {
	if (*trash != NULL) {
		for (int i = (*trash)->top - 1; i > -1; i--) {
			if ((*trash)->arr[i] != NULL) {
				if ((*trash)->arr[i]->stringVar != NULL)
					free((*trash)->arr[i]->stringVar->str);
				free((*trash)->arr[i]->stringVar);
				free((*trash)->arr[i]);
			}
		}
		free((*trash)->arr);
		free(*trash);
		*trash = NULL;
	}
}
