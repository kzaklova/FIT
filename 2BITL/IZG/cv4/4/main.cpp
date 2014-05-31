///////////////////////////////////////////////////////////////////////////////
//
// Kod do cviceni #4 z predmetu Zaklady pocitacove grafiky
//
// Autor: Michal Spanel, Jiri Venera
//
// Obsah cviceni: Zobrazovani 2D krivek
//
// 1) Uvod do problematiky zobrazovani 2D krivek
//
// 2) Fergussnova kubika
//
// 2) Bezierovy krivky
//
// 5) Ovladani programu:
//      a) kliknuti mysi levym tlacitkem - zadavani ridicich bodu krivky
//      b) kliknuti mysi levym tlacitkem v okoli ridiciho bodu - uchopeni a posun bodu
//      c) kliknuti mysi pravym tlacitkem - zobrazeni popup menu
//
//      d) stisknuti klaves "D, d" - odstraneni "aktivniho/vybraneho" ridiciho bodu
//      e) stisknuti klaves "n" - vyber NURBS vykreslovani
//      f) stisknuti klaves "b" - vyber metody pro vykresleni racionalni Bezierovy kubiky
//
//      g) stisknuti klaves "S,s" - ulozeni obrazovky do bitmapy 'save.bmp'
//      h) stisknuti klaves "Esc, Q, q, X, x" - ukonceni programu
//      i) stisknuti klaves "F, f" - prepnuti do celoobrazovkoveho modu
//      j) stisknuti klaves "W, w" - vraceni z celoobrazovkoveho modu
//
// Opravy:
//      2005/03/21: "Neznamy student"
//      Opravena chyba pri zobrazovani popup menu v linuxu.
//      Muj dik patri neznamemu studentu. Pokud se pachatel prihlasi
//      a prokaze svuj puvod, bude slavny, nebot tady pripisu jeho jmeno :)
//      Jinak diky za bugfix.
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// include

#ifdef _WIN32
  #include <windows.h>
#endif
#ifdef _WIN64
  #include <windows.h>
#endif

#include "main.h"

#include <GL/gl.h>
#include <GL/glut.h>
#include <assert.h>
#include <float.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>

#include <cstring>

///////////////////////////////////////////////////////////////////////////////
// name spaces

using namespace std;


///////////////////////////////////////////////////////////////////////////////
// globalni konstanty

// titulek hlavniho okna
const char *PROGRAM_TITLE[] =
{
  "IZG Labs #04, Fergusson and Bezier",
  "IZG Labs #04, Fergusson and Bezier",
  "IZG Labs #04, Fergusson and Bezier"
};

enum E_Method
{
    METHOD_FERGUSSON = 0x0,
    METHOD_BEZIER = 0x1
};

E_Method method = METHOD_BEZIER;

// velikost aktivniho okoli bodu
const int          POINT_SNAP_RANGE = 10;

// velikost krizku ilustrujiciho bod
const int          CROSS_SIZE = 4;

// barvy pouzivane v programu
const S_RGBA       BLUE  = {0, 0, 255, 255};
const S_RGBA       GREEN = {0, 255, 0, 255};
const S_RGBA       RED   = {255, 0, 0, 255};
const S_RGBA       GREY  = {190, 190, 190, 255};
const S_RGBA		ORANGE = { 255, 165, 0, 255};

// kvalita (pocet bodu) de Casteljau metody
int                QUALITY = 100;


///////////////////////////////////////////////////////////////////////////////
// globalni promenne

S_RGBA             * frame_buffer;          // ukazatel na pamet RGB hodnot vlastniho frame bufferu
int                frame_w, frame_h;        // velikost frame bufferu

const int          default_frame_w  = 499;  // defaultni velikost okna
const int          default_frame_h  = 499;


////////////////////////////////////////////////////////////////////////////////////////////////
// globalni promenne domaciho ukolu

// ridici body krivky
T_PointVector   control_points;

// ridici vektory krivky
T_PointVector	control_vectors;

// body vektoru knot, popisujici vzdalenosti uzlu ve smyslu parametru t
T_PointVector   knot_points;

T_TriangleVector  triangle_points;

int curve_degree = 2;


// struktura popisujici aktivni bod, presnejsi popis viz. defineice struktury
ActivePoint active_point;

// struktura popisujici paramatry B-spline/NURBS krivky
BSplineDescription bdesc( 0 , 2 );

///////////////////////////////////////////////////////////////////////////////
// ovladani mysi

// stav tlacitek mysi
enum E_MouseState
{
  MOUSE_UNDEFINED,
  MOUSE_RIGHT_BUTTON_DOWN,
  MOUSE_LEFT_BUTTON_DOWN
};

