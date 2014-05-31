/* ************************ scanner.c ************************************** */
/* Soubor:              scanner.c - Lexikalni analyzator                      */
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


#include "scanner.h"

enum tstates
{

START ,				//0- pocatecni stav
MINUS,				//1 - komentar/minus
KONTROLA_VNORENEHO_KOMENTARE, //2 - kontrola vnoreneho komentare 
KONTROLA, 			//3 - kontrola druheho znaku vnoreneho komentare
KOMENTAR,			//4 - komentar
VNORENY_KOMENTAR,	// 5 - vnoreny komentar
KONEC_VNORENEHO, 	//6 - konec vnoreneho komentare
IDENTIFIKATOR,		//7 - identifikator
RETEZEC,				//8 - retezec
ESCAPE_SEKVENCE,	//9 - kontrola escape sekvenci
CISLO, 				//10 - vynechani pocatecnich nul u cisla
CELOCISELNA_CAST,	//11 - celociselna cast cisla
KONTROLA_NEPRAZDNOSTI, //12 - kontrola neprazdnosti desetineho cisla
DESETINE_CISLO,	//13 - desetine cislo
MOCNINA_KONTROLA,	//14 - kontrola neprazdnosti mocniny nebo zadani +- u mocniny
PLUSMINUS,			//15 - kontrola neprazdnosti cisla mocniny po zadani +-
KONTROLA_NUL,		//16 - vynechani pocatecnich nul u mocniny
MOCNINA,				//17 - hodnoty mocniny
KONKATENACE,		//18 - konkatenace
DVOUZNAKOVE_OPERATORY,//19 - rozliseni dvouznakovych operatoru od jednoznakovych == <= >= 
DVOUZNAKOVY_OPERATOR ,//20 - kontrola spravnosti dvouznakoveho operatoru ~=
};


// promena cisla radku 
int line =1;
// promena pro ulozeni zbyleho znaku
int rest=-1;
// vycet klicovych slov
char* key_words[] =
  {
   "do","if","then","else","local","true","end","while",
   "false", "read","write","function","return","repeat","until"
  };
// vycet logickych hodnot
char * logic_words[]=
{ "true","false"
};

// funkce po chybe ulozi prvni dulezity znak
void chyba (int *cislo, FILE *source)
{
  while ((!isspace(*cislo)) && *cislo!='+' &&  *cislo != '*' && *cislo != '/' && *cislo != '^' && *cislo != '(' && *cislo != ')' 
   && *cislo != ';' && *cislo != ','  && *cislo!='.' && *cislo!='=' && *cislo!='<' && *cislo!='>' && *cislo!='~'&& *cislo!='#' && *cislo!='%')
  { 	
	*cislo=getc(source);
	if (*cislo==LF) // kontrola poctu radku
		line++;
  }
 rest=*cislo;	  	 

}
// chyba v retezci hledame vyskyt uvozovek
void chybastring (int *cislo, FILE* source)
{
	while (*cislo!='"')
	{
		*cislo=getc(source);
		if (*cislo==LF) // kontrola poctu radku
			line++;
	}

}

