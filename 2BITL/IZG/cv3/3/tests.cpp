/*
	Soubor s podobnymi testy, jake budou pouzity i pri kontrole ukolu. 
    Pokud vase reseni projde timto, tak projde i pri kontrole. Zmena bude
    v poctu bodu, jejich souradnicich a pouzitych barvach. Testovane jevy
    zustavaji...
    
    UPOZORNENI: 
    Testy budou spousteny postupne a v tom poradi, v jakem jsou zde. 
    Pokud chcete nejake body a vite, ze vas algoritmus nezvladne vsechno dobre,
    tak se snazte, aby alespon nespadl a dokoncil testy vsechny.
    
    Omluvte pouzitou czenglish v komentarich...
*/

#include "main.h"

// Vstupni souradnice
const int points1[4][2] = {{10,10},{128,10},{128,67},{10,67}};
const int points2[3][2] = {{50,50},{150,50},{100,200}};
const int points31[8][2] = {{0,0},{128,128},{97,16},{15,200},{2,22},{232,86},{6,137},{68,68}};
const int points41[4][2] = {{120, 60}, {33, 150}, {86, 180}, {210, 53}};
const int points5[8][2] = {{160, 160}, {80, 40}, {100, 100},{50,30},{211,37},{144,13},{213,61}, {111,48}};
const int points7[2][2] = {{0,0},{128,128}};
const int points8[16][2] = {{0,0},{128,128},{128,128},{0,0},{2,22},{232,86},{6,137},{68,68},{212,137},{15,91},{97,3},{111,48},{11,3},{211,37},{144,13},{213,61}};
const int points9[4][2] = {{120, 200}, {300, 120},{101, 200},{100, 50}};
const int points10[4][2] = {{10,10},{128,10},{10,67},{30,167}};

// Vytvor pole bodu
S_Point * makeSeedStack(const int points[][2], int length)
{

	S_Point * s = new S_Point[ length ];

	for(int i = 0; i < length; i++)
		s[ i ] = S_Point(points[i][0], points[i][1]);

	return s;
}

// spust jeden z testu
void runTest(int test)
{
    S_Point * points(NULL);

	switch(test)
	{
	//-----------------------------
	// Simple tests - solid fill

	// 1) Try to create filled rectangle
	case 1:
        points = makeSeedStack(points1, 4);
		InvertFill( points, 4, S_RGBA(0, 255, 0), S_RGBA(255,0,0));
        delete [] points;
		break;

	// 2) Try to create polygon
	case 2:
        points = makeSeedStack(points2, 3);
		InvertFill( points, 3, S_RGBA(0, 255, 0), S_RGBA(255,0,0));
        delete [] points;
		break;

	// 3) Create polygon over polygon
	case 3:
        points = makeSeedStack(points31, 8);
		InvertFill( points, 8, S_RGBA(0, 255, 0), S_RGBA(255,0,0));
        delete [] points;
		break;

	//------------------------------
	// Stress tests

	// 4) Try to create polygons out of view
	case 4:
        points = makeSeedStack(points41, 4);
		InvertFill( points, 4, S_RGBA(0, 255, 0), S_RGBA(255,0,0));
        delete [] points;
		break;

	// 5) Try to create polygon partialy out of view
	case 5:
        points = makeSeedStack(points5, 8);
		InvertFill( points, 8, S_RGBA(0, 255, 0), S_RGBA(255,0,0));
        delete [] points;
		break;

	// 6) Try to create wrong polygon - zero points
	case 6:
        points = new S_Point[0];
 		InvertFill( points, 0, S_RGBA(0, 255, 0), S_RGBA(255,0,0));
        delete [] points;
 		break;

	// 7) Try to create wrong polygon - two points. Line must be drawn.
	case 7:
        points = makeSeedStack(points7, 2);
		InvertFill( points, 2, S_RGBA(0, 255, 0), S_RGBA(255,0,0));
        delete [] points;
		break;

	// 8) Try to create partialy not filled polygon 
	case 8:
        points = makeSeedStack(points8, 16);
		InvertFill( points, 16, S_RGBA(0, 255, 0), S_RGBA(255,0,0));
        delete [] points;
		break;

	// 9) Try to create really big polygon
	case 9:
        points = makeSeedStack(points9, 4);
		InvertFill( points, 4, S_RGBA(0, 255, 0), S_RGBA(255,0,0));
        delete [] points;
		break;

	// 10) Try to create polygon with zero length side
	case 10:
        points = makeSeedStack(points10, 4);
		InvertFill( points, 4, S_RGBA(0, 255, 0), S_RGBA(255,0,0));
        delete [] points;
		break;
	}


}  // void runTest()

// Copy bitmap 
// x, y - destination position
// dx1 - source width
// dy1 - source height
// dx2 - destination width

void copyBuffer(S_RGBA * src, S_RGBA * dst, int x, int y, int dx1, int dy1, int dx2 )
{
  // Prepocitana pozice v cilovem bufferu;
  S_RGBA * dstP(dst + x + y * dx2);

  // castecna kopie obsahu frame bufferu
  for( int i = 0; i < dy1; i++ )
    memcpy( dstP + i*dx2, src + i*dx1, dx1*sizeof(S_RGBA) );
}

// run all tests and store them to the tiled bitmap
void runAllMakeTiled()
{
  int fw = 5 * frame_w;
  int fh = 2 * frame_h;
  
  // alokace pameti 
  S_RGBA * tiled = new S_RGBA[fw * fh];
  memset( tiled, 0, fw*fh*sizeof(S_RGBA) );

  // vsechny testy
  for( int y = 0; y < 2; ++y)
    for( int x = 0; x < 5; ++x)
    {
      // Resolve test number
      int test = 1 + y*5+x;

      // Clear frame buffer
      clearFB();
      
      // Run test
      runTest(test);
      
      // Copy bitmap
      copyBuffer( frame_buffer, tiled, x * frame_w, y * frame_h, frame_w, frame_h, fw );
    
    }

   // Clear frame buffer
   clearFB();

   // save bitmap
   SaveBitmap( "tiled.bmp", tiled, fw, fh );

   // Dealokace pameti
   delete [] tiled;
}
