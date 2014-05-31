///////////////////////////////////////////////////////////////////////////////
// Soubor studentskych funkci
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#   include <windows.h>
#endif

#include "main.h"

#include <GL/gl.h>
#include <cmath>


///////////////////////////////////////////////////////////////////////////////
// tato funkce je volana na zacatku programu

void InitScene(void)
{
  // vygenerovani display listu kolecka
  wheel = CreateWheelDisplayList(RADIUS, COG_COUNT);
  
  // generovani textury pro zrcadlo
  texid = CreateMirrorTexture();
}

void DrawLine() {
glDisable(GL_LIGHTING);
glLineWidth(4);
glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(5.0f, 0.0f, 0.0f);

	glVertex3f(0.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 5.0f, 0.0f);

	glVertex3f(0.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 5.0f);
glEnd();
glEnable(GL_LIGHTING);
}

void DrawTriangle() {
glDisable(GL_LIGHTING);
glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 5.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 5.0f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(5.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 5.0f, 0.0f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 5.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(5.0f, 0.0f, 0.0f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 5.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 5.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(5.0f, 0.0f, 0.0f);
glEnd();
glEnable(GL_LIGHTING);
}

///////////////////////////////////////////////////////////////////////////////
// tato funkce je volana pro vykresleni sceny

void DrawScene(void)
{
  ////////////////////////////////////////////////////////////////
  // nastaveni kamery

  glTranslated(0.0, 0.0, scene_move_z);       // posun sceny podle pohybu mysi
  glRotated(scene_rot_x, 1.0, 0.0, 0.0);      // rotace sceny podle pohybu kurzoru mysi
  glRotated(scene_rot_y, 0.0, 1.0, 0.0);

  ////////////////////////////////////////////////////////////////
  // vykresleni sceny

  // kolecka vpredu
#if 1
  glPushMatrix();                               // zapamatovat souradny system
      glTranslatef(0.0f, 0.0f, 5.0f);

    glCallList(red_mat);                        // cerveny material
    glPushMatrix();                             // zapamatujem souradny system
      glTranslated(-(RADIUS+COG_HEIGHT), 0.0, 0.0); // posun v X
      glRotated(2*time+5.6, 0.0, 0.0, 1.0);         // ANIMACE - rotace podle promenne time
      glCallList(wheel);                        // vykreslime kolecko
      DrawLine();
      DrawTriangle();
    glPopMatrix();                              // vratime souradny system

    glCallList(blue_mat);
    glPushMatrix();
      glTranslated(RADIUS+COG_HEIGHT-1, 0.0, 0.0);
      glRotated(-2*time, 0.0, 0.0, 1.0);
      glCallList(wheel);
      DrawLine();
      DrawTriangle();
    glPopMatrix();
  glPopMatrix();                                // vratit zapamatovany souradny system
#endif

  // vykresleni POLOHY zrcadla do stencil-bufferu
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // zakaze zapis do color-bufferu
  glDepthMask(GL_FALSE);                        // zakaze zapis do depth-bufferu
  glEnable(GL_CULL_FACE);                       // vykreslovani jen licu polygonu
  glEnable(GL_STENCIL_TEST);                    // zapne praci se stencil-bufferem
  glStencilFunc(GL_ALWAYS, 1, 1);               // zapis VZDY
  glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);       // inkrementujeme hodnotu ve stencil-bufferu
  glBegin(GL_QUADS);                            // obdelnik
    glVertex3f(-15.0f, -10.0f, 0.0f);
    glVertex3f( 15.0f, -10.0f, 0.0f);
    glVertex3f( 15.0f,  10.0f, 0.0f);
    glVertex3f(-15.0f,  10.0f, 0.0f);
  glEnd();
  glDisable(GL_CULL_FACE);                      // vratit nastaveni - licove i rubove strany polygonu
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // vratit nastaveni - bude se kreslit
  glDepthMask(GL_TRUE);                         // vratit nastaveni - bude se psat do depth-bufferu

#if 1
  // kolecka jeste jednou - tentokrate za zrcadlem
  glEnable(GL_STENCIL_TEST);                    // zapne praci se stencil-bufferem
  glStencilFunc(GL_EQUAL, 1, 1);                // podminka stencil bufferu - musi tam byt 1
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);       // operace stencil bufferu - necha tam, co bylo
  glPushMatrix();
    glTranslatef(0.0f, 0.0f, -5.0f);

    glCallList(red_mat);
    glPushMatrix();
      glTranslated(-(RADIUS+COG_HEIGHT), 0.0, 0.0);
      glRotated(2*time+5.6, 0.0, 0.0, 1.0);
      glCallList(wheel);
      glScalef(1.0, 1.0, -1.0);
      DrawLine();
      DrawTriangle();
    glPopMatrix();

    glCallList(blue_mat);
    glPushMatrix();
      glTranslated(RADIUS+COG_HEIGHT-1, 0.0, 0.0);
      glRotated(-2*time, 0.0, 0.0, 1.0);
      glCallList(wheel);
      glScalef(1.0, 1.0, -1.0);
      DrawLine();
      DrawTriangle();
    glPopMatrix();
  glPopMatrix();
  glDisable(GL_STENCIL_TEST);                   // vratit nastaveni - vypneme praci se stencil bufferem
#endif

  // mrizka z car dole
  glDisable(GL_LIGHTING);                       // nepracujeme se svetlem - porad stejne sede
  glColor3f(0.5f, 0.5f, 0.5f);                  // seda barva
  glLineWidth(1.0);                             // tloustka cary - 1 pixel
  glBegin(GL_LINES);                            // budem kreslit cary
  for (int i = -10; i <= 10; i++) {
    glColor3f(1.0f, 0.0f, 0.0f);  glVertex3d(1.5*i, -10.0, -20.0);
    glColor3f(0.0f, 1.0f, 0.0f);  glVertex3d(1.5*i, -10.0,  20.0);
    glColor3f(1.0f, 1.0f, 1.0f);  glVertex3d(-20.0, -10.0, 1.5*i);
    glColor3f(0.0f, 0.0f, 0.0f);  glVertex3d( 20.0, -10.0, 1.5*i);
  }
  glEnd();
  glEnable(GL_LIGHTING);                        // vratit nastaveni - zapnout osvetleni

#if 1
  // material zrcadla
  GLfloat mat_diffuse[]  = {0.8f, 0.8f, 0.0f, 1.0f}; // difuzni (matna) slozka materialu
  GLfloat mat_ambient[]  = {0.4f, 0.4f, 0.0f, 1.0f}; // ambient (rozptylena) slozka materialu
  GLfloat mat_specular[] = {0.6f, 0.9f, 0.7f, 1.0f}; // specular (odleskova) slozka materialu

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 125);

  // vykresleni zrcadla s texturou, ktera udava barvy a pruhlednost
  glEnable(GL_TEXTURE_2D);                      // zapnout texturovani
  glEnable(GL_BLEND);                           // zapnout pruhlednost
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // pruhlednostni funkce - alpha slozka materialu
  glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);               // nastaveni normaloveho vektoru
    glTexCoord2f(0.0f, 0.0f);  glVertex3f(-15.0f, -10.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);  glVertex3f( 15.0f, -10.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);  glVertex3f( 15.0f,  10.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f);  glVertex3f(-15.0f,  10.0f, 0.0f);
  glEnd();
  glDisable(GL_BLEND);                          // vratit nastaveni - vypnout michani/pruhlednost
  glDisable(GL_TEXTURE_2D);                     // vratit nastaveni - vypnout texturovani
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
