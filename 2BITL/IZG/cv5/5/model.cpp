///////////////////////////////////////////////////////////////////////////////
// Simple polygonal model
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// include

#include "model.h"

#include <fstream>
#include <cstring>
#include <cmath>


///////////////////////////////////////////////////////////////////////////////
// vertices, polygons and normals of figure

T_Coords           vertices;
T_Coords           normals;
T_Triangles        triangles;


///////////////////////////////////////////////////////////////////////////////
// function loads model from given file and precalculates normals

bool LoadModel(const char *filename)
{
  // open input file
  std::ifstream stream(filename, std::ios_base::in);
  if (!stream.is_open())
    return false;

  // discard previous model
  vertices.clear();
  triangles.clear();

  // load number of vertices
  int num_of_vertices = 0;
  stream >> num_of_vertices;

  // load vertices
  for( int i = 0; i < num_of_vertices; ++i )
  {
    double x, y, z;
    stream >> x >> y >> z;
    vertices.push_back(S_Coords(x, y, z));
  }

  // load number of triangles
  int num_of_triangles = 0;
  stream >> num_of_triangles;

  // load triangles
  for( int j = 0; j < num_of_triangles; ++j )
  {
    int v0, v1, v2;
    stream >> v0 >> v1 >> v2;
    if (v0 < 0 || v0 >= num_of_vertices
        || v1 < 0 || v1 >= num_of_vertices
        || v2 < 0 || v2 >= num_of_vertices)
      return false;
    triangles.push_back(S_Triangle(v0, v1, v2, j));

    // vector v0v1
    double ax = vertices[v1].x - vertices[v0].x;
    double ay = vertices[v1].y - vertices[v0].y;
    double az = vertices[v1].z - vertices[v0].z;

    // vector v0v2
    double bx = vertices[v2].x - vertices[v0].x;
    double by = vertices[v2].y - vertices[v0].y;
    double bz = vertices[v2].z - vertices[v0].z;

    // triangle normal a x b
    S_Coords n(ay * bz - az * by, az * bx - ax * bz, ax * by - ay * bx);
    n.normalize();
    normals.push_back(n);
  }

  // everything OK?
  return !stream.fail();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
