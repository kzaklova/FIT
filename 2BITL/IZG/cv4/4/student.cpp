///////////////////////////////////////////////////////////////////////////////
// Soubor studentskych funkci
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// include

#include "main.h"

#include <assert.h>
#include <stdlib.h>
#include <iostream>

#include <cmath>

///////////////////////////////////////////////////////////////////////////////
// name spaces

using namespace std;

// mocnina
double Power(double a, int b)
{
  double result = 1.0;
  for (int i = 0; i < b; ++i)
  {
  	result *= a;
  }
  return result;
}

// vypocet faktorialu
double Factorial(int n)
{
  if (n <= 0)
  {
    return 1.0;
  }
  else
  {
    return n * Factorial(n - 1);
  }
}

// vypocet kombinacniho cisla 'n' nad 'k'
double BinomialCoefficient(int n, int k)
{
  if (n < k)
  {
      return 0.0;
  }
  else
  {
      return Factorial(n) / (Factorial(k) * Factorial(n - k));
  }
}

double BernsteinPolynom(int n, int i, double t)
{
  return BinomialCoefficient(n, i) * Power(t, i) * Power(1.0-t, n-i);
}

//------------------------Zde dopiste Vas kod-------------------------
// generovani bodu Bezierovy krivky
void    Bezier(int quality, const T_PointVector & points, T_PointVector & line_points)
{
	// vymazani vysledneho seznamu bodu
	line_points.clear();
 
	S_Point bod;
	double krok = 1.0/quality;
	double citatel, jmenovatel;

	for (int i=0; i < quality; ++i){
		citatel = 0.0, jmenovatel = 0.0;

		for (unsigned j = 0; j < points.size(); ++j) {
			citatel += points[j].x * points[j].weight * BernsteinPolynom(points.size() - 1, j, i * krok);
			jmenovatel += points[j].weight * BernsteinPolynom(points.size() - 1, j, i * krok);
		}
		bod.x = citatel/jmenovatel; 

		citatel = 0.0, jmenovatel = 0.0;

		for (unsigned j = 0; j < points.size(); ++j) {
			citatel += points[j].y * points[j].weight * BernsteinPolynom(points.size() - 1, j, i * krok);
			jmenovatel += points[j].weight * BernsteinPolynom(points.size() - 1, j, i * krok);
		}
		bod.y = citatel/jmenovatel;
		line_points.push_back(bod);

	}

}

///////////////////////////////////////////////////////////////////////////////
// generovani bodu Fergussonovi kubiky
void Fergusson(int quality, const T_PointVector & points,
		const T_PointVector & vectors, T_PointVector & line_points)
{
	// vymazani vysledneho seznamu bodu
	line_points.clear();

	double stepQuality = 1.0 / (double) quality;

	//Vypocteny bod krivky
	S_Point linePoint;

	//Matice koeficientu Hermitovskeho polynomu
	double coefMat[4][4] =
	{
	{ 2.0, -2.0, 1.0, 1.0 },
	{ -3.0, 3.0, -2.0, -1.0 },
	{ 0.0, 0.0, 1.0, 0.0 },
	{ 1.0, 0.0, 0.0, 0.0 } };

	//Prochazime ridicimi body
	for (unsigned int cPoint = 1; cPoint < points.size(); cPoint++)
	{
		//Pocitame vsechny body krivky mezi dvema ridicimi body
		double t = 0.0;
		for (int p = 0; p < quality; p++, t += stepQuality)
		{
			double tMat[1][4] =	{ { t * t * t, t * t, t, 1.0 } };
			//T x CoefMAt
			double tmpMat[1][4] = {	{ 0.0, 0.0, 0.0, 0.0 } };
			//Nasobime T x CoefMat
			for (unsigned int row = 0; row < 1; row++)
			{
				for (unsigned int col = 0; col < 4; col++)
				{
					for (unsigned int AB = 0; AB < 4; AB++)
					{
						tmpMat[row][col] += tMat[row][AB] * coefMat[AB][col];
					}
				}
			}

			linePoint.x = tmpMat[0][0] * points[cPoint - 1].x;
			linePoint.x += tmpMat[0][1] * points[cPoint].x;
			linePoint.x += tmpMat[0][2] * (vectors[cPoint - 1].x - points[cPoint - 1].x);
			linePoint.x += tmpMat[0][3]	* (vectors[cPoint].x - points[cPoint].x);

			linePoint.y = tmpMat[0][0] * points[cPoint - 1].y;
			linePoint.y += tmpMat[0][1] * points[cPoint].y;
			linePoint.y += tmpMat[0][2] * (vectors[cPoint - 1].y - points[cPoint - 1].y);
			linePoint.y += tmpMat[0][3]	* (vectors[cPoint].y - points[cPoint].y);

			//Ulozime vypocteny bod krivky
			line_points.push_back(linePoint);
		}
	}
}
