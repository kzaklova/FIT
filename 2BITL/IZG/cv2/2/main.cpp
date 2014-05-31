////////////////////////////////////////////////////////////////////////////////
//
// Kod do cviceni #2 z predmetu Zaklady pocitacove grafiky
//
//
// Obsah cviceni:
//
//   Generovani zakladnich objektu v rastru
//
// 1) Opakování rasterizace úseèky z pøedná¹ek.
//
// 2) Implementace Bresenhamova algoritmu rasterizace úseèky.
//
// 3) Opakování rasterizace kru¾nice Midpoint algoritmem.
//
// 4) Zadani ukolu:
//    Implementace rasterizace kru¾nice Midpoint algoritmem. 
// 
// 5) Ovladani programu:
//      a) tazeni mysi (stisknute L tlacitko) - vykresleni usecky zadanim pocatecniho a koncoveho bodu
//      b) tazeni mysi (stisknute P tlacitko) - vykresleni kruznice zadanim stredu a natazenim polomeru
//      c) klavesa 'c' - premazani obsahu okna bilou barvou
//      d) kliknuti mysi do okna programu - vypsani barvy pixelu
//
////////////////////////////////////////////////////////////////////////////////

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
#include <math.h>
#include <string.h>

#define PROGRAM_TITLE      "IZG labs #02, Rasterize line/circle"

////////////////////////////////////////////////////////////
// globalni promenne

S_RGBA           * frame_buffer;            // ukazatel na pamet RGB hodnot vlastniho frame bufferu
int                frame_w, frame_h;        // velikost frame bufferu

const int          default_frame_w  = 500;  // defaultni velikost okna
const int          default_frame_h  = 388;


////////////////////////////////////////////////////////////////////////////////
// funkce zajistujici zobrazeni pixelu dane barvy na danych souradnicich

void PutPixel(int x, int y, S_RGBA color)
{
	// test existence frame bufferu
	assert(frame_buffer != NULL);

	// test rozmeru okna
	if ((x >= 0) && (x < frame_w) && (y >= 0) && (y < frame_h))
		// zapsani dane barvy do pameti frame bufferu
		*(frame_buffer + y*frame_w + x) = color;
}

////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
// prepise cely framebuffer bilou barvou
void CleanBuf()
{
	for (int x = 0; x < frame_w; x++) {
		for (int y = 0; y < frame_h; y++){
			PutPixel(x, y, COLOR_WHITE);
		}
	}

	return;
}


////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
// funkce reagujici na stisknuti klavesnice
// key - kod stisknute klavesy
// x, y - souradnice kurzoru pri stisknuti klavesy

void onKeyboard(unsigned char key, int x, int y)
{
	static int old_frame_h, old_frame_w, old_window_x, old_window_y; // parametry puvodniho umisteni okna
	
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
	
	
		// smazani framebuffer = prepsani bilou barvou
		case 'c':
			CleanBuf();
			break;
	
		default:
			break;
	}
	
	// vyvolani prekresleni OpenGL okna
	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////
// funkce reagujici na stisknuti tlacitka mysi
// button - kod stisknuteho tlacitka
// state - stav tlacitka - stisknute/uvolnene
// x, y - souradnice kurzoru pri stisknuti klavesy

void onMouseClick(int button, int state, int x, int y)
{
	static int oldX, oldY; // souradnice mysi pri stisknuti tlacitka
	
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
				// ulozeni pocatecniho bodu usecky
				oldX = x;
				oldY = y;
			}
			else                      // pusteno
			{
				// vykresli usecku
				DrawLine(oldX, oldY, x, y);
			}
			break;

		// prave tlacitko mysi
		case GLUT_RIGHT_BUTTON:
		
			// vetveni podle stisknuti nebo pusteni tlacitka
			if (state == GLUT_DOWN)   // tlacitko stisknuto
			{
				// ulozeni stredu kruznice
				oldX = x;
				oldY = y;
			}
			else                      // pusteno
			{
				// vykresli kruznici
				int radius = int(sqrt(1.0 * (x - oldX) * (x - oldX) + (y - oldY) * (y - oldY)));
				DrawCircle(oldX, oldY, radius);
			}
			break;
	};
	
	// vyvolani prekresleni OpenGL okna
	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////
// funkce reagujici na pohyb mysi se stisknutym tlacitkem
// x, y - aktualni souradnice kurzoru

void onMouseMotion(int x, int y)
{
}

////////////////////////////////////////////////////////////////////////////////
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
	// registrace funkce reagujici na pohyb mysi
	glutMotionFunc(onMouseMotion);
	
	// nekonecna smycka volajici registrovane funkce
	glutMainLoop();
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// konec souboru main.cpp
////////////////////////////////////////////////////////////////////////////////
