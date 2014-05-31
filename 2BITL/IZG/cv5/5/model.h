///////////////////////////////////////////////////////////////////////////////
// Simple polygonal model
///////////////////////////////////////////////////////////////////////////////

// include guard
#ifndef MODEL_H
#define MODEL_H

///////////////////////////////////////////////////////////////////////////////
// include

#include <vector>
#include <cmath>


///////////////////////////////////////////////////////////////////////////////
// types definition

// vertex/vector coordinate in 3D
struct S_Coords
{
  double x, y, z;   // vertex coordinate

  S_Coords() : x(0.0), y(0.0), z(0.0) {}
  S_Coords(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}

  // vector normalization (unit length)
  void normalize()
  {
    double length = x * x + y * y + z * z;
    if( length > 0.0 )
    {
      double norm = 1.0 / sqrt(length);
      x *= norm;
      y *= norm;
      z *= norm;
    }
  }
};

// vector (dynamic array) of vertices/vectors
typedef std::vector<S_Coords>       T_Coords;

// triangle
struct S_Triangle
{
  int v[3];         // vertices index
  int n;            // normal index

  S_Triangle() { v[0] = v[1] = v[2] = n = -1; }
  S_Triangle(int _v0, int _v1, int _v2, int _n) { v[0] = _v0; v[1] = _v1; v[2] = _v2; n = _n; }
};

// vector of triangles
typedef std::vector<S_Triangle>     T_Triangles;


///////////////////////////////////////////////////////////////////////////////
// global variables

// vertices, polygons and normals of figure
extern T_Coords           vertices;
extern T_Coords           normals;
extern T_Triangles        triangles;


///////////////////////////////////////////////////////////////////////////////
// functions 

// function loads model from given file and precalculates normals
bool LoadModel(const char *filename);


#endif // MODEL_H

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
