/* **************************** parser.c ************************************ */
/* Soubor:              parser.c - Syntakticka analyza                        */
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

#include "parser.h"
//promenna signalizujici vyskyt mainu
bool mainFlag = false;
//rozhrani parser - handler
tTreeCont *controlHelper, control = {0, 0, NULL, NULL, NULL, NULL, NULL};
//pomocne pole argumentArray -- jedna polozka == jedna deklarovana funkce
tFuncArgPtr funcsArgs = NULL;
tVarValue *functionReturn;


//funkce pro nonterminal <expressionNext>
bool expressionNext(string *attr, int *type, FILE *file) {
	//rozvoj dle 28. pravidla
	if (!strcmp(attr->str, ",")) {//,
		if ((getNextToken(attr, type, file)) ||
		!precedenceSA(attr, type, file, true))//<expression>
			return false;
		//vytvori instrukci vypsani vysledku vyrazu
		if (MakeInst(I_WRITE, intermResult, NULL, NULL, 0))
			return false;
		if (!expressionNext(attr, type, file))//<expression_n>
			return false;
	}
	else//pravidlo 29.(epsilon pravidlo)
		return true;
	return true;
}

//funkce pro nonterminal <expression list>
bool expressionList(string *attr, int *type, FILE *file) {
	if ((getNextToken(attr, type, file)))
		return false;

	//pravidlo 26.
	if (*type == STRING || *type == INTEGER ||	*type == LOGIC || *type == NIL ||
	*type == REAL || *type == IDENTIFIER || !strcmp(attr->str, "(") ||
	!strcmp(attr->str, "#")) {
		if (!precedenceSA(attr, type, file, true))//<expression>
			return false;

		//vytvori instrukci vypsani vysledku vyrazu
		if (MakeInst(I_WRITE, intermResult, NULL, NULL, 0))
			return false;

		if (!expressionNext(attr, type, file))//<expression_n>
			return false;
	}
	else//pravidlo 27.(epsilon pravidlo)
		return true;
	return true;
}

//funkce pro nonterminal <input>
bool input(string *attr, int *type, FILE *file) {
	if (getNextToken(attr, type, file))
		return false;
	//vytvori si "mezivysledek" do ktereho se ulozi argument volani read
	if (!exprAlloc(pointerTrashPtr, &exprResult)) {
		AddError(line, ProgErr);
		return false;
	}
	//vytvori si "mezivysledek" do ktereho se ulozi co bylo nacteno
	if (!exprAlloc(pointerTrashPtr, &intermResult)) {
		AddError(line, ProgErr);
		return false;
	}

	switch (*type) {//dle typu parametru volani read ulozi do mezivysledku jeho hodnotu
		case STRING:
			exprResult->type = T_STRING;
			if (strCopyString(exprResult->stringVar, attr)) {
				AddError(line, ProgErr);
				return false;
			}
			break;
		case INTEGER:
			exprResult->type = T_INTEGER;
			exprResult->real = atoi(attr->str);
			break;
		case REAL:
			exprResult->type = T_REAL;
			exprResult->real = strtod(attr->str, NULL);
			break;
		default:
			return false;
	}

	//zavola vytvoreni instrukce
	if (MakeInst(I_READ, intermResult, exprResult, NULL, 0))
		return false;

	return true;
}

//funkce pro nonterminal <param_n>
bool paramNext(string *attr, int *type, FILE *file) {
	//pravidlo 35.
	if (!strcmp(attr->str, ",")) {//,
		if (getNextToken(attr, type, file) ||
		!param(attr, type, file, false) ||//<param>
		!paramNext(attr, type, file)//<param_n>
		)
			return false;
	}
	else//pravidlo 36.(epsilon pravidlo)
		return true;
	return true;
}

//funkce pro nonterminal <param>
bool param(string *attr, int *type, FILE *file, bool minusFlag) {
	//pravidlo 37.
	if (*type == STRING || *type == INTEGER || *type == LOGIC ||
	*type == REAL || *type == NIL) {
		if (!factor(attr, type, file, INIT_FALSE, minusFlag))
			return false;
	}
	//pravidlo 38.
	else if (*type == IDENTIFIER) {
		if (validateParamNumber()) {
		//osetreni predani vice parametru nez bylo deklarovano
			//prvne ziskam ukazatel na polozku tVarValue ze stromu promennych
			control.operation = O_FETCH_VALUE;
			if (strCopyString(controlHelper->variable, attr)) {
				AddError(line, ProgErr);
				return false;
			}
			//ziskani adresy zdrojove promenne
			control.variable = controlHelper->variable;
			if (tableHandler(&STable, &control) == TREE_FAIL) {//ziska hodnotu parametru
				AddError(line, SemErr);
				return false;
			}
			//vytvorim zalohu STitem
			controlHelper->STitem = control.STitem;

			if (!funcArgFetch()) {//zjisti nazev argumentu do control.varialbe
				AddError(line, ProgErr);
				return false;
			}
			//vygeneruje instrukci, ktera priradi hodnotu parametru(volani) do spravneho
			//argumentu(deklarace) funkce
			//ziska adresu cilove promenne -> volana fce, identifikator promenne
			control.identif = controlHelper->function;
			control.function = controlHelper->identif;
			if (tableHandler(&STable, &control) == TREE_FAIL) {//ziska hodnotu parametru
				AddError(line, SemErr);
				return false;
			}
			//vrati zpatky smurf v control
			control.identif = controlHelper->identif;
			control.function = controlHelper->function;
			if (strCmpString(control.function, control.identif)) {
			//pokud se nejedna o rekurzi
				//vygeneruje instrukci prirazeni spravne hodnoty do argumentu
				if (MakeInst(I_ARG_ASSIGN, control.STitem, controlHelper->STitem, NULL, 0))
					return false;
			}
			else {//o rekruzi se nejedna
				tVarValue *helperName;
				if (!exprAlloc(pointerTrashPtr, &helperName)) {
					AddError(line, ProgErr);
					return false;
				}
				if (strCopyString(helperName->stringVar, attr)) {
					AddError(line, ProgErr);
					return false;
				}
				if (MakeInst(I_REC_ARG_ASSIGN, control.STitem, helperName->stringVar, NULL, 0))
					return false;
			}
		}
		if (getNextToken(attr, type, file))
			return false;
	}
	else if (!strcmp("-", attr->str)) {
		if (minusFlag)
			return false;
		if (getNextToken(attr, type, file) || !param(attr, type, file, true))
			return false;
		return true;
	}
	if (strCopyString(control.variable, controlHelper->varData->stringVar)) {
		AddError(line, ProgErr);
		return false;
	}
	control.paramCounter++;
	return true;
}

