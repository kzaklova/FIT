///////////////////////////////////////////////////////////////////////////////
// Soubor studentskych funkci
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// include

#include "main.h"

#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <math.h>

///////////////////////////////////////////////////////////////////////////////
// name spaces

using namespace std;

///////////////////////////// UDELAT VE CVICENI ///////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// seminkove vyplnovani
// x, y - souradnice pocatecniho bodu/seminka
// color - vyplnovaci barva
void FloodFill(int x, int y, const S_RGBA & color)
{
	// Zde zanechte vas kod
	S_RGBA orig, tmp;
	S_Point point;
	S_Point temp;
	vector<S_Point> pointstack;
	
	GetPixel(x, y, orig);
	
	point.x = x;
	point.y = y;
	//prvni bod na zasobnik
	pointstack.push_back(point);

	while (pointstack.size() != 0) {
		point = pointstack.back();
		pointstack.pop_back();
		GetPixel(point.x, point.y, tmp);

		if (color != orig && orig == tmp && point.x < frame_w && point.y < frame_h) {
			PutPixel(point.x,point.y,color);
			temp = point;

			// x+1,y
			if (point.x < frame_w) {
				point.x++;
				pointstack.push_back(point);
				point = temp;
			}
			// x-1,y
			if (point.x > 0) {
				point.x--;
				pointstack.push_back(point);
				point = temp;
			}
			// x,y-1
			if (point.y > 0) {
				point.y--;
				pointstack.push_back(point);
				point = temp;
			}
			// x,y+1
			if (point.y < frame_h) {
				point.y++;
				pointstack.push_back(point);
			}

			
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// radkove vyplnovani
// points - pole vrcholu oblasti, posledni hranice je z posledniho do prvniho bodu.
// size - delka pole vrcholu
// color1 - barva hranic a pocatecni barva gradientu
// color2 - koncova barva gradientu

void InvertFill(const S_Point * points, const int size,  const S_RGBA & color1, const S_RGBA & color2)
{
	// Zde zanechte vas kod
	LineStack ls;
	
	//zostavme zoznam hran
	for(int a = 0; a < size; a++)
	{
		S_Point p1, p2;
		p1 = points[a];		//bod 1
		if(a < size-1)		//bod 2, posledna hrana bude z posledneho bodu do prveho
			p2 = points[a+1];
		else
			p2 = points[0]; 

		//ak je hrana rovnobezna s riadkom, ignorujeme ju
		if(p1.y != p2.y) 
		{
			//hranu pridame do zoznamu tak, aby bola orientovana z dola nahor
			if(p1.y < p2.y)
			{
				ls.push_back(S_Line(p1, p2));
			} else {
				ls.push_back(S_Line(p2, p1));
			}
		}
	}

	//vytvorenie masky
	S_Mask m(frame_w, frame_h);

	//modifikacia masky pre kazdu hranu
	for(int a = 0; a < ls.size(); a++) 
	{
		S_Line line = ls[a];	//vyberieme hranu zo zoznamu
		
		/////////// SEM DOPLNTE VAS KOD ////////////////////
		//test zvislosti usecky
		bool vertical = false;
		if(line.point1.x == line.point2.x)
			vertical = true;

		double K, Q;		//K - smernica, Q - intercept
		if(!vertical)
		{
			K = (double)(line.point1.y - line.point2.y) / 
				(double)(line.point1.x - line.point2.x);
			Q = line.point1.y - K * line.point1.x;
		}

		//postupuj pre kazdy riadok
		for(int b = line.point1.y; b < line.point2.y; b++) 
		{
			//najdi priesecnik hrany s X
			int P;
			
			if(vertical) {
				P = line.point1.x;
			} else
				P = (int)((b - Q) / K);

			//invertuj masku napravo od tohto bodu
			for(int c = P; c < frame_w; c++) 
			{
				if(m.mask[c][b] == 0)
					m.mask[c][b] = 1;
				else
					m.mask[c][b] = 0;
			}
		}
		

	}

	//vykresli pixely tak, kde maska je 1
	for(int a = 0; a < frame_h; a++)
		for(int b = 0; b < frame_w; b++)
			if(m.mask[b][a] == 1)
				PutPixel(b, a, color1);

	//uvolni pamat
}
