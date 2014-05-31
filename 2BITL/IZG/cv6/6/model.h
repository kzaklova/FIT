///////////////////////////////////////////////////////////////////////////////
// Polygonalni model kolecka
///////////////////////////////////////////////////////////////////////////////

// include guard
#ifndef MODEL_H
#define MODEL_H


///////////////////////////////////////////////////////////////////////////////
// globalni konstanty

// zakladni parametry kolecka
#define COG_COUNT       12          // pocet zubu kola
#define RADIUS          3.5f        // radius kola (vnitrni radius zubu)

// pevne parametry kolecka
#define COG_HEIGHT      1.0f        // vyska zubu
#define RADIUS_AXLE     1.0f        // radius diry pro osu v kole
#define WHEEL_THICKNESS 3.0f        // tloustka kola


///////////////////////////////////////////////////////////////////////////////
// vygenerovani display-listu ozubeneho kolecka
// - funkce se vola pri zacatku programu
// - vraci id vytvoreneho display-listu
// - radius - polomer kolecka
// - cog_count - pocet zubu na obvodu

int CreateWheelDisplayList(float radius, int cog_count);

///////////////////////////////////////////////////////////////////////////////
// vygenerovani textury pro zrcadlo
// - bitmapy RGBA o vhodnych rozmerech
// - funkce se vola jednou, pri zacatku programu
// - vraci id textury

unsigned CreateMirrorTexture();


#endif // MODEL_H

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