//funkce pro nonterminal <param list>
bool paramList(string *attr, int *type, FILE *file) {
	//pravidlo 33.
	if (*type == STRING || *type == INTEGER || *type == LOGIC ||
	*type == REAL || *type == IDENTIFIER || *type == NIL) {
		if (!param(attr, type, file, false) ||//<param>
		!paramNext(attr, type, file)//<param_n>
		)
			return false;
	}
	else if (!strcmp(attr->str, "-")) {
		if (getNextToken(attr, type, file) ||
        !param(attr, type, file, true) ||//<param>
		!paramNext(attr, type, file)//<param_n>
		)
			return false;
	}
	else//pravidlo 34.(epsilon pravidlo)
		return true;
	return true;
}

//funkce pro nonterminal <assignment>
bool assignment(string *attr, int *type, FILE *file) {
	if (getNextToken(attr, type, file))
		return false;

	//pravidlo 23.
	if (!strcmp(attr->str, "read")) {//read
		if (getNextToken(attr, type, file) ||
		strcmp(attr->str, "(") ||//(
		!input(attr, type, file) ||//<input>
		getNextToken(attr, type, file) ||
		strcmp(attr->str, ")") ||//)
		getNextToken(attr, type, file)
		)
			return false;
	}

	else if (*type == IDENTIFIER) {
		int idType;
		control.operation = O_IDENTIFY_ID;
		//ulozime identifikator do control.identif
		if (strCopyString(controlHelper->identif, attr)) {
			AddError(line, ProgErr);
			return false;
		}
		control.identif = controlHelper->identif;
		idType = tableHandler(&STable, &control);

		if (idType == T_FID) {//fid
			//alokuje promennou pro ulozeni vysledku
			if (!exprAlloc(pointerTrashPtr, &functionReturn)) {
				AddError(line, ProgErr);
				return false;
			}
			//alokuje promennou, kde bude ulozen identifikator volane fce
			if (!exprAlloc(pointerTrashPtr, &exprResult)) {
				AddError(line, ProgErr);
				return false;
			}
			//do nove alokovane promenne ulozi identif volane fce
			if (strCopyString(exprResult->stringVar, attr)) {
				AddError(line, ProgErr);
				return false;
			}
			//vytvori strukturu pro ulozeni nazvu volajici fce a jeji id tam ulozi
			if (!exprAlloc(pointerTrashPtr, &intermResult)) {
				AddError(line, ProgErr);
				return false;
			}
			//do nove alokovane promenne ulozi identif volajici fce
			if (strCopyString(intermResult->stringVar, control.function)) {
				AddError(line, ProgErr);
				return false;
			}
			//vytvorim instrukci pro volani fce
			if (MakeInst(I_CALL_FUNC, functionReturn, exprResult->stringVar, intermResult->stringVar, 0))
				return false;
			//printf("I_CALL_FUNC: %s FROM FUNCTION: %s\n", exprResult->stringVar->str, intermResult->stringVar->str);

			//pravidlo 24.
			if (getNextToken(attr, type, file) ||
			strcmp(attr->str, "(") ||//(
			getNextToken(attr, type, file) ||
			!paramList(attr, type, file) ||//<param list>
			strcmp(attr->str, ")") ||//)
      getNextToken(attr, type, file)
			)
				return false;

			control.STitem = NULL;
			control.paramCounter = 0;
			//tVarValue *mothafaka = intermResult;
			//printf("\n\nPred zmenou: %g", mothafaka->real);
			intermResult = functionReturn;
			//printf("\n\nPo zmene: %g", mothafaka->real);
		}

		else if (idType == T_VID) {//vid
			if (!precedenceSA(attr, type, file, false))
				return false;
		}
		else {
			if (ErrorFlag != ProgErr && ErrorFlag != LexErr)
				AddError(line, SemErr);
			return false;
		}
	}
	else if  (*type == STRING || *type == INTEGER ||	*type == LOGIC ||
		*type == REAL || !strcmp(attr->str, "(") || !strcmp(attr->str, "#") ||
		*type == NIL) {
			if (!precedenceSA(attr, type, file, false))
				return false;
	}
	else
		return false;

	control.variable = controlHelper->variable;
	control.operation = O_FETCH_VALUE;
	if (tableHandler(&STable, &control) == TREE_FAIL) {
		AddError(line, SemErr);
		return false;
	}
	//printf("\n\nS timhle volam FETCH_VALUE: vid -- %s a fid -- %s\n\n", control.variable->str, control.function->str);
	//vygenerujeme instrukci prirazeni
	if (MakeInst(I_ASSIGN, control.STitem, intermResult, NULL, 0))
		return false;
	//printf("I_ASSIGN\n");
	return true;
}

