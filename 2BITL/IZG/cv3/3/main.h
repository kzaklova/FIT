
#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

/**
 *	\file main.h
 *	\author	Vit Stancl 
 *	\brief	Deklarace rozhrani a funkci pro cviceni c. 5 z IZG
 *	
 *	Zalozeno na prikladu Michala Spanela. Cast testu nasledne pouzitych pro 
 *  kontrolu DU je v souboru tests.cpp. 
 */

///////////////////////////////////////////////////////////////////////////////
//
// Kod do cviceni #5 z predmetu Zaklady pocitacove grafiky
//
// Autor: Michal Spanel, Vit Stancl, Igor Potucek
//        Michal Seeman
//
// Obsah cviceni: Vyplnovani uzavrenych objektu
//
// 1) Uvod do problematiky vyplnovani uzavrenych oblasti
//
// 2) Seminkove vyplnovani bez pouziti rekurze (s pouzitim zasobniku).
//
// 3) Inverzni radkove vyplnovani
//
// 4) Ovladani programu:
//      a) kliknuti mysi levym tlacitkem
//  			 		- pro seminkove vyplnovani je to vlozeni seminka
//						- pro Inverzni vyplnovani je to pocatek zadavani vrcholu polygonu (konec prostrednim tlacitkem)
//      b) kliknuti mysi pravym tlacitkem - zobrazeni popup menu pro vyber
//         vyplnovaci metody, vymazani frame bufferu apod.
//
//      c) stisknuti klaves "C, c" - vymazani frame bufferu
//      d) stisknuti klaves "S,s" - ulozeni obrazovky do bitmapy 'save.bmp'
//      e) stisknuti klaves "L, l" - nacteni souboru, ktery byl predan jako parametr prikazove radky.
//      f) stisknuti klaves "Esc, Q, q, X, x" - ukonceni programu
//      g) stisknuti klaves "F, f" - prepnuti do celoobrazovkoveho modu
//      h) stisknuti klaves "W, w" - vraceni z celoobrazovkoveho modu
//		i) Pokud je jako parametr pouzito jmeno souboru *.bmp a tento je ve 
//				 formatu 24 bitu na pixel, program se jej pokusi nacist. Tento soubor
//				 pote pouzije pri stisku "l" jako pozadi.
//		j) V menu jsou testy (podmenu tests). Je mozne je spoustet postupne, nebo vsechny najednou. Po spusteni (pokud 
//			je vas algoritmus alespon trochu funkcni) se ulozi vysledny obrazek do adresare. 
//
//
//  Kod vznikal radu let a podle toho take vypada :) 
//
// Opravy:
//
///////////////////////////////////////////////////////////////////////////////
    
// Include soubory
#include <vector>	// Zasobnik pro seminkove vyplnovani
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Definice vzdalenosti barvy hranice od barvy oblasti
#define MAX_DISTANCE 5.0

// Standardni namespace
using namespace std;

typedef unsigned char UCHAR;

//Definice typu prejaty z predchozich projektu
// struktura typu RGBA barvy, 32 bit
struct S_RGBA
{
  UCHAR red;
  UCHAR green;
  UCHAR blue;
  UCHAR alpha;
  
  //! Konstruktory
  S_RGBA(UCHAR r, UCHAR g, UCHAR b, UCHAR a = 255):
		red(r), green(g), blue(b), alpha(a)
		{}
	S_RGBA(): red(0), green(0), blue(0), alpha(255)
	{}
  S_RGBA(const S_RGBA & c)
      : red(c.red), green(c.green), blue(c.blue), alpha(c.alpha)
  {}

  // Operator prirazeni
  S_RGBA operator=(const S_RGBA & c)
  {
    red = c.red; green = c.green; blue = c.blue; alpha = c.alpha;
    return c;
  }
};

bool operator == (const S_RGBA & c1, const S_RGBA & c2);
bool operator != (const S_RGBA & c1, const S_RGBA & c2);


