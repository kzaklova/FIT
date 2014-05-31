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


///////////////////////////////////////////////////////////////////////////////
// include

#ifdef _WIN32
  #include <windows.h>
#endif

#include "main.h"

#include <assert.h>
#include <float.h>
#include <stdlib.h>
#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>


///////////////////////////////////////////////////////////////////////////////
// name spaces

using namespace std;


///////////////////////////////////////////////////////////////////////////////
// globalni konstanty

// titulek hlavniho okna
const char         * PROGRAM_TITLE = "IZG Labs #05, Vyplnovani uzavrenych objektu";

// barvy pouzivane v programu

const S_RGBA       BLUE(0, 0, 255, 255);
const S_RGBA       GREEN(0, 255, 0, 255);
const S_RGBA       RED(255, 0, 0, 255);
const S_RGBA       GREY(190, 190, 190, 255);


///////////////////////////////////////////////////////////////////////////////
// globalni promenne

S_RGBA       * frame_buffer;          // ukazatel na pamet RGB hodnot vlastniho frame bufferu

int          frame_w, frame_h;        // velikost frame bufferu


const int          default_frame_w   = 499;   // defaultni velikost okna
const int          default_frame_h   = 499;

int                mouse_x, mouse_y;          // poloha kurzoru mysi
int                last_x = -1, last_y = -1;  // posledni zadany hranicni bod

// Vektor bodu pro Pineaduv algoritmus
SeedStack points;
S_Point point;
bool isOld = false;

// Nazev souboru, ktery se ma nahrat
char loaded_file[256];
// Priznak, zda se ma nahrat soubor (podle parametru)
bool bLoadFile(false);
bool bFileLoaded(false);


////////////////////////////////////////////////////////////////////////////////////////////////
// globalni promenne domaciho ukolu

// hranicni body uzavrene oblasti
//T_PointVector       boundary_points;

// kreslici barva
S_RGBA              color = RED;


////////////////////////////////////////////////////////////////////////////////////////////////
// popup menu

// prikazy popup menu
enum E_MenuCommand
{
  MAIN_MENU_FLOOD_FILL,
  MAIN_MENU_SCANLINE_FILL,
  MAIN_MENU_CLEAR,
  MAIN_MENU_FULL_SCREEN,
  MAIN_MENU_WINDOW,
  MAIN_MENU_QUIT,
  MAIN_MENU_LOAD_BITMAP,
  MAIN_MENU_SAVE_BITMAP
};

enum E_SubmenuColor
{
	COLOR_MENU_RED,
	COLOR_MENU_GREEN,
	COLOR_MENU_BLUE
};

// Submenu test
enum E_SubmenuTest
{
	TEST_MENU_ALL = 0,
	TEST_MENU_1 = 1,
	TEST_MENU_2 = 2,
	TEST_MENU_3 = 3,
	TEST_MENU_4 = 4,
	TEST_MENU_5 = 5,
	TEST_MENU_6 = 6,
	TEST_MENU_7 = 7,
	TEST_MENU_8 = 8,
	TEST_MENU_9 = 9,
	TEST_MENU_10 = 10
};

// Pouzity algoritmus
enum UseAlg
{
	USE_FLOOD_FILL,
	USE_SCANLINE_FILL
};

// identifikator popup menu
int main_menu, color_menu, tile_menu, test_menu;

// Stav pouziti algoritmu
UseAlg algorithm;

///////////////////////////////////////////////////////////////////////////////
// struktury pro praci s graficky formatem souboru BMP

// zarovnani scanline na 4byte
#define DIB_SCAN_LINE(WIDTH) (((WIDTH) & 0xfffffffc) + (((WIDTH) & 0x3) ? 4 : 0))

// GRAFICKA STRUKTURA - hlavicka bmp souboru
#ifndef _WIN32 // Windows maji nasledujici struktury definovany ve windows.h, Linux ne
#pragma pack(2) // zarovnani pameti na 2byty
typedef short WORD;                  // 2 byte
typedef int DWORD;                   // 4 byte