//funkce pro nonterminal <statement>
bool statement(string *attr, int *type, FILE *file) {
	//pravidlo 17.
	if (*type == IDENTIFIER) {//vid
		//zkontroluje, zda se jedna o platny identifikator
		control.identif = controlHelper->identif;
		if (strCopyString(controlHelper->identif, attr)) {
			AddError(line, ProgErr);
			return false;
		}

		//pokud STable handler nevrati T_VID => semanticky error
		control.operation = O_IDENTIFY_ID;
		if (tableHandler(&STable, &control) != T_VID) {
			AddError(line, SemErr);
			return false;
		}
		//zkopiruje si VID do control.variable, aby pak mohl fetchnout jeho tVarValue
		control.variable = controlHelper->variable;
		if (strCopyString(controlHelper->variable, attr)) {
			AddError(line, ProgErr);
			return false;
		}

		//udela maly smurf, potrebuje si zachovat nazev promenne kam se bude prirazovat
		if(strCopyString(controlHelper->varData->stringVar, attr)) {
			AddError(line, ProgErr);
			return false;
		}

		if (getNextToken(attr, type, file) ||
		strcmp(attr->str, "=") ||//=
		!assignment(attr, type, file)//<assignment>
		)
			return false;
	}
	//pravidlo 18.
	else if (!strcmp(attr->str, "write")) {//write
		if (getNextToken(attr, type, file) ||
		strcmp(attr->str, "(") ||//(
		!expressionList(attr, type, file) ||//<expression list>
		strcmp(attr->str, ")") ||//)
		getNextToken(attr, type, file)
		)
			return false;
	}
	//pravidlo 19.
	else if (!strcmp(attr->str, "return")) {//return
		if (getNextToken(attr, type, file) ||
		!precedenceSA(attr, type, file, false)//<expression>
		)
			return false;
		//generuje instrukci pro volani return
		if (MakeInst(I_RETURN, intermResult, NULL, NULL, 0))
			return false;
		//printf("I_RETURN\n");
	}
	//pravidlo 20.
	else if (!strcmp(attr->str, "if")) {//if
		if (getNextToken(attr, type, file) ||
		MakeInst(I_IF, NULL, NULL, NULL, 0) ||//vygeneruje dummy instrkuci if
		!precedenceSA(attr, type, file, false) ||//<expression>
		strcmp(attr->str, "then") ||//then
		MakeInst(I_THEN, intermResult, NULL, NULL, 0) ||//instrukce pro then
		getNextToken(attr, type, file) ||
		!statementList(attr, type, file) ||//<statement list>
		strcmp(attr->str, "else") ||//<else>
		MakeInst(I_ELSE, NULL, NULL, NULL, 0) ||//dummy instrukce -- konec if vetve
		getNextToken(attr, type, file) ||
		!statementList(attr, type, file) ||//<statement list>
		strcmp(attr->str, "end")||//end
		MakeInst(I_END_IF, NULL, NULL, NULL, 0) ||//dummy instrukce konec if prikazu
		getNextToken(attr, type, file)
		)
			return false;
	}
	//pravidlo 21.
	else if (!strcmp(attr->str, "while")) {//while
		if (getNextToken(attr, type, file) ||
		MakeInst(I_WHILE, NULL, NULL, NULL, 0) ||//vygeneruje dummy instrukci
		!precedenceSA(attr, type, file, false) ||//<expression>
		strcmp(attr->str, "do") ||//do
		MakeInst(I_DO, intermResult, NULL, NULL, 0) ||//generuje instrukci s dest. podminky
		getNextToken(attr, type, file) ||
		!statementList(attr, type, file) ||//<statement list>
		strcmp(attr->str, "end") ||//end
		MakeInst(I_DO_END, NULL, NULL, NULL, 0) ||//generuje dummy instrukci konec while
		getNextToken(attr, type, file)
		)
			return false;
	}
	//pravidlo 22.
	else if (!strcmp(attr->str, "repeat")) {//repeat
		if (getNextToken(attr, type, file) ||
		MakeInst(I_REPEAT, NULL, NULL, NULL, 0) ||//generuje dummy instrukci
		!statementList(attr, type, file) ||//<statement list>
		strcmp(attr->str, "until") ||//until
		getNextToken(attr, type, file) ||
		!precedenceSA(attr, type, file, false) ||//<expression>
		MakeInst(I_UNTIL, intermResult, NULL, NULL, 0)
		)
			return false;
	}
	return true;
}//konec funkce statement

//funkce pro nonterminal <statement list>
bool statementList(string *attr, int *type, FILE *file) {
	//nasleduje rozvoj dle pravidla 15.
	if (!strcmp(attr->str, "repeat") ||
	!strcmp(attr->str, "while") ||
	!strcmp(attr->str, "return") ||
	!strcmp(attr->str, "write") ||
	!strcmp(attr->str, "if") ||
	*type == IDENTIFIER
	) {
			if (!statement(attr, type, file) ||//<statement>
			strcmp(attr->str, ";") ||//;
			getNextToken(attr, type, file) ||
			!statementList(attr, type, file)//<statement list>
			)
				return false;
	}
	else//pravidlo 16.(epsilon pravidlo)
		return true;
	return true;
}

