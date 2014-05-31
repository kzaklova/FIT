///////////////////////////////////////////////////////////////////////////////
// Implementation of operations with matrix
///////////////////////////////////////////////////////////////////////////////

#include "transform.h"


///////////////////////////////////////////////////////////////////////////////
// global variables

// main transformation matrix (object/scene manipulation in 3D)
Matrix model;

// separate projection matrix
Matrix projection;

//actual window size
int frame_height = 0, frame_width = 0;


///////////////////////////////////////////////////////////////////////////////
// operation with "model" matrix

// set model matrix to given value
void trSetMatrix(const Matrix &m)
{
  model = m;
}

// returns actual model matrix
const Matrix &trGetMatrix()
{
  return model;
}

// reset model matrix to identity
void trLoadIdentity()
{
  model.LoadIdentity();
}

// composition of transformations (given matrix is multiplied by actual model matrix)
// Rem.: Implementation of function is in file student.cpp!
void trMultMatrix(Matrix &m)
{
  // copy original matrix
  Matrix temp = model;
  
  // multiplication of matrices
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      model[i][j] = 0.0;
      for (int k = 0; k < 4; ++k) {
        model[i][j] += m[i][k] * temp[k][j];
      }
    }
  }
}

// multiply model matrix by translation matrix (object is moved for given distances)
void trTranslate(double dx, double dy, double dz)
{
  Matrix m;

  m[3][0] = dx;
  m[3][1] = dy;
  m[3][2] = dz;

  trMultMatrix(m);
}

// multiply model matrix by rotation matrix along X axis
void trRotateY(double angle)
{
  Matrix m;

  // reduction of computation of goniometric functions (only once)
  double sina = sin(angle);
  double cosa = cos(angle);

  m[0][0] = cosa;
  m[0][2] = sina;
  m[2][0] = -sina;
  m[2][2] = cosa;

  trMultMatrix(m);
}

// multiply model matrix by rotation matrix along X axis
void trRotateX(double angle)
{
  Matrix m;
  
  // reduction of computation of goniometric functions (only once)
  double sina = sin(angle);
  double cosa = cos(angle);
  
  m[1][1] = cosa;
  m[1][2] = sina;
  m[2][1] = -sina;
  m[2][2] = cosa;

  trMultMatrix(m);
}


// multiply model matrix by scale matrix
void trScale(double sx, double sy, double sz)
{
  Matrix m;
  
  m[0][0] = sx;
  m[1][1] = sy;
  m[2][2] = sz;

  trMultMatrix(m);
}

///////////////////////////////////////////////////////////////////////////////
// operations with projection matrix

// set projection matrix performs perspective projection
// d > 0 - camera distance in z axis
// width, height - actual window size (frame buffer)
void trProjectionPerspective(double d, int width, int height)
{
  projection.LoadIdentity();

  projection[2][2] = 0.0f;
  projection[2][3] = 1.0f / d;

  // store window size...
  frame_width = width;
  frame_height = height;
}

///////////////////////////////////////////////////////////////////////////////
// functions that apply set transformations

// transform point in 3D using model matrix
// p - original coordinates in 3D
// p2 - new coordinates
void trTransformVertex(S_Coords& p2, const S_Coords& p)
{
  // multiplication of vector (x,y,z,1) and transformation matrix
  p2.x = p.x * model[0][0] + p.y * model[1][0] + p.z * model[2][0] + model[3][0];
  p2.y = p.x * model[0][1] + p.y * model[1][1] + p.z * model[2][1] + model[3][1];
  p2.z = p.x * model[0][2] + p.y * model[1][2] + p.z * model[2][2] + model[3][2];
}

// transform vector in 3D using model matrix
void trTransformVector(S_Coords& v2, const S_Coords& v)
{
  // multiplication of vector (x,y,z,1) and transformation matrix
  v2.x = v.x * model[0][0] + v.y * model[1][0] + v.z * model[2][0];
  v2.y = v.x * model[0][1] + v.y * model[1][1] + v.z * model[2][1];
  v2.z = v.x * model[0][2] + v.y * model[1][2] + v.z * model[2][2];
}

// project point in 3D to screen using projection matrix
// p - original coordinate in 3D
// u,v -  screen coordinates
void trProjectVertex(int &u, int &v, const S_Coords& p)
{
  // multiplication of vector (x,y,z,1) and projection matrix
  double xx = p.x;
  double yy = p.y;
  double ww = p.x * projection[0][3] + p.y * projection[1][3] + p.z * projection[2][3] + projection[3][3];

  // homogeneous coordinates
  xx /= ww;
  yy /= ww;

  // projected coordinates rounding ???
  u = int(xx + frame_width / 2);
  v = int(yy + frame_height / 2);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
