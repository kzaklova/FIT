	////////////////////////////////////////////////////////////////////////////////
// Vzorove reseni
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// include 

#include "main.h"
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

////////////////////////////////////////////////////////////////////////////////
// Funkce vyplnena ve cviceni - nebodovany ukol
////////////////////////////////////////////////////////////////////////////////

void DrawLine (int x1, int y1, int x2, int y2)
{
	// test existence frame bufferu
	assert(frame_buffer != NULL);

	// implementujte
	S_RGBA color = COLOR_RED;

	int dx = x2 - x1;
	int dy = y2 - y1;

	int aux_x = 1;
	int aux_y = 1;
	int swap_lines = 0;
	
	if (dx < 0)
	{
		dx = -dx;
		aux_x = -1;
	}
	if (dy < 0)
	{
		dy = -dy;
		aux_y = -1;
	}

	if (dx < dy)
	{
		SWAP(dx,dy);
		SWAP(aux_x, aux_y);
		swap_lines = 1;
	}

	int y = 0;

	int P1 = 2*dy;
	int P2 = P1 - 2*dx;
	int P = 2*dy - dx;

	for (int x = 0; x <= dx; x++) {
		if (swap_lines)
			PutPixel(x1 + aux_y*y, y1 + aux_x*x, COLOR_RED);
		else
			PutPixel(x1 + aux_x*x, y1 + aux_y*y, COLOR_GREEN);

		if (P >= 0) {
			P += P2;
			y++;
		}
		else
			P += P1;
	}
	
	return;
}


////////////////////////////////////////////////////////////////////////////////
// Ukol za 3b
////////////////////////////////////////////////////////////////////////////////

void DrawCircle (int s1, int s2, int R)
{
	// test existence frame bufferu
	assert(frame_buffer != NULL);

	// implementujte
	S_RGBA color = COLOR_GREEN;

	int x = 0;
	int y = R;
	int P = 1 - R;
	int X2 = 3;
	int Y2 = 2 * R - 2;

	while (x < y)
	{
		PutPixel(s1 + x, s2 + y, COLOR_BLUE);
		PutPixel(s1 - x, s2 - y, COLOR_RED);
		PutPixel(s1 + x, s2 - y, COLOR_GREEN);
		PutPixel(s1 - x, s2 + y, COLOR_GREY);
		PutPixel(s1 + y, s2 + x, COLOR_RED);
		PutPixel(s1 - y, s2 - x, COLOR_BLUE);
		PutPixel(s1 + y, s2 - x, COLOR_GREY);
		PutPixel(s1 - y, s2 + x, COLOR_GREEN);

		if (P >= 0) {
			P += -Y2;
			Y2 -= 2;
			y--;
		}
		P += X2;
		X2 += 2;
		x++;
	}

	return;
}

////////////////////////////////////////////////////////////////////////////////
// konec souboru student.cpp
////////////////////////////////////////////////////////////////////////////////