//funkce pro nonterminal <factor>
bool factor(string *attr, int *type, FILE *file, int initFlag, bool minusFlag) {

	if(*type == STRING ||//pravidlo 57.
	*type == INTEGER ||//pravidlo 58.
	*type == REAL ||//pravidlo 59.
	*type == LOGIC ||//pravidlo 60.
	*type == NIL
	) {
		//pridani hodnoty promenne do tabulky symbolu
		if (initFlag) {
			switch (*type) {
				case STRING:
					controlHelper->varData->type = T_STRING;
					if (strCopyString(controlHelper->varData->stringVar, attr)) {
						AddError(line, ProgErr);
						return false;
					}
					break;
				case INTEGER:
					controlHelper->varData->type = T_INTEGER;
					controlHelper->varData->real = atoi(attr->str);
					break;
				case REAL:
					controlHelper->varData->type = T_REAL;
					controlHelper->varData->real = strtod(attr->str, NULL);
					break;
				case LOGIC:
					controlHelper->varData->type = T_BOOLEAN;
					controlHelper->varData->boolean = (strcmp(attr->str, "true") ? false : true);
					break;
				case NIL:
					controlHelper->varData->type = T_NIL;
					break;
			}
			control.varData = controlHelper->varData;
			control.operation = O_ADD;
			tableHandler(&STable, &control);
			if (ErrorFlag == ProgErr)
				return false;
			}
			else {//initFlag = INIT_FALSE => zpracovavame parametry volani fce
				if (validateParamNumber()) {
					control.STitem = NULL;
					if (!funcArgFetch()) {//do control.variable priradi nazev argumentu
						AddError(line, ProgErr);
						return false;
					}
					//je potreba si vytvorit promennou
					if (!exprAlloc(pointerTrashPtr, &intermResult)) {
						AddError(line, ProgErr);
						return false;
					}

					switch (*type) {
						case STRING:
							intermResult->type = T_STRING;
							if (strCopyString(intermResult->stringVar, attr)) {
								AddError(line, ProgErr);
								return false;
							}
							break;
						case INTEGER:
							intermResult->type = T_INTEGER;
							intermResult->real = atoi(attr->str);
							if (minusFlag)
								intermResult->real = -intermResult->real;
							break;
						case REAL:
							intermResult->type = T_REAL;
							intermResult->real = strtod(attr->str, NULL);
							if (minusFlag)
								intermResult->real = -intermResult->real;
							break;
						case LOGIC:
							intermResult->type = T_BOOLEAN;
							intermResult->boolean = (strcmp(attr->str, "true") ? false : true);
							break;
						case NIL:
							intermResult->type = T_NIL;
							break;
					}
					//ziska ukazatel na argument, kde ma aktualizovat hodnotu parametru
					control.identif = controlHelper->function;
					control.variable = controlHelper->variable;
					control.function = controlHelper->identif;
					control.operation = O_FETCH_VALUE;
					//printf("function: %s, variable: %s, identif: %s\n", control.function->str, control.variable->str, control.identif->str);
					if (tableHandler(&STable, &control) == TREE_FAIL) {
						AddError(line, SemErr);
						return false;
					}
					control.identif = controlHelper->identif;
					control.function = controlHelper->function;
					//vytvori instrukci ktera, preda argumentu jeho hodnotu, pri zavolani
					if (MakeInst(I_ARG_ASSIGN, control.STitem, intermResult, NULL, 0))
						return false;

				}//if (validateParamNumber())
			}
		}//urceni typu
		/**
		Fce factor si zde neopravnene nacita dalsi token - resi se tak problem
		vznikajici kvuli epsilon pravidlu ve fci initialization()
		*/
		if (getNextToken(attr, type, file))
			return false;
		return true;
}

//funkce pro nonterminal <initialization>
bool initialization(string *attr, int *type, FILE *file) {
	if (getNextToken(attr, type, file))
		return false;

	//nasleduje rozvoj dle 13. pravidla
	if(!strcmp(attr->str, "=")) {//=
		if(getNextToken(attr, type, file) ||
		!factor(attr, type, file, INIT_TRUE, false)//<factor>
		)
			return false;
	}
	else//pravidlo 14.(epsilon pravidlo)
		return true;
	return true;
}

//funkce pro nonterminal <var def>
bool varDef(string *attr, int *type, FILE *file) {
	//local byl zkontrolovan ve var def list => cteme tedy dalsi token
	if (getNextToken(attr, type, file))
		return false;

	//nasleduje rozvoj dle 12. pravidla
	if(*type == IDENTIFIER) {//vid

		//pridani VID do tabulky symbolu
		if (strCopyString(controlHelper->variable, attr)) {
			AddError(line, ProgErr);
			return false;
		}
		control.varData = NULL;
		control.variable = controlHelper->variable;
		control.operation = O_ADD;
		if (tableHandler(&STable, &control) != TREE_SUCCESS) {
			if (ErrorFlag != LexErr && ErrorFlag != ProgErr)
				AddError(line, SemErr);
			return false;
		}

		if(!initialization(attr, type, file))//<initialization>
			return false;
	}
	else
		return false;
	return true;
}
//funkce pro nonterminal <var def list>
bool varDefList(string *attr, int *type, FILE *file) {
	if (getNextToken(attr, type, file))
		return false;

	//rozvoj dle 10. pravidla
	if(!strcmp(attr->str, "local")) {
		if(!varDef(attr, type, file) ||//<var def>
		strcmp(attr->str, ";") ||//;
		!varDefList(attr, type, file)//<var def list>
		)
			return false;
	}
	else//pravidlo 11.(epsilon pravidlo)
		return true;
	return true;
}

//funkce pro nonterminal <argument_n>
bool argumentNext(string *attr, int *type, FILE *file) {
	if (getNextToken(attr, type, file))
		return false;

	//rozvoj dle 8. pravidla
	if(!strcmp(attr->str, ",")) {//,
		if (getNextToken(attr, type, file))
			return false;
		if(*type == IDENTIFIER) {

			if (!argumentAddToArgArr(attr)) {
				AddError(line, ProgErr);
				return false;
			}

			if (strCopyString(controlHelper->variable, attr)) {
				AddError(line, ProgErr);
				return false;
			}
			control.varData = NULL;
			control.variable = controlHelper->variable;
			control.operation = O_ADD;
			if (tableHandler(&STable, &control) != TREE_SUCCESS) {
				if (ErrorFlag != LexErr && ErrorFlag != ProgErr)
					AddError(line, SemErr);
				return false;
			}

			if(!argumentNext(attr, type, file))//<argument_n>
				return false;//spatna syntaxe
		}
		else
			return false;//spatna syntaxe
	}
	else//pravidlo 9.(epsilon pravidlo)
		return true;
	return true;
}

