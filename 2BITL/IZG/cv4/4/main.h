///////////////////////////////////////////////////////////////////////////////
// Definice dulezitych globalnich promennych a funkci
///////////////////////////////////////////////////////////////////////////////

// include guard
#ifndef MAIN_H
#define MAIN_H

///////////////////////////////////////////////////////////////////////////////
// include

#include<vector>


///////////////////////////////////////////////////////////////////////////////
// name spaces

using namespace std;


///////////////////////////////////////////////////////////////////////////////
// definice typu

// struktura typu RGBA barvy, 32 bit
struct S_RGBA
{
  unsigned char red;
  unsigned char green;
  unsigned char blue;
  unsigned char alpha;
};

// ridici bod ve 2D
struct S_Point
{
	double x, y;      // souradnice bodu
	double weight;    // vaha bodu
    S_Point() : x(0.0), y(0.0), weight(1.0) {}
    S_Point(double _x, double _y) : x(_x), y(_y), weight(1.0) {}

	const S_Point operator*(const double &other) const { S_Point result = *this; result.x *= other; result.y *= other; return result; }
	const S_Point operator/(const double &other) const { S_Point result = *this; result.x /= other; result.y /= other; return result; }
	S_Point &operator+=(const S_Point &other) { this->x += other.x; this->y += other.y; return *this; }
};

struct S_Triangle
{
   S_Point A;
   S_Point B;
   S_Point C;   
   S_Triangle() : A(), B(), C() {}   
};

// vektor nekolika bodu
typedef vector<S_Point>   T_PointVector;
// vektor KNOT, uzlovy vektor
typedef vector<double>  K_NOT;
//
typedef vector<S_Triangle> T_TriangleVector;



///////////////////////////////////////////////////////////////////////////////
// globalni promenne

extern S_RGBA             * frame_buffer;          // ukazatel na pamet RGB hodnot vlastniho frame bufferu
extern int                frame_w, frame_h;        // velikost frame bufferu


////////////////////////////////////////////////////////////////////////////////////////////////
// globalni promenne domaciho ukolu

// ridici body krivky
extern T_PointVector      control_points;

// vypoctene body krivky
extern T_PointVector      line_points;

//
extern  T_TriangleVector  triangle_points;


//Predefinovane barvy pouzite v programu
extern const S_RGBA       BLUE;
extern const S_RGBA       GREEN;
extern const S_RGBA       RED;
extern const S_RGBA       GREY;

// velikost krizku ilustrujiciho bod
extern const int          CROSS_SIZE;


///////////////////////////////////////////////////////////////////////////////
// uzitecna makra

// prohodi hodnoty dvou promennych
#define SWAP(a, b)    { (b) ^= (a); (a) ^= (b); (b) ^= (a); }

// zaokrouhli hodnotu typu double na int
#define ROUND(x)      ((int)((x) + 0.5))

// test na nulu, u floating point datovych typu je test na rovnost nespolehlivy
#define EQUAL_ZERO( val ) ( (val) < 0.00001 && (val) > -0.00001 )


///////////////////////////////////////////////////////////////////////////////
// funkce

// funkce pro nastaveni a zjisteni barvy bodu
void PutPixel(int x, int y, const S_RGBA & color);
void GetPixel(int x, int y, S_RGBA & color);

// ulozeni obsahu obrazovky do souboru ve formatu BMP (save.bmp)
int SaveBitmap(const char * filename, S_RGBA * frame_buffer, int Width, int Height);

// funkce pro vykresleni krizku na danych souradnicich
void DrawCrossPoint(int x, int y, int size, const S_RGBA & color);

// funkce vykresli krizky na souradnicich zadanych jako vektor bodu
void DrawCrossPoints(const T_PointVector & points, int size, const S_RGBA & color);

// vykresleni usecky danych souradnic a barvy do frame bufferu
void DrawLine(int x1, int y1, int x2, int y2, const S_RGBA & color);

// vykresleni usecky mezi vsemi body daneho vektoru
void DrawLines(const T_PointVector & points, const S_RGBA & color);

