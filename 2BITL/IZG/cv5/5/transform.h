///////////////////////////////////////////////////////////////////////////////
// Transformation matrix handling
///////////////////////////////////////////////////////////////////////////////

// include guard
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "model.h"

#include <cmath>


///////////////////////////////////////////////////////////////////////////////
// trnsformation matrix

class Matrix
{
  double data[16];

public:
  // initialize matrix to identity
  Matrix() { LoadIdentity(); }
  
  // access to matrix "row"
  double *operator [] (int row) { return (data + row * 4); }

  // set matrix to identity
  void LoadIdentity()
  {
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 4; ++j)
        data[i*4 + j] = (i == j) ? 1.0 : 0.0;
  }
};

///////////////////////////////////////////////////////////////////////////////
// active matrix

// main transformation matrix (object/scene manipulation in 3D)
extern Matrix model;

// separate projection matrix
extern Matrix projection;

///////////////////////////////////////////////////////////////////////////////
// operation with "model" matrix

// set model matrix to given value
void trSetMatrix(const Matrix &m);

// returns actual model matrix
const Matrix &trGetMatrix();

// reset model matrix to identity
void trLoadIdentity();

// composition of transformations (given matrix is multiplied by actual model matrix)
// Rem.: Implementation of function is in file student.cpp!
void trMultMatrix(Matrix &m);

// multiply model matrix by translation matrix (object is moved for given distances)
void trTranslate(double dx, double dy, double dz);

// multiply model matrix by rotation matrix along Y axis
void trRotateY(double angle);

// multiply model matrix by rotation matrix along X axis
void trRotateX(double angle);

// multiply model matrix by scale matrix
void trScale(double dx, double dy, double dz);

///////////////////////////////////////////////////////////////////////////////
// operations with projection matrix

// set projection matrix performs perspective projection
// d > 0 - camera distance in z axis
// width, height - actual window size (frame buffer)
void trProjectionPerspective(double d, int width, int height);

///////////////////////////////////////////////////////////////////////////////
// functions that apply set transformations

// transform point in 3D using model matrix
// p - original coordinates in 3D
// p2 - new coordinates
void trTransformVertex(S_Coords& p2, const S_Coords& p);

// transform vector in 3D using model matrix
void trTransformVector(S_Coords& v2, const S_Coords& v);

// project point in 3D to screen using projection matrix
// p - original coordinates in 3D
// u,v -  screen coordinates
void trProjectVertex(int &u, int &v, const S_Coords& p);


#endif // TRANSFORM_H