typedef struct
{
    WORD           bfType;           /* Oznaceni BM - typ souboru - 2byte*/
    DWORD          bfSize;           /* Velikost souboru - 4byte*/
    WORD           bfReserved1;      /* rezervovane */
    WORD           bfReserved2;      /* rezervovane */
    DWORD          bfOffBits;        /* Offset na obrazova data */
} BITMAPFILEHEADER;

// GRAFICKA STRUKTURA - informace o formatu ulozeneho obrazu
typedef struct
{
    DWORD          biSize;           /* velikost informacni hlavicky */
    DWORD          biWidth;          /* sirka obrazu */
    DWORD          biHeight;         /* vyska obrazu */
    WORD           biPlanes;         /* pocet barevnych rovin */
    WORD           biBitCount;       /* pocet bitu na pixel */
    DWORD          biCompression;    /* typ pouzite komprese */
    DWORD          biSizeImage;      /* velikost obrazovych dat */
    DWORD          biXPelsPerMeter;  /* pocet X pixelu na metr */
    DWORD          biYPelsPerMeter;  /* pocet Y pixelu na metr */
    DWORD          biClrUsed;        /* pocet pouzitych barev */
    DWORD          biClrImportant;   /* pocet dulezitych barev */
} BITMAPINFOHEADER;
#pragma pack() // default memory alignment
#endif // _WIN32

// Konstanty promenne biCompression - typ pouzite komprese dat
#define BIF_RGB       0               /* zadna komprese */
#define BIF_RLE8      1               /* 8-bit RLE */
#define BIF_RLE4      2               /* 4-bit RLE */
#define BIF_BITFIELDS 3               /* RGB bitove pole s maskou */


///////////////////////////////////////////////////////////////////////////////
// funkce zajistujici zobrazeni pixelu dane barvy na danych souradnicich