// aktualni stav mysi
E_MouseState       mouse_state = MOUSE_UNDEFINED;


////////////////////////////////////////////////////////////////////////////////////////////////
// popup menu

// prikazy popup menu
enum E_MenuCommand
{
    //Identifikace metody pro vykreslovani
    MAIN_MENU_FERGUSSON = 0x2,  
    MAIN_MENU_BEZIER = 0x1,
    

    
    MAIN_MENU_FULL_SCREEN,
    MAIN_MENU_WINDOW,
    MAIN_MENU_QUIT,
  
    POINT_MENU_DELETE,
    POINT_MENU_INC_WEIGHT,
    POINT_MENU_DEC_WEIGHT,
    POINT_MENU_WEIGHT_1,
    POINT_MENU_WEIGHT_2,
    POINT_MENU_WEIGHT_3,
    POINT_MENU_WEIGHT_4,
    POINT_MENU_WEIGHT_5
};



// identifikatory popup menu
int                point_menu, main_menu;

// priznak zda je prave pouzivano popup menu
bool               menu_in_use;


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


// Konstanty promenne biCompression - typ pouzite komprese dat
#define BI_RGB       0               /* zadna komprese */
#define BI_RLE8      1               /* 8-bit RLE */
#define BI_RLE4      2               /* 4-bit RLE */
#define BI_BITFIELDS 3               /* RGB bitove pole s maskou */

#endif // _WIN32


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
}

