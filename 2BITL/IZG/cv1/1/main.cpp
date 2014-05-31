////////////////////////////////////////////////////////////////////////////////////////////////
//
// Kod do cviceni #1 z predmetu Zaklady pocitacove grafiky
//
//
// Obsah cviceni:
//
//   Implementace omezeni barevneho prostoru
//
// 1) Opakovani z prednasek: barevne prostory
//
// 2) Implementace prevodu barevneho prostoru do stupnu sedi
//
// 3) Implementace prevodu do cernobile prahovanim
//
// 4) Zadani ukolu
// 
// 5) Ovladani programu:
//      a) stisknuti klavesy "l" - nacteni vzoroveho obrazku, muze byt zadan jako parametr
//      b) stisknuti klavesy "g" - prevod do stupnu sedi
//      c) stisknuti klaves "1,2,3,4" - dithering obrazku prahovanim
//      d) stisknuti klavesy "d" distribuce chyby
//      e) kliknuti mysi do okna programu vypsani barvy pixelu
//      f) kliknuti mysi do okna programu pravym tlacitkem - zapsani modreho obdelnika
//
////////////////////////////////////////////////////////////////////////////////////////////////

// include 

#ifdef _WIN32
  #include <windows.h>
#endif

#include "main.h"

#include <GL/gl.h>
#include <GL/glut.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

#define PROGRAM_TITLE      "IZG labs #01, Load/Save BMP - greyscale, error diffusion"

////////////////////////////////////////////////////////////
// globalni promenne

S_RGBA           * frame_buffer;          // ukazatel na pamet RGB hodnot vlastniho frame bufferu
int                frame_w, frame_h;        // velikost frame bufferu

const int          default_frame_w  = 1056;  // defaultni velikost okna
const int          default_frame_h  = 704;
char *             PictureName;             // jmeno souboru s obrazkem


////////////////////////////////////////////////////////////
// Struktury pro praci s graficky formatem souboru BMP

// zarovnani scanline na 4byte
#define DIB_SCAN_LINE(WIDTH) (((WIDTH) & 0xfffffffc) + (((WIDTH) & 0x3) ? 4 : 0))

// GRAFICKA STRUKTURA - hlavicka bmp souboru

#ifndef _WIN32

  #pragma pack(2)        // zarovnani pameti na 2byty

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

  #define BI_RGB       0             /* zadna komprese */
  #define BI_RLE8      1             /* 8-bit RLE */
  #define BI_RLE4      2             /* 4-bit RLE */
  #define BI_BITFIELDS 3             /* RGB bitove pole s maskou */
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
// funkce zajistujici zobrazeni obdelniku dane barvy na danych souradnicich
// x1, y1 - souradnice prvniho rohu obdelnika pro vykresleni
// x2, y2 - souradnice diagonalniho rohu obdelnika pro vykresleni
// color - barva vykresleneho obdelnika