void PutPixel(int x, int y, const S_RGBA & color)
{
  // test existence frame bufferu
  assert(frame_buffer != NULL);

  // test rozmeru okna
  if ((x >= 0) && (x < frame_w) && (y >= 0) && (y < frame_h))
    // zapsani dane barvy do pameti frame bufferu
    *(frame_buffer + y*frame_w + x) = color;
  else

      std::cout << "Zapis mimo okno!!!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
// funkce zajistujici ziskani barvy pixelu na danych souradnicich

void GetPixel(int x, int y, S_RGBA & color)
{
  // test existence frame bufferu
  assert(frame_buffer != NULL);

  // test rozmeru okna
  if ((x >= 0) && (x < frame_w) && (y >= 0) && (y < frame_h))
    // ziskani barvy z pameti frame bufferu
    color = *(frame_buffer + y*frame_w + x);
  else
  {
    color.red = 0;
    color.green = 0;
    color.blue = 0;
    color.alpha = 0;
  }
}

//! Smazani framebufferu
void clearFB(){ memset( frame_buffer, 0, frame_h*frame_w*sizeof( S_RGBA ) ); }

//! Porovnani dvou barev podle slozek
bool operator == (const S_RGBA & c1, const S_RGBA & c2){
	return (c1.alpha == c2.alpha)&&(c1.red == c2.red)&&(c1.green == c2.green)&&(c1.blue == c2.blue);
}

//! Nonekvivalence dvou barev
bool operator != (const S_RGBA & c1, const S_RGBA & c2){
	return !(c2 == c1);
}

//!  Ulozeni obsahu obrazovky do souboru ve formatu BMP (save.bmp)

int SaveBitmap(const char * filename, S_RGBA * buffer, int Width, int Height)
{ // vraci 0 - chyba, 1 - OK
  FILE *fw = fopen(filename, "wb");
  if (fw == NULL)
  {
    cout << "Cannot open file !" << filename << "'\n";
    return 0;
  }

  BITMAPFILEHEADER head;
  head.bfType         = 0x4d42;
  head.bfSize         = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
                        DIB_SCAN_LINE(Width * 3) * Height;
  head.bfReserved1    = 0;
  head.bfReserved2    = 0;
  head.bfOffBits      = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

  BITMAPINFOHEADER info;
  info.biSize             = sizeof(BITMAPINFOHEADER);
  info.biWidth            = Width;
  info.biHeight           = Height;
  info.biPlanes           = 1;
  info.biBitCount         = 24;
  info.biCompression      = 0;
  info.biSizeImage        = DIB_SCAN_LINE(Width * 3) * Height;
  info.biXPelsPerMeter    = 0;
  info.biYPelsPerMeter    = 0;
  info.biClrUsed          = 0;
  info.biClrImportant     = 0;
  fwrite(&head, sizeof(BITMAPFILEHEADER), 1, fw);
  fwrite(&info, sizeof(BITMAPINFOHEADER), 1, fw);

  int zar = DIB_SCAN_LINE(info.biWidth * 3) - info.biWidth * 3;
  S_RGBA color;

  for (int y = 0; y < info.biHeight; y++)
  {
    for (int x = 0; x < info.biWidth; x++)
	{
      color = buffer[x + y * Width ];
      fputc(color.blue, fw);
      fputc(color.green, fw);
      fputc(color.red, fw);
    }
    for (int x = 0; x < zar; x++)
	  fputc(0, fw);
  }
  fclose(fw);
  return 1;
}


//! Nacteni BMP souboru z disku. Pouze format 24b - 8 bitu na slozku (RGB).
int LoadBitmap(const char *filename, S_RGBA * &frame_buffer, int & width, int & height){

	FILE *fr = fopen(filename, "rb");
	if (fr == NULL)
  {
    cout << "Cannot open file !" << filename << "'\n";
    return 0;
  }else{
		cout << "File " << filename << " opened" << endl;
	}
	
	BITMAPFILEHEADER head;
	fread(&head, sizeof(BITMAPFILEHEADER), 1, fr);
	BITMAPINFOHEADER info;
	fread(&info, sizeof(BITMAPINFOHEADER), 1, fr);
	
	int zar = DIB_SCAN_LINE (info.biWidth * 3) - info.biWidth * 3;
	
	cout << "Size: " << info.biHeight << " x " << info.biWidth << endl;
	cout << "Kern: " << zar << endl;
/*	
	if(frame_buffer == NULL){
		width = info.biWidth;
		height = info.biHeight;
		frame_buffer = new S_RGBA[width * height];
	}
*/
	S_RGBA color;
	int maxx, maxy;
	maxx = MIN(info.biWidth, width);
	maxy = MIN(info.biHeight, height);
	int shift = info.biWidth - maxx;

	for(int y = 0; y < maxy; y++){
		for(int x = 0; x < maxx; x++){
			color.blue = fgetc(fr);
			color.green = fgetc(fr);
			color.red = fgetc(fr);
			
			frame_buffer[x + y*width] = color;
		}
		
		for(int x = 0; x < 3*shift; x++)
			fgetc(fr);

		for (int x = 0; x < zar; x++)
      fgetc (fr) ;
  }
	
	fclose(fr);
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
// funkce pro inicializaci parametru programu

void onInit(void)
{
  // inicializace globalnich promennych
  frame_buffer = NULL;

  frame_w = 0;
  frame_h = 0;

  // nastaveni formatu ulozeni pixelu v bitmape
  glPixelStorei(GL_UNPACK_ALIGNMENT,4);
  // nastaveni barvy pozadi
  glClearColor(0.0, 0.0, 0.0, 0.0);
  
  // Nastaveni pouziteho algoritmu na flood fill
  algorithm = USE_FLOOD_FILL;

  color = S_RGBA( 0, 255, 0 );
}

///////////////////////////////////////////////////////////////////////////////
// funkce reagujici na zmenu rozmeru okna
// w, h - nova velikost okna

void onResize(int w, int h)
{
  S_RGBA *new_frame_buffer;         // ukazatel na pamet noveho frame bufferu

  // nastaveni viditelne oblasti OpenGL okna pres cele okno programu
  glViewport(0, 0, w, h);

  // nastaveni modifikace projekcni matice
  glMatrixMode(GL_PROJECTION);

  // inicializace projekcni matice, nastaveni jednotkove matice
  glLoadIdentity();

  // nastaveni paralelni projekce zobrazeni obsahu OpenGL okna, mapovani abstraktnich souradnic OpenGL na souradnice okna
  glOrtho(0, w, 0, h, -1, 1);

  // alokace pameti noveho frame bufferu
  new_frame_buffer = new S_RGBA[w * h];
  assert(new_frame_buffer != NULL);
  
  // vymazani obsahu noveho frame bufferu
  clearFB();

  // pripadna kopie obsahu stareho frame bufferu do noveho
  if( frame_buffer != NULL )
  {
    int       min_w;        // minimalni sirka frame bufferu
    int       min_h;        // minimalni vyska frame bufferu
    int       dh;           // rozdil vysek oken, pro urceni pocatku kopirovane oblasti u noveho okna
    int       frame_dh;     // rozdil vysek oken, pro urceni pocatku kopirovane oblasti u stareho okna

    // urceni minimalni sirky a vysky frame bufferu
    min_w = (w < frame_w) ? w : frame_w;
    min_h = (h < frame_h) ? h : frame_h;

    // urceni pocatku kopirovanych dat
    dh = (frame_h > h) ? 0 : (h - frame_h);
    frame_dh = (frame_h > h) ? (frame_h - h) : 0;

    // castecna kopie obsahu frame bufferu
    for( int i = 0; i < min_h; i++ )
      memcpy((new_frame_buffer + ((i + dh) * w)), (frame_buffer + ((i + frame_dh) * frame_w)), min_w * sizeof(S_RGBA));
  }

  // nastaveni novych rozmeru frame bufferu
  frame_w = w;
  frame_h = h;

  // zruseni pripadne existujici pameti frame bufferu
  if (frame_buffer != NULL)
    delete[] frame_buffer;

  // nastaveni noveho frame bufferu
  frame_buffer = new_frame_buffer;

  if(bLoadFile&&bFileLoaded){
  	LoadBitmap(loaded_file, frame_buffer, frame_w, frame_h);
  	bFileLoaded = true;
  }
}

///////////////////////////////////////////////////////////////////////////////
// funkce zajistujici prekresleni obsahu okna programu

void onDisplay(void)
{
  // test existence frame bufferu
  assert(frame_buffer != NULL);

  // vymazani bitovych rovin barevneho bufferu
  glClear(GL_COLOR_BUFFER_BIT);

  // nastaveni kresleni do barevneho RGBA OpenGL bufferu
  glDrawBuffer(GL_COLOR);
  // nastaveni souradnice leveho dolniho rohu pixmapy
  glRasterPos2i(0, 0);
  // vykresleni pixmapy
  glDrawPixels(frame_w, frame_h, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char *)frame_buffer);

  // realizace, vykresleni provedenych prikazu zobrazeni
  glFlush();
  // vymena zobrazovaneho a zapisovaneho bufferu
  glutSwapBuffers();
}



///////////////////////////////////////////////////////////////////////////////
// funkce volana pri vyberu prikazu z hlavni menu

void onMainMenu(int command)
{
  // parametry puvodniho umisteni okna
  static int old_frame_h, old_frame_w, old_window_x, old_window_y;

  isOld = false;

  switch (command)
  {
    // seminkove vyplnovani
    case MAIN_MENU_FLOOD_FILL:
 			algorithm = USE_FLOOD_FILL;
			
      break;
      
    case MAIN_MENU_SCANLINE_FILL:
    	algorithm = USE_SCANLINE_FILL;
			points.erase(points.begin(), points.end());
			
    	break;

	// Nacti pozadi plochy
	case MAIN_MENU_LOAD_BITMAP:
		if(strlen(loaded_file) > 0)
        {
		
            clearFB();
        
		    LoadBitmap(loaded_file, frame_buffer, frame_w, frame_h);
    //		onResize(frame_w, frame_h);
		    bFileLoaded = true;
		} else {
			clearFB();
        
		    LoadBitmap("scooby.bmp", frame_buffer, frame_w, frame_h);
    //		onResize(frame_w, frame_h);
		    bFileLoaded = true;
		}
		break;


    // Uloz obrazek
    case MAIN_MENU_SAVE_BITMAP:
        SaveBitmap( "save.bmp", frame_buffer, frame_w, frame_h );
        break;

   

    // vymazani frame bufferu
    case MAIN_MENU_CLEAR:
      clearFB();
      
	  break;

    // ukonceni programu
    case MAIN_MENU_QUIT:
      exit(0);
      break;

    // prepnuti do celoobrazovkoveho modu
    case MAIN_MENU_FULL_SCREEN:
      old_frame_h = frame_h;
      old_frame_w = frame_w;
      old_window_x = glutGet(GLUT_WINDOW_X);
      old_window_y = glutGet(GLUT_WINDOW_Y);
      glutFullScreen();
      break;

    // vraceni z celoobrazovkoveho modu
    case MAIN_MENU_WINDOW:
      // nastaveni ulozenych rozmeru okna
      if( old_frame_w > 0 )
      {
        glutPositionWindow(old_window_x, old_window_y);
        glutReshapeWindow(old_frame_w, old_frame_h);
      }
      else
      { // defaultni rozmer frame bufferu
        glutReshapeWindow(default_frame_w, default_frame_h);
      }
      break;

    default:
      break;
  }

  // vyvolani prekresleni OpenGL okna
  glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////////////////
// Funkce pro vyber v podmenu barev
void onColorMenu(int command)
{
	// vyber kreslici barvy
	switch(command){
	 case COLOR_MENU_RED:
		 color = RED;
		 break;
	 case COLOR_MENU_GREEN:
		 color = GREEN;
		 break;
	 case COLOR_MENU_BLUE:
		 color = BLUE;
		 break;
	}
}



///////////////////////////////////////////////////////////////////////////////
// Funkce pro vyber v podmenu testu
void onTestMenu(int command)
{
	char fname[128];

	if(command == 0)
	{
        runAllMakeTiled();

        // Clear buffer
		clearFB();

        // vyvolani prekresleni OpenGL okna
        glutPostRedisplay();

		return;
	}

	// Clear buffer
	clearFB();

	runTest(command);
	sprintf(fname, "test%i.bmp", command);
	SaveBitmap(fname, frame_buffer, frame_w, frame_h);

    // vyvolani prekresleni OpenGL okna
    glutPostRedisplay();
}


///////////////////////////////////////////////////////////////////////////////
// funkce reagujici na stisknuti klavesnice
// key - kod stisknute klavesy
// x, y - souradnice kurzoru pri stisknuti klavesy

void onKeyboard(unsigned char key, int x, int y)
{
  // uprava velkych pismen na mala
  if (key >= 'A' && key <= 'Z')
    key += 'a' - 'A';

  // vetveni podle stisknute klavesy
  switch (key)
  {
    // ukonceni programu
    case 27:                                // klavesa Escape
    case 'q':
    case 'x':
      exit(0);
      break;

    // vyber kreslici barvy
    case 'r':
      onColorMenu(COLOR_MENU_RED);
      break;
    case 'g':
      onColorMenu(COLOR_MENU_GREEN);
      break;
    case 'b':
      onColorMenu(COLOR_MENU_BLUE);
      break;

    // vymazani frame bufferu
    case 'c':
      onMainMenu(MAIN_MENU_CLEAR);
      break;

    // prepnuti do celoobrazovkoveho modu
    case 'f':
      onMainMenu(MAIN_MENU_FULL_SCREEN);
      break;

    // vraceni z celoobrazovkoveho modu
    case 'w':
      onMainMenu(MAIN_MENU_WINDOW);
      break;

	// Znovu nacti data
	case 'l':
      onMainMenu(MAIN_MENU_LOAD_BITMAP);
	  break;

    // ulozi obrazek
    case 's':
      fprintf(stdout,"Ukladam obrazek %dx%d %s\n", frame_w, frame_h,"save.bmp");
      SaveBitmap("save.bmp", frame_buffer, frame_w, frame_h);
      break;

    default:
      break;
  }

  // vyvolani prekresleni OpenGL okna
  glutPostRedisplay();
}

// Create C array from the vector of points
S_Point * stackToArray( SeedStack & stack )
{
    S_Point * points = new S_Point[ stack.size() ];

    SeedStack::iterator i;

    int j(0);

    for( i = stack.begin(); i != stack.end(); ++i )
        points[ j++ ] = *i;

    return points;
}

///////////////////////////////////////////////////////////////////////////////
// funkce reagujici na stisknuti tlacitka mysi
// button - kod stisknuteho tlacitka
// state - stav tlacitka - stisknute/uvolnene
// x, y - souradnice kurzoru pri stisknuti klavesy

void onMouseClick(int button, int state, int x, int y)
{
  // obraceni osy y
  y = frame_h - y - 1;

  // vetveni podle stisknuteho tlacitka mysi
  switch (button)
  {
    // leve tlacitko mysi
    case GLUT_LEFT_BUTTON:
      // vetveni podle stisknuti nebo pusteni tlacitka
      if (state == GLUT_DOWN)   // stisknuto
      {
      
      	// Proved akci podle algoritmu
      	switch(algorithm){
				
				case USE_SCANLINE_FILL:
					
//					if(isOld)
                        PutPixel( x, y, color );
//						Bresenham(point.x, point.y, x, y, color);

					point = S_Point(x, y);
					points.push_back(point);
					isOld = true;
					break;

				case USE_FLOOD_FILL:
					FloodFill(x, y, color);
					break;

				
				}
		     // ulozeni souradnic
  		  last_x = x;
	   	  last_y = y;
      }
      break;

    // prostredni tlacitko mysi
    case GLUT_MIDDLE_BUTTON:
      // vetveni podle stisknuti nebo pusteni tlacitka
      if (state == GLUT_DOWN)   // stisknuto
      {
		  if(algorithm == USE_SCANLINE_FILL && isOld)
		  {
              S_Point * pointa = stackToArray( points );
			  // Fill polygon
              InvertFill(pointa, (const int )points.size(),  color, S_RGBA(0,0,0));
              delete [] pointa;
			  points.clear();
			  isOld = false;
		  }
      }
      break;

    // prave tlacitko mysi
    case GLUT_RIGHT_BUTTON:
      // vetveni podle stisknuti nebo pusteni tlacitka
      if (state == GLUT_DOWN)   // stisknuto
      {


      }
      break;
  };

  // vyvolani prekresleni OpenGL okna
  glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////////////////
// funkce reagujici na pohyb mysi bez stisknutych tlacitek
// x, y - aktualni souradnice kurzoru

void onPassiveMouseMotion(int x, int y)
{
  // obraceni osy y
  y = frame_h - y - 1;

  // ulozeni aktualni souradnice mysi
  mouse_x = x;
  mouse_y = y;
}

///////////////////////////////////////////////////////////////////////////////
// Volano pri ukonceni programu
void onDestroy()
{
	if(frame_buffer != NULL)
		delete [] frame_buffer;
}

///////////////////////////////////////////////////////////////////////////////
// Vytvoreni menu
void createMenu()
{
	// Inicializace menu barev
	color_menu = glutCreateMenu(onColorMenu);
	glutAddMenuEntry("Set Color [Red] \tr", COLOR_MENU_RED);
	glutAddMenuEntry("Set Color [Green] \tg", COLOR_MENU_GREEN);
	glutAddMenuEntry("Set Color [Blue] \tb", COLOR_MENU_BLUE);


	// Inicializace menu testu
	test_menu = glutCreateMenu(onTestMenu);
	glutAddMenuEntry("All tests", TEST_MENU_ALL);
	glutAddMenuEntry("Test 1", TEST_MENU_1);
	glutAddMenuEntry("Test 2", TEST_MENU_2);
	glutAddMenuEntry("Test 3", TEST_MENU_3);
	glutAddMenuEntry("Test 4", TEST_MENU_4);
	glutAddMenuEntry("Test 5", TEST_MENU_5);
	glutAddMenuEntry("Test 6", TEST_MENU_6);
	glutAddMenuEntry("Test 7", TEST_MENU_7);
	glutAddMenuEntry("Test 8", TEST_MENU_8);
	glutAddMenuEntry("Test 9", TEST_MENU_9);
	glutAddMenuEntry("Test 10", TEST_MENU_10);

	// inicilizace hlavniho popup menu
	main_menu = glutCreateMenu(onMainMenu);
	glutAddMenuEntry("Flood Fill", MAIN_MENU_FLOOD_FILL);
	glutAddMenuEntry("Invert Fill", MAIN_MENU_SCANLINE_FILL);
	glutAddSubMenu("Select color", color_menu);
	glutAddSubMenu("Tests", test_menu);
	glutAddMenuEntry("Load Bitmap \tl", MAIN_MENU_LOAD_BITMAP);
    glutAddMenuEntry("Save Bitmap \ts", MAIN_MENU_SAVE_BITMAP);
	glutAddMenuEntry("Clear \tc", MAIN_MENU_CLEAR);
	glutAddMenuEntry("Full Screen \tf", MAIN_MENU_FULL_SCREEN);
	glutAddMenuEntry("Default Window \tw", MAIN_MENU_WINDOW);
	glutAddMenuEntry("Quit \tx", MAIN_MENU_QUIT);

	// vyber aktivniho menu
	glutSetMenu(main_menu);
	// prirazeni menu pravemu tlacitku
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

///////////////////////////////////////////////////////////////////////////////
// hlavni funkce programu
// argc - pocet vstupnich parametru
// argv - pole vstupnich parametru

int main(int argc, char **argv)
{
  // inicializace knihovny GLUT
  glutInit(&argc, argv);

  // inicializaci parametru programu
  onInit();

  // nastaveni grafickeho modu okna programu
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
  // nastaveni pocatecnich rozmeru okna programu
  glutInitWindowSize(default_frame_w, default_frame_h);
  // vytvoreni hlavniho okna pogramu
  glutCreateWindow(PROGRAM_TITLE);

  // registrace fukce zajistujici prekresleni obsahu okna programu
  glutDisplayFunc(onDisplay);
  // registrace funkce reagujici na zmenu rozmeru okna programu
  glutReshapeFunc(onResize);
  // registrace funkce reagujici na stisknuti klavesnice
  glutKeyboardFunc(onKeyboard);
  // registrace funkce reagujici na stisknuti tlacitka mysi
  glutMouseFunc(onMouseClick);
  // registrace funkce reagujici na pohyb mysi bez stisknutych tlacitek
  glutPassiveMotionFunc(onPassiveMouseMotion);

  // Vytvoreni vsech menu
  createMenu();


  // Zpracovani prikazove radky
  if(argc > 1){
	
		bLoadFile = true;
		strcpy(loaded_file, argv[1]);
  }

  // nekonecna smycka volajici registrovane funkce
  glutMainLoop();

  // Vymaz buffery
  onDestroy();

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