//funkce pro nonterminal <argument list>
bool argumentList(string *attr, int *type, FILE *file) {
	if (getNextToken(attr, type, file))
		return false;

	if (mainFlag)//main nema zadne argumenty
		return true;

	//rozvoj dle 6. pravidla
	if(*type == IDENTIFIER) {//vid
		//prida argument do pole argumentu dane fce
		if (!argumentAddToArgArr(attr)) {
			AddError(line, ProgErr);
			return false;
		}
		//zanese argument do tabulky symbolu
		if (strCopyString(controlHelper->variable, attr)) {
			AddError(line, ProgErr);
			return false;
		}
		control.varData = NULL;
		control.variable = controlHelper->variable;
		control.operation = O_ADD;
		if (tableHandler(&STable, &control) != TREE_SUCCESS) {
			if (ErrorFlag != LexErr && ErrorFlag != ProgErr)
				AddError(line, SemErr);
			return false;
		}

		if(!argumentNext(attr, type, file))//<argument_n>
			return false;
	}
	else//pravidlo 7.(epsilon pravidlo)
		return true;
	return true;
}

//funkce pro nonterminal <func def>
bool funcDef(string *attr, int *type, FILE *file) {
	if (getNextToken(attr, type, file))
		return false;
	//nasleduje rozvoj dle pravidla 5.
	if (*type == IDENTIFIER) {//fid

		//osetri vsechny problemy s mainem(dvojita deklarace, main neni posledni)
		if (mainFlag)
			return false;
		if (!strcmp(attr->str, "main"))
				mainFlag = true;

		//Prida fci do tabulky symbolu
		if (strCopyString(controlHelper->function, attr))
			return false;
		control.paramCounter = 0;
		control.identif = NULL;
		control.STitem = NULL;
		control.varData = NULL;
		control.variable = NULL;
		control.function = controlHelper->function;
		control.operation = O_ADD;
		if (tableHandler(&STable, &control) != TREE_SUCCESS) {
			if (ErrorFlag != ProgErr && ErrorFlag != LexErr)
				AddError(line, SemErr);
		return false;
		}

		//vytvori fci prazdny seznam argumentu
		if (!funcsArgsCreate()) {
			AddError(line, ProgErr);
			return false;
		}

		//vytvori pomocnou promennou do ktere nakopiruje identifikator deklarovane
		//fce a vytvori instrukci deklarace
		if (!exprAlloc(pointerTrashPtr, &exprResult)) {
			AddError(line, ProgErr);
			return false;
		}
		if (strCopyString(exprResult->stringVar, attr)) {
			AddError(line, ProgErr);
			return false;
		}

		//prida do instrkucniho listu zaznam o nove deklaraci fce
		if (MakeInst(I_DECL_FUNC, exprResult->stringVar, NULL, NULL, funcsArgs->funcCounter))
			return false;
		//printf("I_DECL_FUNC\n");

		if (getNextToken(attr, type, file))
			return false;

		//pokracujeme dle rozvoje v 5. pravidle
		if (strcmp(attr->str, "(") ||//(
		!argumentList(attr, type, file) ||//<argument list>
		strcmp(attr->str, ")") ||//)
		!varDefList(attr, type, file) ||//<var definition list>
		!statementList(attr, type, file) ||//<statement list>
		strcmp(attr->str, "end") ||//end
		MakeInst(I_END_DECL_FUNC, NULL, NULL, NULL, funcsArgs->funcCounter)
		)
			return false;
		//printf("I_END_DECL_FUNC\n");
	}
	else
		return false;
	return true;
}

//fce pro nonterminal <func def_n>
bool funcDefNext(string *attr, int *type, FILE *file) {
	if (getNextToken(attr, type, file))
		return false;

	if (!strcmp(attr->str, ";"))//pravidlo 4.(epsilon pravidlo)
		return true;

	//nasleduje rozvoj dle pravdila 3.
	else if (strcmp(attr->str, "function") ||
	!funcDef(attr, type, file) ||//<func def>
	!funcDefNext(attr, type, file)//<func def_n>
	)
		return false;
	return true;
}

//fce pro nonterminal <func def list>
bool funcDefList(string *attr, int *type, FILE *file) {

	if (getNextToken(attr, type, file))//ziskam prvni token
		return false;

	//nasleduje rozvoj dle pravidla 2.
	if (strcmp(attr->str, "function") ||
	!funcDef(attr, type, file) ||//<function def>
	!funcDefNext(attr, type, file) ||//<func def_n>
	strcmp(attr->str,";") ||//;
	getNextToken(attr, type, file) ||
	*type != END_OF_FILE ||//$
	!mainFlag//main musi byt definovan
	) {
			if (!ErrorFlag && ErrorFlag != ProgErr && ErrorFlag != LexErr)
				AddError(line, SynErr);
			return false;
	}
	return true;
}

//funkce alkouje pamet pro strukturu rozhrani STable handlera a parseru
bool controlInit(tTreeCont **control) {
	if ((*control = malloc(sizeof(tTreeCont))) != NULL) {
		(*control)->function = NULL;
		(*control)->variable = NULL;
		(*control)->identif = NULL;
		(*control)->varData = NULL;
		if ((((*control)->function = malloc(sizeof(string))) != NULL) &&
		(((*control)->variable = malloc(sizeof(string))) != NULL) &&
		(((*control)->identif = malloc(sizeof(string))) != NULL) &&
		(((*control)->varData = malloc(sizeof(tVarValue))) != NULL) &&
		(((*control)->varData->stringVar = malloc(sizeof(string))) != NULL)
		) {
			(*control)->function->str = NULL;
			(*control)->variable->str = NULL;
			(*control)->identif->str = NULL;
			(*control)->varData->stringVar->str = NULL;
			if (!strInit((*control)->function) &&
			!strInit((*control)->variable) &&
			!strInit((*control)->identif) &&
			!strInit((*control)->varData->stringVar)
			)
				return true;
		}
		else
			controlDispose(control);
	}
	return false;
}

