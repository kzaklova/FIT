///////////////////////////////////////////////////////////////////////////////
// Polygonalni model kolecka
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// include

#ifdef _WIN32
#   include <windows.h>
#endif

#include "model.h"

#include <GL/gl.h>
#include <cmath>


///////////////////////////////////////////////////////////////////////////////
// vygenerovani display-listu ozubeneho kolecka
// - funkce se vola pri zacatku programu
// - vraci id vytvoreneho display-listu
// radius - polomer kolecka
// cog_count - pocet zubu na obvodu

#define CONST_PI    3.1415926f

int CreateWheelDisplayList(float radius, int cog_count)
{
  float phi = 0;

  // vygeneruje identifikator display-listu
  int id = glGenLists(1);
  
  // naplneni display-listu s koleckem 
  glNewList(id, GL_COMPILE);
  {
    glBegin(GL_QUADS);                          // vykreslujem same ctyruhelniky
    for (int i = 0; i < cog_count; i++) {       // pro kazdy zub...
      glNormal3f(0, 0, -1);
      glVertex3f(RADIUS_AXLE*cos(phi), RADIUS_AXLE*sin(phi), -WHEEL_THICKNESS/2.0f);
      glVertex3f((radius+COG_HEIGHT)*cos(phi), (radius+COG_HEIGHT)*sin(phi), -WHEEL_THICKNESS/2.0f);
      glVertex3f((radius+COG_HEIGHT)*cos(phi + 2/7.0f*2*CONST_PI/cog_count),
                 (radius+COG_HEIGHT)*sin(phi + 2/7.0f*2*CONST_PI/cog_count),
                 -WHEEL_THICKNESS/2.0f);
      glVertex3f(RADIUS_AXLE*cos(phi + 2/7.0f*2*CONST_PI/cog_count),
                 RADIUS_AXLE*sin(phi + 2/7.0f*2*CONST_PI/cog_count),
                 -WHEEL_THICKNESS/2.0f);
  
      glNormal3f(cos(phi + 2/7.0f*2*CONST_PI/cog_count/2), sin(phi + 2/7.0f*2*CONST_PI/cog_count/2), 0.0f);
      glVertex3f((radius+COG_HEIGHT)*cos(phi), (radius+COG_HEIGHT)*sin(phi), -WHEEL_THICKNESS/2.0f);
      glVertex3f((radius+COG_HEIGHT)*cos(phi + 2/7.0f*2*CONST_PI/cog_count),
                 (radius+COG_HEIGHT)*sin(phi + 2/7.0f*2*CONST_PI/cog_count),
                 -WHEEL_THICKNESS/2.0f
                 );
      glVertex3f((radius+COG_HEIGHT)*cos(phi + 2/7.0f*2*CONST_PI/cog_count),
                 (radius+COG_HEIGHT)*sin(phi + 2/7.0f*2*CONST_PI/cog_count),
                 WHEEL_THICKNESS/2.0f);
      glVertex3f((radius+COG_HEIGHT)*cos(phi), (radius+COG_HEIGHT)*sin(phi), WHEEL_THICKNESS/2.0f);
  
      glNormal3f(0, 0, 1);
      glVertex3f(RADIUS_AXLE*cos(phi), RADIUS_AXLE*sin(phi), WHEEL_THICKNESS/2.0f);
      glVertex3f((radius+COG_HEIGHT)*cos(phi), (radius+COG_HEIGHT)*sin(phi), WHEEL_THICKNESS/2.0f);
      glVertex3f((radius+COG_HEIGHT)*cos(phi + 2/7.0f*2*CONST_PI/cog_count),
                 (radius+COG_HEIGHT)*sin(phi + 2/7.0f*2*CONST_PI/cog_count),
                 WHEEL_THICKNESS/2.0f);
      glVertex3f(RADIUS_AXLE*cos(phi + 2/7.0f*2*CONST_PI/cog_count),
                 RADIUS_AXLE*sin(phi + 2/7.0f*2*CONST_PI/cog_count),
                 WHEEL_THICKNESS/2.0f);
  
      glNormal3f(-cos(phi + 2/7.0f*2*CONST_PI/cog_count/2), -sin(phi + 2/7.0f*2*CONST_PI/cog_count/2), 0.0f);
      glVertex3f(RADIUS_AXLE*cos(phi), RADIUS_AXLE*sin(phi), -WHEEL_THICKNESS/2.0f);
      glVertex3f(RADIUS_AXLE*cos(phi + 2/7.0f*2*CONST_PI/cog_count),
                 RADIUS_AXLE*sin(phi + 2/7.0f*2*CONST_PI/cog_count),
                 -WHEEL_THICKNESS/2.0f);
      glVertex3f(RADIUS_AXLE*cos(phi + 2/7.0f*2*CONST_PI/cog_count),
                 RADIUS_AXLE*sin(phi + 2/7.0f*2*CONST_PI/cog_count),
                 WHEEL_THICKNESS/2.0f);
      glVertex3f(RADIUS_AXLE*cos(phi), RADIUS_AXLE*sin(phi), WHEEL_THICKNESS/2.0f);
      phi += 2/7.0f*2*CONST_PI/cog_count;
  
      
      glNormal3f(0, 0, -1);
      glVertex3f(RADIUS_AXLE*cos(phi), RADIUS_AXLE*sin(phi), -WHEEL_THICKNESS/2.0f);
      glVertex3f((radius+COG_HEIGHT)*cos(phi), (radius+COG_HEIGHT)*sin(phi), -WHEEL_THICKNESS/2.0f);
      glVertex3f(radius*cos(phi + 1/7.0f*2*CONST_PI/cog_count),
                 radius*sin(phi + 1/7.0f*2*CONST_PI/cog_count),
                 -WHEEL_THICKNESS/2.0f);
      glVertex3f(RADIUS_AXLE*cos(phi + 1/7.0f*2*CONST_PI/cog_count),
                 RADIUS_AXLE*sin(phi + 1/7.0f*2*CONST_PI/cog_count),
                 -WHEEL_THICKNESS/2.0f);
  
      glNormal3f(cos(phi + 1/7.0f*2*CONST_PI/cog_count/2), sin(phi + 1/7.0f*2*CONST_PI/cog_count/2), 0.0f);
      glVertex3f((radius+COG_HEIGHT)*cos(phi), (radius+COG_HEIGHT)*sin(phi), -WHEEL_THICKNESS/2.0f);
      glVertex3f(radius*cos(phi + 1/7.0f*2*CONST_PI/cog_count),
                 radius*sin(phi + 1/7.0f*2*CONST_PI/cog_count),
                 -WHEEL_THICKNESS/2.0f);
      glVertex3f(radius*cos(phi + 1/7.0f*2*CONST_PI/cog_count),
                 radius*sin(phi + 1/7.0f*2*CONST_PI/cog_count),
                 WHEEL_THICKNESS/2.0f);
      glVertex3f((radius+COG_HEIGHT)*cos(phi), (radius+COG_HEIGHT)*sin(phi), WHEEL_THICKNESS/2.0f);
  
      glNormal3f(0, 0, 1);
      glVertex3f(RADIUS_AXLE*cos(phi), RADIUS_AXLE*sin(phi), WHEEL_THICKNESS/2.0f);
      glVertex3f((radius+COG_HEIGHT)*cos(phi), (radius+COG_HEIGHT)*sin(phi), WHEEL_THICKNESS/2.0f);
      glVertex3f(radius*cos(phi + 1/7.0f*2*CONST_PI/cog_count),
                 radius*sin(phi + 1/7.0f*2*CONST_PI/cog_count),
                 WHEEL_THICKNESS/2.0f);
      glVertex3f(RADIUS_AXLE*cos(phi + 1/7.0f*2*CONST_PI/cog_count),
                 RADIUS_AXLE*sin(phi + 1/7.0f*2*CONST_PI/cog_count),
                 WHEEL_THICKNESS/2.0f);
  
      glNormal3f(-cos(phi + 1/7.0f*2*CONST_PI/cog_count/2), -sin(phi + 1/7.0f*2*CONST_PI/cog_count/2), 0.0f);
      glVertex3f(RADIUS_AXLE*cos(phi), RADIUS_AXLE*sin(phi), -WHEEL_THICKNESS/2.0f);
      glVertex3f(RADIUS_AXLE*cos(phi + 1/7.0f*2*CONST_PI/cog_count),
                 RADIUS_AXLE*sin(phi + 1/7.0f*2*CONST_PI/cog_count),
                 -WHEEL_THICKNESS/2.0f);
      glVertex3f(RADIUS_AXLE*cos(phi + 1/7.0f*2*CONST_PI/cog_count),
                 RADIUS_AXLE*sin(phi + 1/7.0f*2*CONST_PI/cog_count),
                 WHEEL_THICKNESS/2.0f);
      glVertex3f(RADIUS_AXLE*cos(phi), RADIUS_AXLE*sin(phi), WHEEL_THICKNESS/2.0f);
      phi += 1/7.0f*2*CONST_PI/cog_count;
  
      
      glNormal3f(0, 0, -1);
      glVertex3f(RADIUS_AXLE*cos(phi), RADIUS_AXLE*sin(phi), -WHEEL_THICKNESS/2.0f);
      glVertex3f(radius*cos(phi), radius*sin(phi), -WHEEL_THICKNESS/2.0f);
      glVertex3f(radius*cos(phi + 3/7.0f*2*CONST_PI/cog_count),
                 radius*sin(phi + 3/7.0f*2*CONST_PI/cog_count),
                 -WHEEL_THICKNESS/2.0f);
      glVertex3f(RADIUS_AXLE*cos(phi + 3/7.0f*2*CONST_PI/cog_count),
                 RADIUS_AXLE*sin(phi + 3/7.0f*2*CONST_PI/cog_count),
                 -WHEEL_THICKNESS/2.0f);
  
      glNormal3f(cos(phi + 3/7.0f*2*CONST_PI/cog_count/2), sin(phi + 3/7.0f*2*CONST_PI/cog_count/2), 0.0f);
      glVertex3f(radius*cos(phi), radius*sin(phi), -WHEEL_THICKNESS/2.0f);
      glVertex3f(radius*cos(phi + 3/7.0f*2*CONST_PI/cog_count),
                 radius*sin(phi + 3/7.0f*2*CONST_PI/cog_count),
                 -WHEEL_THICKNESS/2.0f);
      glVertex3f(radius*cos(phi + 3/7.0f*2*CONST_PI/cog_count),
                 radius*sin(phi + 3/7.0f*2*CONST_PI/cog_count),
                 WHEEL_THICKNESS/2.0f);
      glVertex3f(radius*cos(phi), radius*sin(phi), WHEEL_THICKNESS/2.0f);
  
      glNormal3f(0, 0, 1);
      glVertex3f(RADIUS_AXLE*cos(phi), RADIUS_AXLE*sin(phi), WHEEL_THICKNESS/2.0f);
      glVertex3f(radius*cos(phi), radius*sin(phi), WHEEL_THICKNESS/2.0f);
      glVertex3f(radius*cos(phi + 3/7.0f*2*CONST_PI/cog_count),
                 radius*sin(phi + 3/7.0f*2*CONST_PI/cog_count),
                 WHEEL_THICKNESS/2.0f);
      glVertex3f(RADIUS_AXLE*cos(phi + 3/7.0f*2*CONST_PI/cog_count),
                 RADIUS_AXLE*sin(phi + 3/7.0f*2*CONST_PI/cog_count),
                 WHEEL_THICKNESS/2.0f);
  
      glNormal3f(-cos(phi + 3/7.0f*2*CONST_PI/cog_count/2), -sin(phi + 3/7.0f*2*CONST_PI/cog_count/2), 0.0f);
      glVertex3f(RADIUS_AXLE*cos(phi), RADIUS_AXLE*sin(phi), -WHEEL_THICKNESS/2.0f);
      glVertex3f(RADIUS_AXLE*cos(phi + 3/7.0f*2*CONST_PI/cog_count),
                 RADIUS_AXLE*sin(phi + 3/7.0f*2*CONST_PI/cog_count),
                 -WHEEL_THICKNESS/2.0f);
      glVertex3f(RADIUS_AXLE*cos(phi + 3/7.0f*2*CONST_PI/cog_count),
                 RADIUS_AXLE*sin(phi + 3/7.0f*2*CONST_PI/cog_count),
                 WHEEL_THICKNESS/2.0f);
      glVertex3f(RADIUS_AXLE*cos(phi), RADIUS_AXLE*sin(phi), WHEEL_THICKNESS/2.0f);
      phi += 3/7.0f*2*CONST_PI/cog_count;
  
      
      glNormal3f(0, 0, -1);
      glVertex3f(RADIUS_AXLE*cos(phi), RADIUS_AXLE*sin(phi), -WHEEL_THICKNESS/2.0f);
      glVertex3f(radius*cos(phi), radius*sin(phi), -WHEEL_THICKNESS/2.0f);
      glVertex3f((radius+COG_HEIGHT)*cos(phi + 1/7.0f*2*CONST_PI/cog_count),
                 (radius+COG_HEIGHT)*sin(phi + 1/7.0f*2*CONST_PI/cog_count),
                 -WHEEL_THICKNESS/2.0f);
      glVertex3f(RADIUS_AXLE*cos(phi + 1/7.0f*2*CONST_PI/cog_count),
                 RADIUS_AXLE*sin(phi + 1/7.0f*2*CONST_PI/cog_count),
                 -WHEEL_THICKNESS/2.0f);
  
      glNormal3f(cos(phi + 1/7.0f*2*CONST_PI/cog_count/2), sin(phi + 1/7.0f*2*CONST_PI/cog_count/2), 0.0f);
      glVertex3f(radius*cos(phi), radius*sin(phi), -WHEEL_THICKNESS/2.0f);
      glVertex3f((radius+COG_HEIGHT)*cos(phi + 1/7.0f*2*CONST_PI/cog_count),
                 (radius+COG_HEIGHT)*sin(phi + 1/7.0f*2*CONST_PI/cog_count),
                 -WHEEL_THICKNESS/2.0f);
      glVertex3f((radius+COG_HEIGHT)*cos(phi + 1/7.0f*2*CONST_PI/cog_count),
                 (radius+COG_HEIGHT)*sin(phi + 1/7.0f*2*CONST_PI/cog_count),
                 WHEEL_THICKNESS/2.0f);
      glVertex3f(radius*cos(phi), radius*sin(phi), WHEEL_THICKNESS/2.0f);
  
      glNormal3f(0, 0, 1);
      glVertex3f(RADIUS_AXLE*cos(phi), RADIUS_AXLE*sin(phi), WHEEL_THICKNESS/2.0f);
      glVertex3f(radius*cos(phi), radius*sin(phi), WHEEL_THICKNESS/2.0f);
      glVertex3f((radius+COG_HEIGHT)*cos(phi + 1/7.0f*2*CONST_PI/cog_count),
                 (radius+COG_HEIGHT)*sin(phi + 1/7.0f*2*CONST_PI/cog_count),
                 WHEEL_THICKNESS/2.0f);
      glVertex3f(RADIUS_AXLE*cos(phi + 1/7.0f*2*CONST_PI/cog_count),
                 RADIUS_AXLE*sin(phi + 1/7.0f*2*CONST_PI/cog_count),
                 WHEEL_THICKNESS/2.0f);
  
      glNormal3f(-cos(phi + 1/7.0f*2*CONST_PI/cog_count/2), -sin(phi + 1/7.0f*2*CONST_PI/cog_count/2), 0.0f);
      glVertex3f(RADIUS_AXLE*cos(phi), RADIUS_AXLE*sin(phi), -WHEEL_THICKNESS/2.0f);
      glVertex3f(RADIUS_AXLE*cos(phi + 1/7.0f*2*CONST_PI/cog_count),
                 RADIUS_AXLE*sin(phi + 1/7.0f*2*CONST_PI/cog_count),
                 -WHEEL_THICKNESS/2.0f);
      glVertex3f(RADIUS_AXLE*cos(phi + 1/7.0f*2*CONST_PI/cog_count),
                 RADIUS_AXLE*sin(phi + 1/7.0f*2*CONST_PI/cog_count),
                 WHEEL_THICKNESS/2.0f);
      glVertex3f(RADIUS_AXLE*cos(phi), RADIUS_AXLE*sin(phi), WHEEL_THICKNESS/2.0f);
      phi += 1/7.0f*2*CONST_PI/cog_count;
    }                                           // konec cyklu pro kazdy zub
    glEnd();                                    // GL_QUADS
  }
  glEndList();                                  // to je ke kolecku vsechno...

  // vratime identifikator display-listu
  return id;
}


