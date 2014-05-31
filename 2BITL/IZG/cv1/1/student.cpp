////////////////////////////////////////////////////////////////////////////////////////////////
// Vzorove reseni
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// include 

#include "main.h"
#include <assert.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////////////////////
// Funkce vyplnene ve cvicenich
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// funkce zajistujici zobrazeni pixelu dane barvy na danych souradnicich
// 0.5 b
void PutPixel(int x, int y, S_RGBA color)
{
	if (x >= 0 && y >= 0 && x < frame_w && y < frame_h)
	{
		*(frame_buffer + frame_w * y + x) = color;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// funkce zajistujici ziskani barvy pixelu na danych souradnicich
// pokud je pixel mimo hranice, vrátí funkce barvu (0,0,0,0)
// 0.5 b
void GetPixel(int x, int y, S_RGBA & color)
{
	if (x >= 0 && y >= 0 && x < frame_w && y < frame_h)
	{
		color = frame_buffer[frame_w*y + x];
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// funkce prevede obrazek na urovne sedi
void GreyScale()
{
	int y, x;
	S_RGBA color;
	unsigned char intensity;
	for (y = 0; y < frame_h; y++)
		for (x = 0; x < frame_w; x++)
		{
			GetPixel(x, y, color);
			intensity = ROUND(color.red * 0.299 + color.green * 0.587 + color.blue * 0.114);
			color.red = intensity;
			color.green = intensity;
			color.blue = intensity;
			PutPixel(x, y, color);
		}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// funkce prevede obrazek na cernobily
//
// Threshold    - prah
// 1 bod
void Thresholding(int Threshold)
{
	GreyScale();
	for(int i = 0; i < frame_h * frame_w; i++)
	{
		frame_buffer[i] = frame_buffer[i].red > Threshold ? COLOR_WHITE : COLOR_BLACK;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// funkce prevede obrazek na cernobily
// pomoci distribuce chyby metodou Floyd-Steinberg, velikost obrazku nemeni.
// pouzijte promenne definovane v main.h:
// 1 bod

void Error(int i, int j, float err)
{
	S_RGBA color;
	int aux;
	if (i >= 0 && j >= 0 && i < frame_w && j < frame_h)
	{
		GetPixel(i,j,color);
		aux = ROUND(color.green + err);
		if (aux > 255) aux = 255;
		if (aux < 0) aux = 0;
		color.blue = aux;
		color.green = aux;
		color.red = aux;
		PutPixel(i,j,color);
	}

}

void ErrorDiffusion()
{
	S_RGBA color;
	int err = 0;
	int aux;
	GreyScale();
	for (int i = 0; i < frame_w; i++)
		for (int j = 0; j < frame_h; j++)
		{
			GetPixel(i,j,color);
			aux = color.red;
			if (color.red <= 127)
				color = COLOR_BLACK;
			else
				color = COLOR_WHITE;
			err = aux - color.red;
			PutPixel(i,j,color);
			
			Error(i+1,j,err*3.0/8.0);
			Error(i,j+1,err*3.0/8.0);
			Error(i+1,j+1,err*1.0/4.0);
		}
}

