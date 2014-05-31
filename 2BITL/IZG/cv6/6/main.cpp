///////////////////////////////////////////////////////////////////////////////
// Zaklady pocitacove grafiky
// Cviceni #7
//
// Autor: Adam Herout, modified by Michal Spanel
//
// Obsah cviceni: "Informativni nejzakladnejsi zaklady OpenGL"
//
// Demonstrovane jevy:
//      - OpenGL + GLUT
//      - vykreslovani
//      - display-listy
//      - barvy
//      - osvetleni
//      - pruhlednost
//      - stencil buffer
//
// *) Ovladani programu:
//      mys + leve tl.  - rotace cele sceny
//      mys + prave tl. - priblizeni sceny
//      1,2,3           - polygony vyplnit, carami, body
//
// Opravy:
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// include

#ifdef _WIN32
#   include <windows.h>
#endif

#include "main.h"

#include <GL/gl.h>
#include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>


///////////////////////////////////////////////////////////////////////////////
// globalni konstanty

// titulek hlavniho okna
const char      * PROGRAM_TITLE = "IZG Labs #07, Informativni nejzakladnejsi zaklady OpenGL";

const float     fov = 45.0;             // zorny uhel - field of view
const float     near_plane = 10.0;      // blizsi orezavaci rovina
const float     far_plane = 250.0;      // vzdalenejsi orezavaci rovina

const int       default_w = 600;        // defaultni velikost okna
const int       default_h = 600;

///////////////////////////////////////////////////////////////////////////////
// globalni promenne sdilene mezi main.cpp a student.cpp

int             scene_rot_x = 0;        // rotace sceny dana mysovanim
int             scene_rot_y = 0;
int             scene_move_z = -50;     // posuv sceny
int             time = 0;               // virtualni cas [s]

int             wheel;                  // identifikator display-listu kolecka
int             red_mat, blue_mat;      // display-listy pro materialy
unsigned        texid;                  // identifikator textury zrcadla


///////////////////////////////////////////////////////////////////////////////
// zakladni nastaveni OpenGL
// - funkce se vola jednou, pri zacatku programu

void Init(void)
{
  // nastaveni clear color
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // parametry svetla
  GLfloat light_ambient[]  = {0.5f, 0.5f, 0.5f, 1.0f};  // ambientni slozka svetla
  GLfloat light_specular[] = {0.8f, 0.8f, 0.8f, 1.0f};  // barva odlesku
  GLfloat light_diffuse[]  = {0.9f, 0.9f, 0.9f, 1.0f};  // difuzni slozka svetla
  GLfloat light_position[] = {15.0f, 0.0f, 0.0f, 1.0f}; // definice pozice svetla

  // nastaveni svetla
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  // nastaveni osvetlovaciho modelu
//  glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL_EXT, GL_SEPARATE_SPECULAR_COLOR_EXT);

  // povoleni z-bufferu 
  glClearDepth(1.0);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // velikost bodu, pokud se nejaky kresli...
  glPointSize(5.0f);
}


///////////////////////////////////////////////////////////////////////////////
// vygenerovani display-listu pro dva materialy
// - funkce se vola jednou, pri zacatku programu

void InitMaterialDisplayLists()
{
  // vygenerujeme display listy pro dva dulezite materialy
  red_mat = glGenLists(1);                      // cerveny material - display-list
  glNewList(red_mat, GL_COMPILE);               // budeme plnit prislusny display-list
  {
    GLfloat mat_diffuse[]  = {0.9f, 0.0f, 0.0f, 1.0f};  // difuzni (matna) slozka materialu
    GLfloat mat_ambient[]  = {0.2f, 0.2f, 0.2f, 1.0f};  // ambient (rozptylena) slozka materialu
    GLfloat mat_specular[] = {0.5f, 0.5f, 0.5f, 1.0f};  // spekularni (odleskova) slozka materialu

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 80); // lesklost materialu - 1-128
  }
  glEndList();                                  // konec cerveneho materialu

  // vygenerujeme display-list pro modry material
  blue_mat = glGenLists(1);
  glNewList(blue_mat, GL_COMPILE);
  {
    GLfloat mat_diffuse[]  = {0.0f, 0.0f, 0.9f, 1.0f};  // difuzni slozka materialu
    GLfloat mat_ambient[]  = {0.2f, 0.2f, 0.2f, 1.0f};  // ambient (rozptylena) slozka materialu
    GLfloat mat_specular[] = {0.5f, 0.5f, 0.5f, 1.0f};  // spekularni slozka materialu

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular); 
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 120);
  }
  glEndList();                                  // konec popisu modreho materialu
}


///////////////////////////////////////////////////////////////////////////////
// nastaveni souradneho systemu podle velikosti okna
// - funkci zavola GLUT vzdy, kdyz se zmeni rozmery okna

void onReshape(int w, int h)
{
  glViewport(0, 0, w, h);                       // viditelna oblast
  glMatrixMode(GL_PROJECTION);                  // nastavujeme projekcni matici
  glLoadIdentity();                             // nahrat jednotkovou matici
  gluPerspective(fov, (double)w / (double)h, near_plane, far_plane); // perspektivni projekce
  glMatrixMode(GL_MODELVIEW);                   // modelova matice
  glLoadIdentity();
}


///////////////////////////////////////////////////////////////////////////////
// tato funkce je volana pro prekresleni okna
// - vola ji GLUT, kdyz okno vystoupi do popredi, nebo kdyz si to
//   objedname funkci glutPostRedisplay()

