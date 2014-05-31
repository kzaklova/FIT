////////////////////////////////////////////////////////////////////////////////
// definice typu


// struktura typu RGBA barvy, 32 bit

struct S_RGBA { unsigned char red;
                unsigned char green;
                unsigned char blue;
                unsigned char alpha;  };

extern S_RGBA  * frame_buffer;          // ukazatel na pamet RGB hodnot vlastniho frame bufferu
extern int       frame_w, frame_h;      // velikost frame bufferu

               
////////////////////////////////////////////////////////////////////////////////
// uzitecna makra

// prohodi hodnoty dvou celociselnych promennych
#define SWAP(a, b)          { (b) ^= (a); (a) ^= (b); (b) ^= (a); }
#define SWAP1(a, b, w)      { (w) = (a); (a) = (b); (b) = (w); }

// zaokrouhli hodnotu typu double na int
#define ROUND(x)            (int((x) + 0.5))

// minimum ze dvou zadanych hodnot
#define MIN(a, b)           (((a) < (b)) ? (a) : (b))

// maximum ze dvou zadanych hodnot
#define MAX(a, b)           (((a) > (b)) ? (a) : (b))

////////////////////////////////////////////////////////////////////////////////
// konstanty barev pouzivanych v programu

const S_RGBA   COLOR_BLUE  = {0, 0, 255, 255};
const S_RGBA   COLOR_GREEN = {0, 255, 0, 255};
const S_RGBA   COLOR_RED   = {255, 0, 0, 255};
const S_RGBA   COLOR_GREY  = {190, 190, 190, 255};
const S_RGBA   COLOR_BLACK = {0, 0, 0, 255};
const S_RGBA   COLOR_WHITE = {255, 255, 255, 255};

////////////////////////////////////////////////////////////////////////////////
// hlavicky funkci

void PutPixel(int x, int y, S_RGBA color);
void GetPixel(int x, int y, S_RGBA & color);

void DrawLine (int x1, int y1, int x2, int y2);
void DrawCircle (int s1, int s2, int R);

////////////////////////////////////////////////////////////////////////////////
// konec souboru main.h
////////////////////////////////////////////////////////////////////////////////