void DrawTriangles( const T_TriangleVector& triangle_vector , const S_RGBA & odd_color , const S_RGBA & even_color );

// projde zadany vektor bodu a zjisti, ktery je pobliz dane souradnice
int PointSnap(int x, int y, int range, const T_PointVector & points);

// Struktura popisujici atributy NURBS/B-spline krivky
// struktura je plnena automaticky v GUI behem prace,
// strukturu pouzivejte pouze ke cteni, jinak nelze zajistit korektni chovani aplikace
typedef struct BSplineDescription {
  int CurveDegree; //Stupen krivky
  int CountDrivingPoints;//Pocet bodu ridiciho polygonu
  int n; // n+1 = poctu bodu ridiciho polygonu
  int k; // stupen krivky
  int m; // m + 1 = delka uzloveho vektoru
  K_NOT U; // hodnoty uzloveho vektoru
  T_PointVector drawn_knot; // body ktere se pouziji pro grficke znazorneni knot vektoru
  
  // konstanty pouzite pro vypocet bodu grafickeho znazorneni knot vektoru
  const double scale_x; 
  const double scale_y;  

  // metoda vygeneruje unformni kont vektor podle zadanych parametru krivky,
  // je volana automaticky pokud dojde v programu ke zmene stupne krivky nebo poctu bodu ridiciho polygonu
  void UniformKnot()
  {
    int i; // promenna iteraci
    double step = 0.0; // konstantni krok knot vektoru
    double div = ((double)(m-2*k));

    // Vymazat stare hodnoty vektoru
    U.clear();
    //Vypocist krok
    step = div == 0.0 ? 0.0 : 1.0/div;

    // prvnich k+1 prvku vektoru je rovno 0
    for ( i = 0; i < k + 1; i++ ) {           
      U.push_back( double(0.0) );
    }
    // zbyvajici cleny vektoru vyplnime konstantim krokem
    for ( i = k + 1; i < m - k; i++ ) {
      U.push_back(double((double)(i-(k + 1)+1)*step) );
    }
    // poslednich k+1 prvku vektoru je rovno 1
    for ( i = m - k; i < m + 1; i++ ) {
      U.push_back( double(1.0) );
    }
  }
  // C++ konstruktor
  BSplineDescription( int _cdp , int _cd ) 
      : CurveDegree(_cd), CountDrivingPoints(_cdp) , n(_cdp-1), scale_x(25.0),scale_y(50.0)
  {
    // nasteveni parametru krivky podle zadanych hodnot
    CurveDegree = CurveDegree < 1 ? 1 : CurveDegree;
    k = CurveDegree > _cdp - 1 ? _cdp - 1 : CurveDegree;
    CurveDegree = k;
    m = n + k + 1;
  }
  // Nastaveni stupne krivky
  void SetCurveDegree( int _cd ) {
    _cd = _cd < 1 ? 1 : _cd;
    if ( _cd != CurveDegree ) { // Pokud doslo ke zmene stupne krivky je treba jeji nastavit parametry
      CurveDegree = _cd;
      k = CurveDegree > CountDrivingPoints - 1 ? CountDrivingPoints - 1 : CurveDegree;
      m = n + k + 1;
      // vygenerovat novy uniformni knot vektor
      UniformKnot();
    }
  }

  // Nastaveni poctu bodu ridiciho polygonu
  void SetCountDrivingPoints( int _cdp ) {
    if ( _cdp != CountDrivingPoints ) {// Pokud doslo ke zmene ridiciho polygonu je treba nastavit jeji parametry
      CountDrivingPoints = _cdp;
      n = _cdp-1;
      k = CurveDegree > CountDrivingPoints - 1 ? CountDrivingPoints - 1 : CurveDegree;
      m = n + k + 1;
      // vygenerovat novy uniformni knot vektor
      UniformKnot();
    }
  }
  // metoda vykresly knot vektorm ktery lze ovladat z GUI
  void DrawKnot( T_PointVector & knot_points) {
    // pocatecni bod od ktereho se zacne kreslit
    S_Point start_point( 50.0 , 10.0 );
    int i;

    knot_points.clear();
    drawn_knot.clear();
    
    //ridici cars
    DrawLine( ROUND(start_point.x) , ROUND(start_point.y) , ROUND(((double)((int)U.size()-1))*scale_x+start_point.x) , ROUND(start_point.y), BLUE );
    start_point.y += (double)CROSS_SIZE;
  
    // zelene vykreslime hodnoty vektoru ktere by meli byt nulove
    for ( i = 0; i < k + 1; i++ ) {           
        DrawCrossPoint(ROUND(((double)(i))*scale_x+start_point.x), ROUND(start_point.y + (U[i]*scale_y)), CROSS_SIZE , GREEN );
        knot_points.push_back( S_Point(((double)(i))*scale_x+start_point.x , start_point.y + (U[i]*scale_y)) );
    }
    // ostatni vekory
    for ( i = k + 1; i < m - k; i++ ) {
        DrawCrossPoint(ROUND(((double)(i))*scale_x+start_point.x), ROUND(start_point.y + (U[i]*scale_y)), CROSS_SIZE , RED );
        knot_points.push_back( S_Point(((double)(i))*scale_x+start_point.x , start_point.y + (U[i]*scale_y)) );
    }
    // zelene vykreslime hodnoty vektoru ktere by meli byt jedna
    for ( i = m - k; i < m + 1; i++ ) {
        DrawCrossPoint(ROUND(((double)(i))*scale_x+start_point.x), ROUND(start_point.y + (U[i]*scale_y)), CROSS_SIZE , GREEN );
        knot_points.push_back( S_Point(((double)(i))*scale_x+start_point.x , start_point.y + (U[i]*scale_y)) );
    }
    drawn_knot = knot_points;

  }
  //Metoda volana pri zmenen knot bektoru
  void OnKnotChange( int index , double x , double y ) {
      double dy; // o kolik se mys posunula ve vertikalnim smeru
      // posledni a prvni bod vektoru je stabilni, je tedy mozne hybat jen body mezi
      //if ( index < 1 || index > (int)U.size() - 2) {
      //    return;
      //}
      //prvnich k+1 a poslednich k+1 bodu ridiciho vektoru je stabilnich
      if ( index < this->CurveDegree + 1 || index > (this->m + 1) - this->CurveDegree - 2 )
      {
         return;
      }
      //prevedeni do skaly 0..1
      dy = (drawn_knot[index].y - y)/scale_y;
      //pohyb hidnoty uzlu vektoru
      U[index] -= dy;
      if (U[index] < U[index-1]) { // hodnota musi byt vetsi nebo rovna predchozi
          U[index] = U[index-1];
      }

      if (U[index] > U[index+1]) { // hodnota musi byt mensi nebo rovna predchozi
          U[index] = U[index+1];
      }
  }
private:
  BSplineDescription& operator = (const  BSplineDescription &bsdesc ) {throw; };
  

} BSplineDescription;

// Struktura popisujici vybrany bod
typedef struct ActivePoint {
    // vyctovy typ, popisujici na ktere komponente doslo k vyberu
    enum ComponentType {
        NO_COMPONENT    = 0x0, // zadny bod neni vybrany
        CONTROL_POLYGON = 0x1, // je vybrany bod ridiciho polygonu
        KNOT_VECTOR     = 0x2,  // je vybrany bod uzloveho vektoru, plati pouze pro NURBS
        CONTROL_VECTOR	= 0x3	// je vybran bod ridiciho vektoru, plati pro Fergusson
    };

    int active_point; // index aktivniho bodu
    ComponentType comp_type; // ktera komponenta je aktivni

    // C++ konstruktor struktury
    ActivePoint( int _ap = -1 , ComponentType _cp = NO_COMPONENT )
        : active_point( _ap ), comp_type( _cp )
    {};
} ActivePoint;


///////////////////////////////////////////////////////////////////////////////
// UDELAT VE CVICENI
void    Fergusson(int quality, const T_PointVector & points, const T_PointVector & vectors, T_PointVector & line_points);
void    Bezier(int quality, const T_PointVector & points, T_PointVector & line_points);
#endif // MAIN_H

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
