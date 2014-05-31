///////////////////////////////////////////////////////////////////////////////
// Definice dulezitych globalnich promennych a funkci
///////////////////////////////////////////////////////////////////////////////

// include guard
#ifndef MAIN_H
#define MAIN_H

///////////////////////////////////////////////////////////////////////////////
// include

#include "model.h"


///////////////////////////////////////////////////////////////////////////////
// funkce specificke pro toto cviceni

// tato funkce je volana na zacatku programu
// - implementace ve student.cpp
void InitScene(void);

// tato funkce je volana pro vykresleni sceny
// - implementace ve student.cpp
void DrawScene(void);


///////////////////////////////////////////////////////////////////////////////
// promenne specificke pro toto cviceni
// - promenne meni main.cpp v reakci na mysovani
// - student.cpp hodnoty pouziva pri kresleni sceny (rotace objektu, apod.)

// natoceni cele sceny se vsim vsady
extern int                scene_rot_x;
extern int                scene_rot_y;

// posun cele sceny
extern int                scene_move_z;

// virtualni cas [s] animace
extern int                time;

// identifikator display-listu kolecka
extern int                wheel;

// display-listy pro materialy
extern int                red_mat, blue_mat;

// identifikator textury zrcadla
extern unsigned           texid;


#endif // MAIN_H

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