void onDisplay(void)
{
  // vymazani bufferu
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);                   // bude se menit modelova matice
  glLoadIdentity();                             // nahrat jednotkovou matici

  // nechame vykreslit scenu
  DrawScene();

  // takrka povinne ukonceni
  glFlush();                                    // provedeni vsech prikazu
  glutSwapBuffers();                            // a prohozeni bufferu
}


///////////////////////////////////////////////////////////////////////////////
// funkce volana pri stisku klavesy
// - vola ji GLUT, kdyz uzivatel stiskne klavesu

void onKeyboard(unsigned char key, int x, int y)
{
  // uprava velkych pismen na mala
  if (key >= 'A' && key <= 'Z')
    key += 'a' - 'A'; 

  // rozeskok podle stlacene klavesy
  switch (key)
  {
    // prepnuti na zobrazeni pouze vrcholu
    case '1':
      glPolygonMode(GL_FRONT, GL_FILL);
      glPolygonMode(GL_BACK, GL_FILL);
      break;

    // prepnuti na zobrazeni dratoveho modelu
    case '2':
      glPolygonMode(GL_FRONT, GL_LINE);
      glPolygonMode(GL_BACK, GL_LINE);
      break;

    // prepnuti na zobrazeni vyplnenych sten
    case '3':
      glPolygonMode(GL_FRONT, GL_POINT);
      glPolygonMode(GL_BACK, GL_POINT);
      break;
    
    // ukonceni programu
    case 27:                                    // klavesa Escape
    case 'q':
    case 'x':
      exit(0);
      break;

    // prepnuti na celou obrazovku
    case 'f':
      glutFullScreen();
      break;

    // prepnuti zpet do okna
    case 'w':
      glutReshapeWindow(default_w, default_h);
      break;

    default:
      break;
  }

  // vyvolani prekresleni OpenGL okna
  glutPostRedisplay();
}


///////////////////////////////////////////////////////////////////////////////
// funkce volana pri stisku tlacitek mysi                          
// - vola GLUT, kdyz uzivatel stiskne tlacitko mysi
             
int old_mouse_x = 0, old_mouse_y = 0;           // minula pozice mysi, ze ktere se pocitaji rotace
int stav = 0;                                   // stav tlacitek mysi

void onMouseClick(int button, int state, int x, int y)
{
  // zapamatovat pozici kurzoru mysi
  old_mouse_x = x;
  old_mouse_y = y;

  if (button == GLUT_LEFT_BUTTON) {             // leve tlacitko aktivuje rotaci
    stav = (state == GLUT_DOWN) ? 1 : 0;        // otestuj stlaceni
  }
  else if (button == GLUT_RIGHT_BUTTON) {       // prave tlacitko aktivuje posun
    stav = (state == GLUT_DOWN) ? 2 : 0;        // otestuj stlaceni
  }
  else {
    stav = 0;
  }
  
  // prekresleni sceny
  glutPostRedisplay();
}


///////////////////////////////////////////////////////////////////////////////
// funkce volana jestlize se mys pohybuje a je stlaceno nektere tlacitko
// - vola GLUT, kdyz uzivatel pohne mysi

void onMouseMotion(int x, int y)
{
  if (stav == 1) {                          // rotace sceny
    scene_rot_y += x - old_mouse_x;         // vypocitat novou pozici
    scene_rot_x += y - old_mouse_y;
    glutPostRedisplay();                    // a prekreslit scenu
  }
  else if (stav == 2) {                     // posuv sceny
    scene_move_z += old_mouse_y - y;
    glutPostRedisplay();
  }

  // zapamatovat novou pozici kurzoru
  old_mouse_x = x;
  old_mouse_y = y;
}


///////////////////////////////////////////////////////////////////////////////
// funkce volana 40x za sekundu                                         
// - vola GLUT                                                       

void onTimer(int x)
{
  time++;                               // zaznacime, ze doslo ke zmene stavu
  glutPostRedisplay();                  // a rekneme, ze ma dojit k prekresleni
  glutTimerFunc(25, onTimer, 0);        // naplanujeme, ze za dalsich 25ms se ma funkce zavolat
}


///////////////////////////////////////////////////////////////////////////////
// hlavni funkce koznolove aplikace                                
// argc - pocet vstupnich parametru
// argv - pole vstupnich parametru

int main(int argc, char **argv)
{
  // inicializace knihovny GLUT
  glutInit(&argc, argv);

  // nastaveni grafickeho modu okna
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
  // pocatecni velikost okna
  glutInitWindowSize(default_w, default_h);
  // pocatecni pozice okna
//  glutInitWindowPosition(200, 100);
  // vytvoreni okna pro kresleni
  glutCreateWindow(PROGRAM_TITLE);

  // registrace funkce volane pri prekreslovani
  glutDisplayFunc(onDisplay);
  // registrace funkce volane pri zmene velikosti
  glutReshapeFunc(onReshape);
  // registrace funkce volane pri stisku klavesy
  glutKeyboardFunc(onKeyboard);
  // registrace funkce volane pri stisku tlacitek mysi
  glutMouseFunc(onMouseClick);
  // registrace funkce volane pri pohybu mysi
  glutMotionFunc(onMouseMotion);
  // registrace funkce volane za 25 milisekund
  glutTimerFunc(25, onTimer, 0);

  // provest nasi uzivatelskou inicializaci
  Init();
  InitMaterialDisplayLists();
  InitScene();

  // nekonecna smycka volajici registrovane funkce
  glutMainLoop();

  // ANSI C potrebuje ukoncit fci main prikazem return
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