///////////////////////////////////////////////////////////////////////////////
// vygenerovani textury
// - bitmapy RGBA o vhodnych rozmerech
// - funkce se vola jednou, pri zacatku programu
// - vraci id textury

// parametry textury
#define TEXTURE_WIDTH   256     // sirka textury - musi byt mocnina 2
#define TEXTURE_HEIGHT  256     // vyska textury - musi byt mocnina 2

// pole kde budou data textury
unsigned char texture[TEXTURE_WIDTH * TEXTURE_HEIGHT * 4];

unsigned CreateMirrorTexture()
{
  unsigned char *p = texture;                   // ukazatel, ktery "leze" pres texturu
  for (int i = 0; i < TEXTURE_HEIGHT; ++i) {
    for (int j = 0; j < TEXTURE_WIDTH; ++j) {
      *p++ = (((int)(i+10*(sin(j/20.0f)+1))&0x10)^(j&0x10)) ? 0xff:160; // red
      *p++ = (((int)(i+10*(sin(j/20.0f)+1))&0x10)^(j&0x10)) ? 0xff:180; // green
      *p++ = (((int)(i+10*(sin(j/20.0f)+1))&0x10)^(j&0x10)) ? 0xff:180; // blue
      *p++ = (((int)(i+10*(sin(j/20.0f)+1))&0x10)^(j&0x10)) ? 0xff:160; // alpha
    }
  }

  // vygeneruje (prideli) identifikator textury - ulozi do promenne texid
  unsigned id;
  glGenTextures(0, &id);

  // texid bude aktivni textura
  glBindTexture(GL_TEXTURE_2D, id);

  // nastaveni parametru textury
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // vytvoreni textury - podle naseho vygenerovaneho pole v promenne texture
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
               TEXTURE_WIDTH, TEXTURE_HEIGHT, 
               0, GL_RGBA, GL_UNSIGNED_BYTE, texture);

  // vratime id
  return id;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