// bod ve 2D spolu s barvou
struct S_Point
{
  int x, y;         // souradnice bodu
                    // konstruktory (C++)
  S_Point() {}
  S_Point(int _x, int _y) : x(_x), y(_y) {}
  S_Point(const S_Point& p) : x(p.x), y(p.y) {}

  S_Point operator=(const S_Point & p)
  {
    x = p.x; y = p.y;
    return p;
  }
};

// usecka v 2D
struct S_Line
{
  S_Point point1, point2;	//pociatocny a koncovy bod

  S_Line() {}
  S_Line(int _x1, int _y1, int _x2, int _y2) 
  {
    point1.x = _x1; point1.y = _y1;
	point2.x = _x2; point2.y = _y2;
  }
  S_Line(const S_Point& p1, const S_Point& p2) 
  {
    point1 = p1;
	point2 = p2;
  }

  S_Line operator=(const S_Line & l)
  {
	  point1 = l.point1;
	  point2 = l.point2;
    return l;
  }
};

struct S_Mask
{
  char **mask;         // souradnice bodu
                    // konstruktory (C++)
  S_Mask(int height, int width) 
  {
	  mask = (char**)malloc(height * sizeof(mask[0]));

	  for(int a = 0; a < height; a++) 
	  {
		mask[a] = (char*)malloc(width * sizeof(mask[0][0]));
		//reset value
		memset(mask[a], 0, width);
	  }
  }
};



// Typ - zasobnik bodu pro seminka
typedef vector<S_Point> SeedStack;

// Typ - zasobnik hranic
typedef vector<S_Line> LineStack;

///////////////////////////////////////////////////////////////////////////////
// globalni promenne

extern S_RGBA           * frame_buffer;         // ukazatel na pamet RGB hodnot vlastniho frame bufferu
extern int                frame_w, frame_h;        // velikost frame bufferu


///////////////////////////////////////////////////////////////////////////////
// uzitecna makra

// prohodi hodnoty dvou promennych s pouzitim jedne pomocne
#define SWAP(a, b)          { (b) ^= (a); (a) ^= (b); (b) ^= (a); }

// zaokrouhli hodnotu typu double na int
#define ROUND(x)            ((int)((x) + 0.5))

// minimum ze dvou zadanych hodnot
#define MIN(a, b)           (((a) < (b)) ? (a) : (b))

// maximum ze dvou zadanych hodnot
#define MAX(a, b)           (((a) > (b)) ? (a) : (b))

// makro pro primy pristup k hodnote pixelu ve frame bufferu
#define PIXEL(x, y)         (*(frame_buffer + (y) * frame_w + (x)))

// porovnani dvou barev (struktur S_RGBA)
//#define IS_EQUAL(c1, c2)    (*((unsigned *)&(c1)) == *((unsigned *)&(c2)))


///////////////////////////////////////////////////////////////////////////////
// funkce

//! Funkce pro nastaveni  barvy bodu - netestuje, aby nezapsala mimo rozsah bufferu!!!
void PutPixel(int x, int y, const S_RGBA & color);

//! Funkce pro zjisteni barvy bodu - netestuje, aby necetla mimo rozsah!!!
void GetPixel(int x, int y, S_RGBA & color);

// ulozeni obsahu obrazovky do souboru ve formatu BMP (save.bmp)
int SaveBitmap(const char * filename, S_RGBA * frame_buffer, int Width, int Height);


// Smazani framebufferu
void clearFB();

// Konverze c++ vektoru do c pole
S_Point * stackToArray( SeedStack & stack );

// Testovaci funkce - index je cislo testu - dalsi info v souboru tests.cpp 
void runTest(int test);
// Testovaci funkce * SMAZAT *
void runAllMakeTiled();


///////////////////////////////////////////////////////////////////////////////
// funkce domaciho ukolu a cviceni

// Inverzni radkove vyplnovani
void InvertFill(const S_Point * points, const int size,  const S_RGBA & color1, const S_RGBA & color2);

// seminkove vyplnovani
void FloodFill(int x, int y, const S_RGBA & color);


#endif // MAIN_H_INCLUDED

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