///////////////////////////////////////////////////////////////////////////////
// funkce zajistujici ziskani barvy pixelu na danych souradnicich
void GetPixel(int x, int y, S_RGBA & color)
{
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


///////////////////////////////////////////////////////////////////////////////
//  ulozeni obsahu obrazovky do souboru ve formatu BMP (save.bmp)

int SaveBitmap(const char * filename, S_RGBA * frame_buffer, int Width, int Height)
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
  for (int y = 0; y < info.biHeight; y++)
  {
    for (int x = 0; x < info.biWidth; x++)
	{
      S_RGBA color;
      GetPixel(x, y, color);
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

///////////////////////////////////////////////////////////////////////////////
// funkce pro vykresleni krizku na danych souradnicich
// size - ovlivnuje velikost krizku (>=1)

void DrawCrossPoint(int x, int y, int size, const S_RGBA & color)
{
  PutPixel(x, y, color);
  for (int i = 0; i < size; ++i)
  {
    PutPixel(x - i, y, color);
    PutPixel(x + i, y, color);
    PutPixel(x, y + i, color);
    PutPixel(x, y - i, color);
  }
}

///////////////////////////////////////////////////////////////////////////////
// funkce vykresli krizky na souradnicich zadanych jako vektor bodu

void DrawCrossPoints(const T_PointVector & points, int size, const S_RGBA & color)
{
  for (int i = 0; i < (int)points.size(); i++)
  {
    DrawCrossPoint(ROUND(points[i].x), ROUND(points[i].y), size, color);
  }
}

///////////////////////////////////////////////////////////////////////////////
// vykresleni usecky danych souradnic a barvy do frame bufferu

void DrawLine(int x1, int y1, int x2, int y2, const S_RGBA & color)
{
  // delta v ose x a y
  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);

  if ( dx == 0 && dy == 0 ) return;

  // zamena osy x a y v algoritmu vykreslovani
  bool xy_swap = (dy > dx);
  if (xy_swap)
  {
    SWAP(x1, y1);
    SWAP(x2, y2);
    SWAP(dx, dy);
  }

  // pripadne prohozeni bodu v ose x
  if (x1 > x2)
  {
    SWAP(x1, x2);
    SWAP(y1, y2);
  }

  // smer postupu v ose y
  int step_y;
  if (y1 > y2) step_y = -1;
  else step_y = 1;

  // konstanty pro Bresenhamuv algoritmus
  int k1 = 2 * dy;
  int k2 = 2 * (dy - dx);
  int p  = 2 * dy - dx;

  // pocatecni prirazeni hodnot
  int x = x1;
  int y = y1;

  // cyklus vykreslovani usecky
  while (x <= x2)
  {
    if (!xy_swap)
      PutPixel(x, y, color);
    else
      PutPixel(y, x, color);

    x++;
    if (p < 0)
	{
      p += k1;
    }
    else
	{
      p += k2;
      y += step_y;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// vykresleni usecky mezi vsemi body daneho vektoru
// points - vektor bodu
// color - barva pouzita pro vykresleni

void DrawLines(const T_PointVector & points, const S_RGBA & color)
{
  for (int i = 1; i < (int)points.size(); i++)
  {
    DrawLine(ROUND(points[i-1].x), ROUND(points[i-1].y),
	         ROUND(points[i].x), ROUND(points[i].y),
			 color);
  }
}

void DrawControlVectors(const T_PointVector & points, const T_PointVector & vectors, const S_RGBA & color)
{
  if(points.size() == vectors.size())
	for (int i = 0; i < (int)points.size(); i++)
	{
		DrawLine(ROUND(vectors[i].x), ROUND(vectors[i].y),
				 ROUND(points[i].x), ROUND(points[i].y),
				 color);
	}
}

void DrawTriangles( const T_TriangleVector& triangle_vector , const S_RGBA & odd_color , const S_RGBA & even_color )
{
   T_TriangleVector& t_vector = const_cast<T_TriangleVector& >(triangle_vector);
   T_TriangleVector::iterator triangle = t_vector.begin();
   int cntr = 0;
   const S_RGBA* act_clr = NULL;
   
   for ( triangle = t_vector.begin(); triangle != t_vector.end(); triangle++, cntr++ )
   {
      if ( cntr%2 )
      {
         act_clr = &odd_color;
      }
      else
      {
         act_clr = &even_color  ;
      }
      DrawLine(ROUND(triangle->A.x), ROUND(triangle->A.y), ROUND(triangle->B.x), ROUND(triangle->B.y), *act_clr);
      DrawLine(ROUND(triangle->B.x), ROUND(triangle->B.y), ROUND(triangle->C.x), ROUND(triangle->C.y), *act_clr);
      DrawLine(ROUND(triangle->C.x), ROUND(triangle->C.y), ROUND(triangle->A.x), ROUND(triangle->A.y), *act_clr);
   } 
}

///////////////////////////////////////////////////////////////////////////////
// projde zadany vektor bodu a zjisti, ktery je pobliz dane souradnice
// x, y - pozice bodu
// range - velikost testovaneho okoli
// points - vektor bodu
// funkce vraci index bodu, nebo -1 pokud pobliz souradnice zadny neni

int PointSnap(int x, int y, int range, const T_PointVector & points)
{
  int index = -1;
  float min_range = FLT_MAX;

  // nalezeni nejblizsiho bodu
  for (int i = 0; i < (int)points.size(); i++)
  {
    float dx = x - points[i].x;
    float dy = y - points[i].y;
    float temp = dx * dx + dy * dy;

    if (temp < min_range) {
	    min_range = temp;
	    index = i;
	  }
  }

  // lezi bod v pozadovanem okoli?
  if (min_range < range * range)
    return index;
  else
    return -1;
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
}

///////////////////////////////////////////////////////////////////////////////
// funkce reagujici na zmenu rozmeru okna
// w, h - nova velikost okna

void onResize(int w, int h)
{
  S_RGBA *new_frame_buffer;          // ukazatel na pamet noveho frame bufferu

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
  memset(new_frame_buffer, 0, w * h * sizeof(S_RGBA));

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
}

///////////////////////////////////////////////////////////////////////////////
// funkce zajistujici prekresleni obsahu okna programu

void onDisplay(void)
{
  // test existence frame bufferu
  assert(frame_buffer != NULL);

  // vymazani bitovych rovin barevneho bufferu
  glClear(GL_COLOR_BUFFER_BIT);

  // vymazani obsahu frame bufferu
  memset(frame_buffer, 0, frame_w * frame_h * sizeof(S_RGBA));
  // propojeni ridicich bodu
  DrawLines(control_points, GREY);
  // vykresleni ridicich bodu krivky
  DrawCrossPoints(control_points, CROSS_SIZE, BLUE);
  // vypocet bodu krivky
  T_PointVector line_points;

  switch(method)
  {
    case METHOD_FERGUSSON:
    {
  	  if (  control_points.size() >= 2 )
      {        
          Fergusson(QUALITY, control_points, control_vectors, line_points );
      }
      //Vykresleni ridicich vektoru
  	  DrawControlVectors(control_points, control_vectors,ORANGE);
    }
    break;
    
    case METHOD_BEZIER:
    {
    	if (  control_points.size() > 2 )
    		Bezier(QUALITY, control_points, line_points );
    }
    break;
    
    default:
    break;
  }
  
  // vykresleni krivky slozene z car
  DrawLines(line_points, RED);
  // vykresleni vypoctenych bodu
  DrawCrossPoints(line_points, CROSS_SIZE, GREEN);
  
  // vykresleni aktivniho bodu
  if (active_point.active_point >= 0)
  {
    //Vyber aktivniho bodu ridiciho polygonu nebo knot vektoru
    switch ( active_point.comp_type )
    {
      case ActivePoint::CONTROL_POLYGON :
      {
        DrawCrossPoint(ROUND(control_points[active_point.active_point].x),
               ROUND(control_points[active_point.active_point].y),
               CROSS_SIZE, RED);
      } 
      break;
      case ActivePoint::KNOT_VECTOR :
      {
        DrawCrossPoint(ROUND(knot_points[active_point.active_point].x),
               ROUND(knot_points[active_point.active_point].y),
               CROSS_SIZE, BLUE);
      }
      break;
      default : break;
    }
      
  }

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

  switch (command)
  {    

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

    default: break;
  }

  // vyvolani prekresleni OpenGL okna
  glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////////////////
// funkce volana pri vyberu prikazu z popup menu pro bod

void onPointMenu(int command)
{
  // editaci bodu lze provadet puze na ridicim polygonu
  if (active_point.comp_type !=  ActivePoint::CONTROL_POLYGON ) 
  {
    return;
  }
  
  // reakce na prikaz
  switch (command)
  {
    // smaze aktivni bod    
    case POINT_MENU_DELETE:
        control_points.erase(control_points.begin() + active_point.active_point);
        control_vectors.erase(control_vectors.begin() + active_point.active_point);
        active_point.comp_type =  ActivePoint::NO_COMPONENT;
    break;
    
     case POINT_MENU_INC_WEIGHT:
        (control_points.begin() + active_point.active_point)->weight*=1.1;
    break;

    case POINT_MENU_DEC_WEIGHT:
        (control_points.begin() + active_point.active_point)->weight/=1.1;

    default:
	  break;
  }

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

    // prepnuti do celoobrazovkoveho modu
    /*case 'f':
      onMainMenu(MAIN_MENU_FULL_SCREEN);
    break;*/

    // vraceni z celoobrazovkoveho modu
    case 'w':
      onMainMenu(MAIN_MENU_WINDOW);
    break;

    case 'b':
        	method = METHOD_BEZIER;
        break;

    case 'f':
    	method = METHOD_FERGUSSON;
    break;

    // ulozi obrazek
    case 's':
      fprintf(stdout,"Ukladam obrazek %dx%d %s\n", frame_w, frame_h,"save.bmp");
      SaveBitmap("save.bmp", frame_buffer, frame_w, frame_h);
    break;

    // smaze aktivni bod
    case 'd':
      onPointMenu(POINT_MENU_DELETE);
    break;
     // INC_WEIGHT
    case '+':
      onPointMenu(POINT_MENU_INC_WEIGHT);
    break;
    // DEC_WEIGHT
    case '-':
      onPointMenu(POINT_MENU_DEC_WEIGHT);
    break;
    
    default: break;
  }

  // vyvolani prekresleni OpenGL okna
  glutPostRedisplay();
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

  // nedefinovany stav mysi
  mouse_state = MOUSE_UNDEFINED;

  // vetveni podle stisknuteho tlacitka mysi
  switch (button)
  {
    // leve tlacitko mysi
    case GLUT_LEFT_BUTTON:
      // vetveni podle stisknuti nebo pusteni tlacitka
      if (state == GLUT_DOWN)   // stisknuto
      {
        // pridani ridiciho bodu
        if (active_point.comp_type ==  ActivePoint::NO_COMPONENT) {
          control_points.push_back(S_Point(x, y));
          control_vectors.push_back(S_Point(x, y+50.0));
          active_point = (int)control_points.size() - 1;
        }
        
        // stav mysi
        mouse_state = MOUSE_LEFT_BUTTON_DOWN;
      }
    break;

    // prostredni tlacitko mysi
    case GLUT_MIDDLE_BUTTON:
      // vetveni podle stisknuti nebo pusteni tlacitka
      if (state == GLUT_DOWN)   // stisknuto
      {
      }
    break;

    // prave tlacitko mysi
    case GLUT_RIGHT_BUTTON:
      // vetveni podle stisknuti nebo pusteni tlacitka
      if (state == GLUT_DOWN)   // stisknuto
      {
        // stav mysi
        mouse_state = MOUSE_RIGHT_BUTTON_DOWN;
      }
    break;
  };

  // vyvolani prekresleni OpenGL okna
  glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////////////////
// funkce reagujici na pohyb mysi se stisknutym tlacitkem
// x, y - aktualni souradnice kurzoru

void onMouseMotion(int x, int y)
{
  // obraceni osy y
  y = frame_h - y - 1;

  // posunuti bodu
  if ( mouse_state == MOUSE_LEFT_BUTTON_DOWN ) {
    switch ( active_point.comp_type ) {
      case ActivePoint::CONTROL_POLYGON : {
        control_points[active_point.active_point].x = x;
        control_points[active_point.active_point].y = y;
       /*
        control_vectors[active_point.active_point].x = x;
        control_vectors[active_point.active_point].y = y;
        */
      }
      break;
      case ActivePoint::KNOT_VECTOR : {
        bdesc.OnKnotChange( active_point.active_point , (double) x , (double) y );
      }
      break;
      case ActivePoint::CONTROL_VECTOR : {
              control_vectors[active_point.active_point].x = x;
              control_vectors[active_point.active_point].y = y;
            }
      break;
          
      default : break;
    }
          
  }

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

  // nalezeni nejblizsiho bodu
  int index;
  if ( (index = PointSnap(x, y, POINT_SNAP_RANGE, control_points)) > -1 ) {
    active_point.comp_type = ActivePoint::CONTROL_POLYGON;
    active_point.active_point = index;
  }
  else if ( (index = PointSnap(x, y, POINT_SNAP_RANGE, control_vectors)) > -1 ) {
      active_point.comp_type = ActivePoint::CONTROL_VECTOR;
      active_point.active_point = index;
    }
  else if ( (index = PointSnap(x, y, POINT_SNAP_RANGE, knot_points)) > -1 ) {
    active_point.comp_type = ActivePoint::KNOT_VECTOR;
    active_point.active_point = index;
  }
  else {
    active_point.comp_type = ActivePoint::NO_COMPONENT;
    active_point.active_point = -1;
  }
  
  // vyber aktivniho menu
  if ( active_point.comp_type == ActivePoint::CONTROL_POLYGON )
  {
    if (!menu_in_use)	{
      glutSetCursor(GLUT_CURSOR_CROSSHAIR);
	    glutDetachMenu(GLUT_RIGHT_BUTTON);
      glutSetMenu(point_menu);
      glutAttachMenu(GLUT_RIGHT_BUTTON);
	  }
  }
  else if ( active_point.comp_type == ActivePoint::KNOT_VECTOR ) {
      
    if (!menu_in_use)	{
      glutSetCursor(GLUT_CURSOR_CROSSHAIR);
      glutDetachMenu(GLUT_RIGHT_BUTTON);
      glutSetMenu(main_menu);
      glutAttachMenu(GLUT_RIGHT_BUTTON);
	  }
  }
  else
  {
    if (!menu_in_use)	{
      glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
      glutDetachMenu(GLUT_RIGHT_BUTTON);
      glutSetMenu(main_menu);
      glutAttachMenu(GLUT_RIGHT_BUTTON);
	  }

  }
 
   // vyvolani prekresleni OpenGL okna
  glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////////////////
// funkce volana pri zmene stavu popup menu

void onMenuStatus(int status, int x, int y)
{
  // je menu zobrazeno
  menu_in_use = (status == GLUT_MENU_IN_USE);
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
  glutCreateWindow(PROGRAM_TITLE[METHOD_BEZIER]);

  // registrace fukce zajistujici prekresleni obsahu okna programu
  glutDisplayFunc(onDisplay);
  // registrace funkce reagujici na zmenu rozmeru okna programu
  glutReshapeFunc(onResize);
  // registrace funkce reagujici na stisknuti klavesnice
  glutKeyboardFunc(onKeyboard);
  // registrace funkce reagujici na stisknuti tlacitka mysi
  glutMouseFunc(onMouseClick);
  // registrace funkce reagujici na pohyb mysi se stisknutym tlacitkem
  glutMotionFunc(onMouseMotion);
  // registrace funkce reagujici na pohyb mysi bez stisknutych tlacitek
  glutPassiveMotionFunc(onPassiveMouseMotion);

  // inicilizace hlavniho popup menu
  main_menu = glutCreateMenu(onMainMenu);
  glutAddMenuEntry("Full Screen", MAIN_MENU_FULL_SCREEN);
  glutAddMenuEntry("Default Window \tw", MAIN_MENU_WINDOW);
  glutAddMenuEntry("Quit \tx", MAIN_MENU_QUIT);
  // inicializace popup menu pro bod
  point_menu = glutCreateMenu(onPointMenu);
  glutAddMenuEntry("Delete Point \td", POINT_MENU_DELETE);


  // vyber aktivniho menu
  glutSetMenu(main_menu);
  // prirazeni menu pravemu tlacitku
  glutAttachMenu(GLUT_RIGHT_BUTTON);
  // registrace funkce volane pri zmene stavu popup menu
  glutMenuStatusFunc(onMenuStatus);

  // nekonecna smycka volajici registrovane funkce
  glutMainLoop();

  return 0;
}