//funkce uvolnuje strukturu tTreeCont
void controlDispose(tTreeCont **control) {
	if (*control != NULL) {
		if ((*control)->function != NULL)
			strFree((*control)->function);
		free((*control)->function);
		if ((*control)->variable != NULL)
			strFree((*control)->variable);
		free((*control)->variable);
		if ((*control)->identif != NULL)
			strFree((*control)->identif);
		free((*control)->identif);
		if ((*control)->varData != NULL) {
			if ((*control)->varData->stringVar != NULL)
				strFree((*control)->varData->stringVar);
			free((*control)->varData->stringVar);
		}
		free((*control)->varData);
	}
	free(*control);
}

bool argumentArrayInit(tArgArrPtr *argumentArray) {
	if ((*argumentArray = malloc(sizeof(struct ArgArr))) != NULL) {
		if (((*argumentArray)->array = malloc(sizeof(string *) * ARG_ARR_ISIZE)) != NULL) {
			if (((*argumentArray)->identif = malloc(sizeof(string))) != NULL) {
				if (!strInit((*argumentArray)->identif)) {
					for (int i = 0; i < ARG_ARR_ISIZE; i++)
						(*argumentArray)->array[i] = NULL;
					for (int i = 0; i < ARG_ARR_ISIZE; i++) {
						if (((*argumentArray)->array[i] = malloc(sizeof(string))) == NULL)
							return false;
					}
					for (int i = 0; i < ARG_ARR_ISIZE; i++)
						((*argumentArray)->array[i])->str = NULL;
					for (int i = 0; i < ARG_ARR_ISIZE; i++) {
						if (strInit((*argumentArray)->array[i]))
							return false;
					}
					(*argumentArray)->size = ARG_ARR_ISIZE;
					(*argumentArray)->argCounter = 0;
					return true;
				}
			}
		}
	}
	return false;
}

void argumentArrayDispose(tArgArrPtr *argumentArray) {
	if (*argumentArray != NULL) {
		if ((*argumentArray)->array != NULL) {
			for (int i = 0; i < (*argumentArray)->size; i++) {
				if ((*argumentArray)->array[i] != NULL)
					strFree((*argumentArray)->array[i]);
				free((*argumentArray)->array[i]);
			}
		}
		free((*argumentArray)->array);
		if ((*argumentArray)->identif != NULL)
			strFree((*argumentArray)->identif);
		free((*argumentArray)->identif);
	}
	free(*argumentArray);
}

bool funcsArgsArrInit(tFuncArgPtr *funcsArgs) {
	if ((*funcsArgs = malloc(sizeof(struct FuncsArgs))) != NULL) {
		if (((*funcsArgs)->argumentArray = malloc(sizeof(tArgArrPtr) * FUNCS_ARGS_ISIZE)) != NULL) {
			for (int i = 0; i < FUNCS_ARGS_ISIZE; i++)
				(*funcsArgs)->argumentArray[i] = NULL;
			(*funcsArgs)->size = FUNCS_ARGS_ISIZE;
			(*funcsArgs)->funcCounter = 0;
			return true;
		}
	}
	return false;
}


void funcsArgsArrDispose(tFuncArgPtr *funcsArgs) {
	if (*funcsArgs != NULL) {
		if ((*funcsArgs)->argumentArray != NULL) {
			for (int i = 0; i < (*funcsArgs)->size; i++)
				argumentArrayDispose(&((*funcsArgs)->argumentArray[i]));
		}
		free((*funcsArgs)->argumentArray);
	}
	free(*funcsArgs);
}

//Fce vytvori prazdny seznam argumentu aktualni fce
bool funcsArgsCreate() {
	if (funcsArgs->funcCounter != funcsArgs->size) {
		if (!argumentArrayInit(&(funcsArgs->argumentArray[funcsArgs->funcCounter])))
			return false;
		else if (strCopyString((funcsArgs->argumentArray[funcsArgs->funcCounter])->identif, control.function))
			return false;
		funcsArgs->funcCounter++;
	}
	else {
		tArgArrPtr *helper;
		funcsArgs->size *= 2;
		if ((helper = realloc(funcsArgs->argumentArray, sizeof(tArgArrPtr) * funcsArgs->size)) == NULL)
			return false;
		else {
			funcsArgs->argumentArray = helper;
			for (int i = funcsArgs->size / 2; i < funcsArgs->size; i++) {
				if (!argumentArrayInit(&(funcsArgs->argumentArray[i])))
					return false;
			}
			funcsArgsCreate();
		}
	}
	return true;
}

bool argumentAddToArgArr(string *attr) {
	tArgArrPtr arrayHelper = funcsArgs->argumentArray[funcsArgs->funcCounter-1];
	if (arrayHelper->argCounter != arrayHelper->size) {
		if (strCopyString(arrayHelper->array[arrayHelper->argCounter], attr))
			return false;
		arrayHelper->argCounter++;
	}
	else {
		string **reallocHelper;
		arrayHelper->size *= 2;
		if ((reallocHelper = realloc(arrayHelper->array, sizeof(string *) * arrayHelper->size)) == NULL)
			return false;
		else {
			arrayHelper->array = reallocHelper;
			for (int i = arrayHelper->size / 2; i < arrayHelper->size; i++)
				arrayHelper->array[i] = NULL;
			for (int i = arrayHelper->size / 2; i < arrayHelper->size; i++) {
				if ((arrayHelper->array[i] = malloc(sizeof(string))) == NULL)
					return false;
			}
			for (int i = arrayHelper->size / 2; i < arrayHelper->size; i++)
				arrayHelper->array[i]->str = NULL;
			for (int i = arrayHelper->size / 2; i < arrayHelper->size; i++) {
				if (strInit(arrayHelper->array[i]))
					return false;
			}
			argumentAddToArgArr(attr);
		}
	}
	return true;
}