// hlavni funkce lexikalniho analyzatoru
int getNextToken(string *attr, int *typ,FILE* source )
{  
	// pomocna promena
	int pom;
	// znak
	int c;
	// stav
	int state=0;
	// kontrola - integer - real
	int CHECKreal=0;
	// ziskani hodnoty ulozeneho znaku   
	pom=rest;
   	// vymazeme obsah atributu 
	strClear(attr);
  	while (1)
   	{
     		if (pom<0) //kontrola zda byl ulozen nejaky znak
     		{
				c = getc(source);// nacteni noveho znaku
				if (c==LF) // kontrola poctu radku
					line++;
     		}
     			else
     			{
      			c=rest;// precteni ulozeneho znaku
      			// prepsani 
					rest=-1;
					pom=-1;
     			}
//KONECNY AUTOMAT
	switch (state)
     	{
	// VYCHOZI STAV
		case START:
			// bila mista - ignorovat
			if (isspace(c))
			 	state = START;
				// komentar - operator minus
				else if (c == '-')
			 		state = MINUS;
		 			// identifikator nebo klicove slovo
					else if (isalpha(c) || c == '_')
					{    
			 			if((strAddChar(attr, c)))
						{
							AddError(line, ProgErr);
							return ERROR_ALOK ;
						}
							else
								state = IDENTIFIKATOR;
		 			}
						// retezec
		 				else if (c== '"')
		 					{
			 					state = RETEZEC;
		 					}
		 						// stav pro odstraneni prebytecnych nul
		 						else if (c=='0')
								{
			 						state=10;
		 	 					}
									// cislo
		 							else if (isdigit(c) )
									{
			 							if((strAddChar(attr, c))==1)
											{
												AddError(line, ProgErr);
												return ERROR_ALOK ;
											}	
											else
			 									state = CELOCISELNA_CAST;
									}
		  								// operator jednoznakovy
										else if (c == '+' || c == '*' || c == '/' ||c == '^'  || c == '(' 
											|| c == ')' ||c == ';'|| c == ',' || c=='#' || c=='%') 
										{
											if((strAddChar(attr, c))==1)
												{
													AddError(line, ProgErr);
													return ERROR_ALOK ;
												}	
												else
			 										{
														*typ=OPERATOR;
			  											return NoErr;
		 											}
										}
											// konkatenace
		 									else if (c == '.') 
		 									{
												if((strAddChar(attr, c))==1)
													{
														AddError(line, ProgErr);
														return ERROR_ALOK ;
													}	
													else
														state=KONKATENACE;
		 									}
		 										// dvouznakove operatory, mohou fungovat i samostatne
		 										else if (c == '=' || c == '<' || c == '>') 
												{
													if((strAddChar(attr, c))==1)
														{
															AddError(line, ProgErr);
															return ERROR_ALOK ;
														}	
														else
															state=DVOUZNAKOVE_OPERATORY;
												}
													// dvouznakovy operator, vlnovka neexistuje samostatne 
													else if (c == '~') 
													{
														if((strAddChar(attr, c))==1)
															{
																AddError(line, ProgErr);
																return ERROR_ALOK ;
															}
															else
																state=DVOUZNAKOVY_OPERATOR;
													}
		 												// konec souboru - adekvatni
		 												else if (c == EOF) 
														{ 
															*typ=END_OF_FILE;
															return NoErr;
														}
		 													// chyba
															else
		 													
															{ 
																AddError(line, LexErr);
		 														return LexErr;
															}
		break;

// KOMENTAR
		case MINUS:
			if (c != '-' ) // nejedna se o komentar ale o znamenko minus
			  	{ 
					if((strAddChar(attr, '-'))==1)
					{
						AddError(line, ProgErr);
						return ERROR_ALOK ;
					}
						else
						{
							rest=c;
							*typ=OPERATOR;
							return NoErr;
						}
				}
					else if (c == EOF) // konec souboru -adekvatni
					{
			  			*typ=END_OF_FILE;
						return NoErr;
					}
						else // jedna se o komentar - znaky neukladame
							state =KONTROLA_VNORENEHO_KOMENTARE;
		break;

// KONTROLA VNORENEHO KOMENTARE
		case KONTROLA_VNORENEHO_KOMENTARE: 
			if (c=='[') // muze se jednat o vlozeny komentar
			 	state=3;
		  		else if (c==LF )// ukonceni komentare
			 		state=0;
					else if (c == EOF) // ukonceni souboru  - adekvatni
		 			{
						*typ=END_OF_FILE;
						return 0;
					}
		 				else 
							state=4;
	  	break;   

//KONTROLA DRUHEHO ZNAKU VNORENEHO KOMETARE
		case KONTROLA: 
			if (c=='[')
			 	state=VNORENY_KOMENTAR;
		 		else if (c==LF )// ukonceni komentare
					state=START;
		  			else if (c == EOF) // ukonceni souboru -adekvatni
		  			{
			 			*typ=END_OF_FILE;
			 			return NoErr;
		  			}
		  				else // zahazujeme znaky 
		  					state =KOMENTAR;
		break;

// KOMENTAR
		case KOMENTAR:
			if (c==LF)// ukonceni
				state=START;
	 			else if (c== EOF) // konec souboru- adekvatni
	  			{  
					*typ=END_OF_FILE;
					return NoErr;
	  			}
		break;

//VNORENY KOMENTAR
		case VNORENY_KOMENTAR:
			if (c==']')// ukonceni- jinak znaky zahazujeme
	  			state=KONEC_VNORENEHO;
				else if (c == EOF) // konec souboru -neadekvatni
	  			{
					AddError(line,LexErr);
					*typ=END_OF_FILE;
					return LexErr;
	 			}
	 	break;

// KONEC VNORENEHO KOMENTARE
		case KONEC_VNORENEHO:
			if (c==']')// ukonceni komentare - nacitame nove slovo
	  			state=START;
	  			else if (c == EOF) // konec souboru  -neadekvatni 
				{
					AddError(line,LexErr);
					*typ=END_OF_FILE;
					return LexErr;
				}
	  				else// vraceni zpet - nedokoncene ukonceni
	  					state=VNORENY_KOMENTAR;
		break;

		//IDENTIFIKATOR
		case IDENTIFIKATOR:
			if (isalpha(c) || isdigit(c) || c=='_')// povolene znaky 
				{
					if((strAddChar(attr, c))==1)
					{
						AddError(line,ProgErr);
						return ERROR_ALOK;
					}
				}
	  			else
	  			{
					state=START;
					rest=c; 
	    			// kontrola zda se nejdna o logickou promenou
	    			for (int i=0;i<=MAX_LOGIC;i++)
	    			{
						if ((strCmpConstStr(attr,logic_words[i]))==0)
						{
		  					*typ=LOGIC;
		  					return NoErr;
						}
	    			}
	    			// kontrola zda se nejdna o klicove slovo
	    			for (int i=0;i<=MAX_KEY;i++)
	    			{
						if ((strCmpConstStr(attr,key_words[i]))==0)
						{ 
							*typ=KEY_WORD;
							return NoErr;
						}
	    			}
					// kontrola zda se nejedna o reservovane slovo
					
						if ((strCmpConstStr(attr,"nil"))==0)
						{ 
							*typ=NIL;
							return NoErr;
						}
	    			
	    			// nejedna se ani o jeden typ specialnich slov =>jde o identifikator
	    				*typ=IDENTIFIER;
	    				return NoErr;
	  			}
		break;

// RETEZEC
		case RETEZEC:
			if (c == EOF) // konec souboru   -neadekvatni
			{
				AddError(line,LexErr);
				*typ=END_OF_FILE;
				return LexErr;
			}
				else if (c=='"')// prazdny retezec
	   		{
					*typ= STRING;
					return NoErr;
	   		}
	  				else if (c=='\\')// kontrola speciich znaku
	  				{ 
						 state=ESCAPE_SEKVENCE;
	  				}
	  					else if (c<1 || c>255)// nepovolene znaky
	  					{
							AddError(line,LexErr);
	    					chybastring (&c, source);
	   	 				return LexErr;
	  					}
	 						else if((strAddChar(attr, c))==1) // ulozeni znaku komentare
							{
								AddError(line,ProgErr);
								return ERROR_ALOK;
							}
		break;

//ESCAPE SEKVENCE
		case ESCAPE_SEKVENCE:
			if (c<1 || c>255)// chyba
	  		{
				AddError(line,LexErr);
				chybastring (&c,  source);
				return LexErr;
	  		}
	  			else if (c=='n')// specialni znak
	  			{
					c='\n';
					if((strAddChar(attr, c))==1)
					{
						AddError(line,ProgErr);
						return ERROR_ALOK;
					}
						else
							state=RETEZEC;
	  			}
	  				else if (c=='t')// specialni znak
	  				{
						c='\t';
						if((strAddChar(attr, c))==1)
						{
							AddError(line,ProgErr);
							return ERROR_ALOK;
						}
							else
						 		state=RETEZEC;
	  				}
	  					else if (c=='\\')// specialni znak
	  					{
							c='\\';
							if((strAddChar(attr, c))==1)
							{
								AddError(line,ProgErr);
								return ERROR_ALOK;
							}
								else
									state=RETEZEC;
	  					}
	  						else if (c=='"')// specialni znak
	  						{
								c='\"';
								if((strAddChar(attr, c))==1)
								{
									AddError(line,ProgErr);
									return ERROR_ALOK;
								}
									else
										state=RETEZEC;
	  						}
	 							else if (isdigit(c))// specialni znak zadany trojmistnym cislem
	  							{
									pom=c;
									if (isdigit(c=getc(source)))// druhe cislo
									{
										pom=pom*10+c;
										if (isdigit(c=getc(source)))//treti cislo
											pom=pom*10+c;
											else if (c == EOF) // konec souboru - neadekvatni
											{ 
												AddError(line,LexErr);
		  										*typ=END_OF_FILE;
		 										return LexErr;
											}		
												else// chyba treti znak neni cislo
												{
													AddError(line,LexErr);
		  											chyba (&c,source);
													return LexErr;
												}
	    							}
										else if (c == EOF) // konec souboru - neadekvatni
										{ 
											AddError(line,LexErr);
											*typ=END_OF_FILE;
											return LexErr;
										}
											else// chyba druhy znak neni cislo
											{
												AddError(line,LexErr);
												chybastring (&c,source);
												return LexErr;
			  								}
										c=pom;
										if((strAddChar(attr, c))==1)
										{
											AddError(line,ProgErr);
											return ERROR_ALOK;
										}
											else
												state=ESCAPE_SEKVENCE;
	  							}
	 								else// jakykoliv jiny znak nepovolen
	  								{
										AddError(line,LexErr);
										chybastring (&c,source);
										return LexErr;
	  								}
		break;

//CISLO
		case CISLO: //vynechani pocatecnich nul u cisla
			if (isdigit(c) && c!='0')// jine cislo nez nula
	  		{
				if((strAddChar(attr, c))==1)
				{
					AddError(line,ProgErr);
					return ERROR_ALOK;
				}
					else
						state=CELOCISELNA_CAST;
			}
	  			else if (c=='.')// desetine cislo
	  			{
					if((strAddChar(attr, '0'))==1)
					{
						AddError(line,ProgErr);
						return ERROR_ALOK;
					}
						else if((strAddChar(attr, c))==1)
						{
							AddError(line,ProgErr);
							return ERROR_ALOK;
						}
							else
					 			state=KONTROLA_NEPRAZDNOSTI;
				}
	  				else if (c=='e' || c=='E')// mocnina
					{ 
						if((strAddChar(attr, '0'))==1)
						{
							AddError(line,ProgErr);
							return ERROR_ALOK;
						}
							else if((strAddChar(attr, c))==1)
							{
								AddError(line,ProgErr);
								return ERROR_ALOK;
							}
								else
									state=DESETINE_CISLO;
					}
	  					else if (c!='0')// ukonceni cisla
	  					{ 
							if((strAddChar(attr, 0))==1)
							{
								AddError(line,ProgErr);
								return ERROR_ALOK;
							}
								else
								{
									rest=c;
									*typ=INTEGER;
									return NoErr;
								}
	 					}
		break;

// CELOCISELNA CAST CISLA
		case CELOCISELNA_CAST:
			if (c=='.')// desetine cislo
	  		{ 
				 if((strAddChar(attr, c))==1)
				{
					AddError(line,ProgErr);
					return ERROR_ALOK;
				}
					else
						state=KONTROLA_NEPRAZDNOSTI;
	  		}
	 			else if (c=='e' || c=='E')// mocnina
				{ 
					if((strAddChar(attr, c))==1)
					{
						AddError(line,ProgErr);
						return ERROR_ALOK;
					}
						else
							state=MOCNINA_KONTROLA;
	  			}
	 				else if ((isdigit(c))==0)// ukonceni cisla
	 				{ 
						rest=c;
						*typ= INTEGER;
						return NoErr;
	 				}
	 					else // celociselna cas cisla
						{
							if((strAddChar(attr, c))==1)
							{
								AddError(line,ProgErr);
								return ERROR_ALOK;
							}
						}
		break;
    
// KONTROLA NEPRAZDNOSTI DESETINEHO CISLA
		case KONTROLA_NEPRAZDNOSTI:
			if (isdigit(c))// neprazde - ok
	  		{ 
				if((strAddChar(attr, c))==1)
					{
						AddError(line,ProgErr);
						return ERROR_ALOK;
					}
				state=DESETINE_CISLO;
	  		}
	  			else if (c == EOF) // konec souboru - neadekvatni
				{ 
					AddError(line,LexErr);
					*typ=END_OF_FILE;
					return LexErr;
	 			}
	 				else// chyba
	  				{
						AddError(line,LexErr);
						chyba (&c,source);
						return LexErr;
	  				}
		break;

// DESETINE CISLO
		case DESETINE_CISLO:
			if (c=='e'|| c=='E')// mocnina
			{
				if((strAddChar(attr, c))==1)
				{
					AddError(line,ProgErr);
					return ERROR_ALOK;
				}
					else
					{
						state=MOCNINA_KONTROLA;
						CHECKreal=1;
					}
			}
				else if (isdigit(c))// hodnoty desetineho cisla
	  			{
					if((strAddChar(attr, c))==1)
					{
						AddError(line,ProgErr);
						return ERROR_ALOK;
					}
				}
	 				else// konec cisla   
	  				{  
						rest=c;
					  	*typ= REAL;
					 	return NoErr;
	  				}
		break;
  
// MOCNINA-KONTROLA
		case  MOCNINA_KONTROLA:
			if (c== '+' || c=='-')// hodnota mocniny se znamenkem
	  		{ 
				if((strAddChar(attr, c))==1)
				{
					AddError(line,ProgErr);
					return ERROR_ALOK;
				}
				state=PLUSMINUS;
	  		}
	  			else if (c=='0')// stav na odstraneni nul
					state=KONTROLA_NUL;
	  				else if (isdigit(c))// hodnota mocniny
	  				{ 
						if((strAddChar(attr, c))==1)
						{
							AddError(line,ProgErr);
							return ERROR_ALOK;
						}
						state=MOCNINA;
	  				}
	  					else if (c == EOF) // konec souboru - neadekvatni
	 					{ 
							AddError(line,LexErr);
							*typ=END_OF_FILE;
							return LexErr;
	 					}
	 						else// chyba
	  						{
								AddError(line,LexErr);
								chyba (&c,source);
								return LexErr;
	  						}
		break;
       
// KONTROLA PLUS MINUS U MOCNINY
		case PLUSMINUS:
			if (c=='0')// stav na odstraneni nul
				state=KONTROLA_NUL;
	  			else if (isdigit(c))// hodnoty mocniny
	  			{
					if((strAddChar(attr, c))==1)
					{
						AddError(line,ProgErr);
						return ERROR_ALOK;
					}
					state=MOCNINA;
	  			}
	  				else if (c == EOF) // konec souboru - neadekvatni
	 				{ 
						AddError(line,LexErr);
						*typ=END_OF_FILE;
						return LexErr;
	 				}
	 					else// chyba 
	  					{
							AddError(line,LexErr);
							chyba (&c,source);
	  						return LexErr;
	  					}
		break;
      
// KONTROLA NUL
		case KONTROLA_NUL:
			if (isdigit(c) && c!='0')// hodnoty mocniny - nenulove
	 		{ 
				if((strAddChar(attr, c))==1)
				{
					AddError(line,ProgErr);
					return ERROR_ALOK;
				}
				state=MOCNINA;
	 		}
	 			else if (c!='0')// ukonceni cisla   
				{
					if((strAddChar(attr, 0))==1)
					{
						AddError(line,ProgErr);
						return ERROR_ALOK;
					}
							rest=c;
							if (CHECKreal==1)// real - integer
							{ 
								*typ= REAL;
								return NoErr;
							}
								else 
								{
									*typ= INTEGER;
									return NoErr;
								}
				}
		break;
      
// MOCNINA
		case MOCNINA:
			if (isdigit(c))// hodnoty mocniny
			{
				if((strAddChar(attr, c))==1)
				{
					AddError(line,ProgErr);
					return ERROR_ALOK;
	  			}
			}
				else// ukonceni cisla
				{ 
					rest=c;
					if (CHECKreal==1)// rozliseni real-int
					{
						*typ= REAL;
						return NoErr;
					}
						else 
						{ 
							*typ= INTEGER;
							return NoErr;
						}
				}
		break;

// KONKATENACE
		case KONKATENACE:
			if (c=='.')// konkatenace
	  		{ 
				if((strAddChar(attr, c))==1)
				{
					AddError(line,ProgErr);
					return ERROR_ALOK;
				}
					else
					{
						*typ= OPERATOR;
						return NoErr;
					}
	  		}
	  			else if (c == EOF) // konec souboru - neadekvatni
	 			{ 
					*typ=END_OF_FILE;
					return LexErr;
	 			}
					else// chyba
	  				{
						AddError(line,LexErr);
						 chyba (&c,source);
						 return LexErr;
	  				}
		break;

// == <= >=
		case DVOUZNAKOVE_OPERATORY:
			if (c=='=')// dvou znakove operatory
			{
				if((strAddChar(attr, c))==1)
				{
					AddError(line,ProgErr);
					return ERROR_ALOK;
				}
				else
				{
					*typ= OPERATOR;
					return NoErr;
				}
			}
	    		else if (c == EOF) // konec souboru - adekvatni
				{ 
					*typ=END_OF_FILE;
					return NoErr;
				}
					else// jednoznakovy operator
					{	
						rest=c;
						*typ= OPERATOR;
						return NoErr;
					}
		break;

// ~=
		case DVOUZNAKOVY_OPERATOR:
			if (c=='=')// dvouznakovy operator
			{ 
				if((strAddChar(attr, c))==1)
				{
					AddError(line,ProgErr);
					return ERROR_ALOK;
				}
					else
					{
						*typ= OPERATOR;
						return NoErr;
					}
			}
				else 
				{
					AddError(line,LexErr);
					chyba (&c,source);
					return LexErr;
				}
		break;
		} // konec switch
	}// konec while
} // konec fce