void DrawRectangle(int x1, int y1, int x2, int y2, const S_RGBA & color)
{
  // test existence frame bufferu
  assert(frame_buffer != NULL);

  if (x2 < x1) SWAP(x1, x2);
  if (y2 < y1) SWAP(y1, y2);

  for (int i = x1; i <= x2; i++)
  {
	PutPixel(i, y1, color);
	PutPixel(i, y2, color);
  }
  for (int i = y1; i <= y2; i++)
  {
	PutPixel(x1, i, color);
	PutPixel(x2, i, color);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// funkce pro nacteni a ulozeni obrazku
//
////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//  Ulozeni obsahu obrazovky do souboru ve formatu BMP (save.bmp)

int SaveMyBitmap(const char *filename, S_RGBA* frame_buffer,int Width, int Height)
{ // vraci 0 - chyba, 1 - OK
  
  // otevreni souboru pro zapis
  FILE *fw = fopen (filename, "wb");

  // osetreni chyby pri otevirani souboru
  if (fw == NULL)
  {
    std::cout << "Cannot open file !" << filename << "'\n";
    return 0;
  }

  // naplneni hlavickek daty
  
  BITMAPFILEHEADER head;
  head.bfType         = 0x4d42;
  head.bfSize         = sizeof (BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER) +
                        DIB_SCAN_LINE (Width * 3) * Height;
  head.bfReserved1    = 0;
  head.bfReserved2    = 0;
  head.bfOffBits      = sizeof (BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER);

  BITMAPINFOHEADER info;
  info.biSize             = sizeof (BITMAPINFOHEADER);
  info.biWidth            = Width;
  info.biHeight           = Height;
  info.biPlanes           = 1;
  info.biBitCount         = 24;
  info.biCompression      = 0;
  info.biSizeImage        = DIB_SCAN_LINE (Width * 3) * Height;
  info.biXPelsPerMeter    = 0;
  info.biYPelsPerMeter    = 0;
  info.biClrUsed          = 0;
  info.biClrImportant     = 0;

  // zapis hlavicek do souboru
  fwrite (&head, sizeof (BITMAPFILEHEADER), 1, fw);
  fwrite (&info, sizeof (BITMAPINFOHEADER), 1, fw);

  int zar = DIB_SCAN_LINE (info.biWidth * 3) - info.biWidth * 3;
  std::cout << "Zarovnani = " << zar << std::endl;

  // cyklus pres obrazove body
  for (int y = 0; y < info.biHeight; y++)
  {
    for (int x = 0; x < info.biWidth; x++)
    { 
      // zjisteni barvy bodu a zapis do souboru
      S_RGBA color;
      GetPixel(x,y,color);
      fputc (color.blue, fw);
      fputc (color.green, fw);
      fputc (color.red, fw);
    }
    for (int x = 0; x < zar; x++)
      fputc (0, fw);
  } 

  fclose (fw);

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//  Nacteni obrazku ze souboru BMP a zobrazeni na obrazovku

int LoadMyBitmap(const char *filename, S_RGBA* frame_buffer)
{ // vraci 0 - chyba, 1 - OK

  // otevreni souboru pro cteni
  FILE *fr = fopen (filename, "rb");

  // osetreni chyby pri otevirani souboru
  if (fr == NULL)
  {
    std::cout << "Cannot open file !" << filename << "'\n";
    return 0 ;
  }

  BITMAPFILEHEADER head;
  BITMAPINFOHEADER info;

  // precteni hlavicek ze souboru
  fread (&head, sizeof (BITMAPFILEHEADER), 1, fr);
  fread (&info, sizeof (BITMAPINFOHEADER), 1, fr);
  
  // skok v souboru na zacatek dat
  fseek (fr, head.bfOffBits, SEEK_SET);
  int zar = DIB_SCAN_LINE (info.biWidth * 3) - info.biWidth * 3;
  std::cout << "Zarovnani = " << zar << std::endl;

  // cyklus pres obrazove body
  for (int y = 0; y < info.biHeight; y++)
  {
    for (int x = 0; x < info.biWidth; x++)
    { 
      // precteni hodnot ze souboru a zapsani do framebufferu
      S_RGBA color;
      color.blue = fgetc (fr) ;
      color.green = fgetc (fr) ;
      color.red = fgetc (fr) ;
      PutPixel(x, y, color);
    }
    for (int x = 0; x < zar; x++)
      fgetc (fr) ;
  } 

  fclose (fr) ;

  return 1 ;
}                               


////////////////////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////////////////////
// funkce reagujici na stisknuti klavesnice
// key - kod stisknute klavesy
// x, y - souradnice kurzoru pri stisknuti klavesy

void onKeyboard(unsigned char key, int x, int y)
{
  static int      old_frame_h, old_frame_w, old_window_x, old_window_y;         // paramewtry puvodniho umisteni okna

  // uprava velkych pismen na mala
  if (key>='A' && key<='Z')
    key+='a'-'A';
  
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
    case 'f':
      old_frame_h = frame_h;
      old_frame_w = frame_w;
      old_window_x = glutGet(GLUT_WINDOW_X);
      old_window_y = glutGet(GLUT_WINDOW_Y);
      glutFullScreen();
      break;
    // vraceni z celoobrazovkoveho modu
    case 'w':
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


    // nacte obrazek
    case 'l':
        fprintf(stdout,"Nacitam obrazek %s\n",PictureName);
        LoadMyBitmap(PictureName, frame_buffer);
        break;
    // ulozi obrazek
    case 's':
        fprintf(stdout,"Ukladam obrazek %dx%d %s\n", frame_w, frame_h,"save.bmp");    
        SaveMyBitmap("save.bmp", frame_buffer, frame_w, frame_h);
        break;

    case 'g':
        fprintf(stdout,"Prevadim obrazek na 256 urovni sede\n");
        GreyScale();
        break;
    case '1':
        fprintf(stdout,"Prevadim obrazek na cernou a bilou s prahem 50\n");
        Thresholding(50);
        break;
    case '2':
        fprintf(stdout,"Prevadim obrazek na cernou a bilou s prahem 100\n");
        Thresholding(100);
        break;
    case '3': 
        fprintf(stdout,"Prevadim obrazek na cernou a bilou s prahem 150\n");
        Thresholding(150);
        break;
    case '4':
        fprintf(stdout,"Prevadim obrazek na cernou a bilou s prahem 200\n");
        Thresholding(200);
        break;
    case 'd':
        fprintf(stdout,"Prevadim obrazek na cernou a bilou pomoci distribuce chyby\n");
        ErrorDiffusion();
        break;

    default:
      break;
  }

  // vyvolani prekresleni OpenGL okna
  glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// funkce reagujici na stisknuti tlacitka mysi
// button - kod stisknuteho tlacitka
// state - stav tlacitka - stisknute/uvolnene
// x, y - souradnice kurzoru pri stisknuti klavesy

void onMouseClick(int button, int state, int x, int y)
{
  static int          rect_start_x, rect_start_y;        // souradnice zacatku obdelniku
  S_RGBA              color;
  
  // obraceni osy y
  y = frame_h - y - 1;

  // vetveni podle stisknuteho tlacitka mysi
  switch (button)
  {
    // leve nebo prostredni tlacitko mysi
        case GLUT_LEFT_BUTTON:
        case GLUT_MIDDLE_BUTTON:
      // vetveni podle stisknuti nebo pusteni tlacitka
      if (state == GLUT_DOWN)   // stisknuto
      {
        // ziskani barvy pixelu na pozici kurzoru mysi
        GetPixel(x, y, color);
        
        // vypsani barvy pixelu do konzoly
        std::cout << "Barva pixelu na pozici kliknuti mysi " << x << ", " << y << " : ";
        std::cout << (int) color.red << ",";
        std::cout << (int) color.green << ",";
        std::cout << (int) color.blue << ",";
        std::cout << (int) color.alpha << std::endl;
      }
      else                      // pusteno
      {
      }
      break;
	  case GLUT_RIGHT_BUTTON:
		  // vetveni podle stisknuti nebo pusteni tlacitka
		  if (state == GLUT_DOWN)   // tlacitko stisknuto
		  {
			// vykresleni pixelu na pozici kurzoru mysi, zacatek obdelniku
			PutPixel(x, y, COLOR_BLUE);
			// ulozeni souradnic pocatku obdelniku
			rect_start_x = x;
			rect_start_y = y;
		  }
		  else                      // tlacitko uvolneno
			// vykresleni obdelniku
			DrawRectangle(rect_start_x, rect_start_y, x, y, COLOR_BLUE);

	   break;
  };

  // vyvolani prekresleni OpenGL okna
  glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// funkce reagujici na pohyb mysi se stisknutym tlacitkem
// x, y - aktualni souradnice kurzoru

void onMouseMotion(int x, int y)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// hlavni funkce programu
// argc - pocet vstupnich parametru
// argv - pole vstupnich parametru

int main(int argc, char **argv)
{
  // default obrazek
  char PicDefault[10]="test.bmp";
  // prvni argument bere jako nazev obrazku
  if (argc>1)
    PictureName = argv[1];
  else
    PictureName = PicDefault;
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
  // registrace funkce reagujici na pohyb mysi
  glutMotionFunc(onMouseMotion);
  
  // nekonecna smycka volajici registrovane funkce
  glutMainLoop();

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