//Najde nazev argumentu volane funkce a ulozi ho odkazem do control.variable
bool funcArgFetch() {
	string *helper;
	for (int i = 0; i < funcsArgs->size; i++) {
		if (!strCmpString((funcsArgs->argumentArray[i])->identif, control.identif)) {
			helper = (funcsArgs->argumentArray[i])->array[control.paramCounter];
			break;
		}
	}

	if (strCopyString(controlHelper->variable, helper))
		return false;
	control.variable = controlHelper->variable;
	return true;
}

//Ujisti se ze zpracovavany parametr byl definovan jako argument
bool validateParamNumber() {
	int helper;
	for (int i = 0; i < funcsArgs->size; i++) {
		if (!strCmpString((funcsArgs->argumentArray[i])->identif, control.identif)) {
			helper = (funcsArgs->argumentArray[i])->argCounter - 1;
			break;
		}
	}
	if (control.paramCounter > helper)
		return false;
	return true;
}

//Funkce zanasi do instrukcniho listu deklarace vestavenych fci
bool parserPrologue() {
	bool result = true;
	char *funcIdentifs[] = {"substr", "sort", "type", "find"};
	char *varIdentifs[] = {"right", "left", "string", "id", "substr"};
	string *fidHelper = NULL, *vidHelper = NULL, *vidHelper2 = NULL, *vidHelper3 = NULL;
	//zajisti aby control.function ukazoval kam ma(pouziva se pri funcsArgsCreate)
	control.function = controlHelper->function;
	//nachysta strukturu pro indexy fce substr
	if ((substrIndexes = malloc(sizeof(struct SubstrIndexes))) == NULL)
		return false;

	//provadi zanaseni do instrkucniho listu a listu argumentu fci
	/**type(id)*/
	//vytvori string z jejiho id
	if (!toString(&fidHelper, funcIdentifs[2]))
		result = false;
	//nakopiruje ho do promenne pro id fci
	else if (strCopyString(controlHelper->function, fidHelper))
		result = false;
	//vytvori pole argumentu teto fce
	else if (!funcsArgsCreate())
		result = false;
	//vytvori instrukci I_DECL_FUNC
	else if (MakeInst(I_DECL_FUNC, fidHelper, NULL, NULL, funcsArgs->funcCounter))
		result = false;
	//vytvori string z argumentu
	else if (!toString(&vidHelper, varIdentifs[3]))
		result = false;
	//prida do jejiho pole argumentu spravne argumenty
	else if (!argumentAddToArgArr(vidHelper))
		result = false;
	//vytvori cil pro instrukci volani vestavene fce
	else if (!exprAlloc(pointerTrashPtr, &intermResult))
		result = false;
	//vytvori instrkuci pro zavolani vestavene fce, zdojovou adresu zna, takze ji
	//lze specifikovat "natvrdo"
	else if (MakeInst(I_TYPE, intermResult, STable->RightRoot->rootVarTree->data, NULL, 0))
		result = false;
	//vytvori instrkuci return a preda ji adresu cile vypoctu vestavene fce
	else if (MakeInst(I_RETURN, intermResult, NULL, NULL, 0))
		result = false;
	//singalizuje konec deklarace vestavene fce
	else if (MakeInst(I_END_DECL_FUNC, NULL, NULL, NULL, funcsArgs->funcCounter))
		result = false;
	//uvolni pomocne stringy
	disposeString(&fidHelper);
	disposeString(&vidHelper);
	if (!result)
		return result;
	/****************************************************************************/
	/**sort(string)*/
	if (!toString(&fidHelper, funcIdentifs[1]))
		result = false;
	//nakopiruje ho do promenne pro id fci
	else if (strCopyString(controlHelper->function, fidHelper))
		result = false;
	//vytvori pole argumentu teto fce
	else if (!funcsArgsCreate())
		result = false;
	//vytvori instrukci I_DECL_FUNC
	else if (MakeInst(I_DECL_FUNC, fidHelper, NULL, NULL, funcsArgs->funcCounter))
		result = false;
	//vytvori string z argumentu
	else if (!toString(&vidHelper, varIdentifs[2]))
		result = false;
	//prida do jejiho pole argumentu spravne argumenty
	else if (!argumentAddToArgArr(vidHelper))
		result = false;
	//vytvori cil pro instrukci volani vestavene fce
	else if (!exprAlloc(pointerTrashPtr, &intermResult))
		result = false;
	//vytvori instrkuci pro zavolani vestavene fce, zdojovou adresu zna, takze ji
	//lze specifikovat "natvrdo"
	else if (MakeInst(I_SORT, intermResult, STable->LeftRoot->rootVarTree->data, NULL, 0))
		result = false;
	//vytvori instrkuci return a preda ji adresu cile vypoctu vestavene fce
	else if (MakeInst(I_RETURN, intermResult, NULL, NULL, 0))
		result = false;
	//singalizuje konec deklarace vestavene fce
	else if (MakeInst(I_END_DECL_FUNC, NULL, NULL, NULL, funcsArgs->funcCounter))
		result = false;
	//uvolni pomocne stringy
	disposeString(&fidHelper);
	disposeString(&vidHelper);
	if (!result)
		return result;
	/****************************************************************************/
	/**find(string, substr)*/
	if (!toString(&fidHelper, funcIdentifs[3]))
		result = false;
	//nakopiruje ho do promenne pro id fci
	else if (strCopyString(controlHelper->function, fidHelper))
		result = false;
	//vytvori pole argumentu teto fce
	else if (!funcsArgsCreate())
		result = false;
	//vytvori instrukci I_DECL_FUNC
	else if (MakeInst(I_DECL_FUNC, fidHelper, NULL, NULL, funcsArgs->funcCounter))
		result = false;
	//vytvori string z prvniho argumentu
	else if (!toString(&vidHelper, varIdentifs[2]))
		result = false;
	//prida do jejiho pole argumentu spravne argumenty
	else if (!argumentAddToArgArr(vidHelper))
		result = false;
	//vytvori string z druheho argumentu
	else if (!toString(&vidHelper2, varIdentifs[4]))
		result = false;
	//prida do jejiho pole argumentu spravne argumenty
	else if (!argumentAddToArgArr(vidHelper2))
		result = false;
	//vytvori cil pro instrukci volani vestavene fce
	else if (!exprAlloc(pointerTrashPtr, &intermResult))
		result = false;
	//vytvori instrkuci pro zavolani vestavene fce, zdojovou adresu zna, takze ji
	//lze specifikovat "natvrdo"
	else if (MakeInst(I_FIND, intermResult, STable->LeftRoot->LeftRoot->rootVarTree->data,
					 STable->LeftRoot->LeftRoot->rootVarTree->RightRoot->data, 0))
		result = false;
	//vytvori instrkuci return a preda ji adresu cile vypoctu vestavene fce
	else if (MakeInst(I_RETURN, intermResult, NULL, NULL, 0))
		result = false;
	//singalizuje konec deklarace vestavene fce
	else if (MakeInst(I_END_DECL_FUNC, NULL, NULL, NULL, funcsArgs->funcCounter))
		result = false;
	//uvolni pomocne stringy
	disposeString(&fidHelper);
	disposeString(&vidHelper);
	disposeString(&vidHelper2);
	if (!result)
		return result;
	/****************************************************************************/
	/**substr(string, left, right)*/
	if (!toString(&fidHelper, funcIdentifs[0]))
		result = false;
	//nakopiruje ho do promenne pro id fci
	else if (strCopyString(controlHelper->function, fidHelper))
		result = false;
	//vytvori pole argumentu teto fce
	else if (!funcsArgsCreate())
		result = false;
	//vytvori instrukci I_DECL_FUNC
	else if (MakeInst(I_DECL_FUNC, fidHelper, NULL, NULL, funcsArgs->funcCounter))
		result = false;
	//vytvori string z prvniho argumentu
	else if (!toString(&vidHelper, varIdentifs[0]))
		result = false;
	//prida do jejiho pole argumentu spravne argumenty
	else if (!argumentAddToArgArr(vidHelper))
		result = false;
	//vytvori string z druheho argumentu
	else if (!toString(&vidHelper2, varIdentifs[1]))
		result = false;
	//prida do jejiho pole argumentu spravne argumenty
	else if (!argumentAddToArgArr(vidHelper2))
		result = false;
	//vytvori string z tretiho argumentu
	else if (!toString(&vidHelper3, varIdentifs[2]))
		result = false;
	//prida do jejiho pole argumentu spravne argumenty
	else if (!argumentAddToArgArr(vidHelper3))
		result = false;
	//vytvori cil pro instrukci volani vestavene fce
	else if (!exprAlloc(pointerTrashPtr, &intermResult))
		result = false;

	if (!result) {
		disposeString(&fidHelper);
		disposeString(&vidHelper);
		disposeString(&vidHelper2);
		disposeString(&vidHelper3);
	}
	substrIndexes->left = STable->rootVarTree->LeftRoot->data;
	substrIndexes->right = STable->rootVarTree->RightRoot->data;
	//vytvori instrkuci pro zavolani vestavene fce, zdojovou adresu zna, takze ji
	//lze specifikovat "natvrdo"
	if (MakeInst(I_SUBSTR, intermResult, STable->rootVarTree->data, substrIndexes, 0))
		result = false;
	//vytvori instrkuci return a preda ji adresu cile vypoctu vestavene fce
	else if (MakeInst(I_RETURN, intermResult, NULL, NULL, 0))
		result = false;
	//singalizuje konec deklarace vestavene fce
	else if (MakeInst(I_END_DECL_FUNC, NULL, NULL, NULL, funcsArgs->funcCounter))
		result = false;
	//uvolni pomocne stringy
	disposeString(&fidHelper);
	disposeString(&vidHelper);
	disposeString(&vidHelper2);
	disposeString(&vidHelper3);
	return result;
}

/**
	Obalovaci funkce parseru, hlavni cinnosti:
		-sprava rozhrani mezi parserem a tabulkou symbolu
		-sprava rozhrani mezi parserem a instrukcnim listem
		-volani rozvoje korenoveho neterminalu <func def list>
*/
bool parser(string *attr, int *type, FILE *file) {
	bool result = true;


	if (!controlInit(&controlHelper)) {
		AddError(line, ProgErr);
		result = false;
	}
	else if (!funcsArgsArrInit(&funcsArgs)) {
		AddError(line, ProgErr);
		result = false;
	}
	else if (!pointerTrashInit(&pointerTrashPtr)) {
		AddError(line, ProgErr);
		result = false;
	}
	else if (!parserPrologue()) {
	AddError(line, ProgErr);
		result = false;
	}
	else if (!funcDefList(attr, type, file))
		result = false;
	funcsArgsArrDispose(&funcsArgs);
	return result;
}

//

/*int main(int argc, char *argv[]) {*/

/*	MakeErrList();*/
/*	FILE *file;*/
/*	if (argc == 2) {*/
/*		file = fopen(argv[1], "r");//otevreni vstupniho souboru*/
/*	}*/
/*	else*/
/*		return EXIT_FAILURE;//spatny pocet parametru => chyba*/

/*	if (file == NULL)*/
/*		return EXIT_FAILURE;//soubor se nezdarilo otevrit => chyba*/

/*	int type = 0;//prmenna do ktere se uklada typ tokenu*/

/*	//inicializace retezce pro obsah tokenu*/
/*	string *attr = malloc(sizeof(string));*/

/*	if (attr == NULL)*/
/*		return EXIT_FAILURE;//nedostatek pameti => chyba*/
/*	strInit(attr);*/

/*	if (funcDefList(attr, &type, file))*/
/*		printf("\nTRUE\n");*/
/*	else*/
/*		printf("\nFALSE\n");*/

/*	fclose(file);*/
/*	free(attr->str);*/
/*	free(attr);*/
/*	PrintError();*/
/*	FreeError();*/
/*	return EXIT_SUCCESS;*/
/*}*/
