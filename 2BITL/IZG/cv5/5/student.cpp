///////////////////////////////////////////////////////////////////////////////
// File containing student functions
///////////////////////////////////////////////////////////////////////////////

#include "main.h"
#include "transform.h"
#include "model.h"

#include <cstdio>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// name spaces

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// function for drawing projected line, etc.
///////////////////////////////////////////////////////////////////////////////

// draw line defined in 3D
// applies transform matrix "model" and consequently "projection"
// a - first vertex in 3D
// b - second vertex
// color - drawing color
void ProjectLine(const S_Coords& a,
                 const S_Coords& b,
                 const S_RGBA &color)
{
  // line is projected by projection of its end points
  // first pally trsnforms of object and scene
  S_Coords aa, bb;
  trTransformVertex(aa, a);
  trTransformVertex(bb, b);

  // subsequently project to screen
  int u1,v1,u2,v2;
  trProjectVertex(u1, v1, aa);
  trProjectVertex(u2, v2, bb);

  DrawLine(u1, v1, u2, v2, aa.z, bb.z, color);
}

// draw triangle defined in 3D
// a,b,c - vertices
// n - normal
// material - material diffuse parameters of lightning model
void ProjectTriangle(const S_Coords& a,
                     const S_Coords& b,
                     const S_Coords& c,
                     const S_Coords& n,
                     const S_Material &material)
{
  // transform vertices with model matrix (object/scene manipulation)
  S_Coords aa, bb, cc;
  trTransformVertex(aa, a);
  trTransformVertex(bb, b);
  trTransformVertex(cc, c);

  // project triangle vertices to display
  int u1, v1, u2, v2, u3, v3;
  trProjectVertex(u1, v1, aa);
  trProjectVertex(u2, v2, bb);
  trProjectVertex(u3, v3, cc);

  // transform normal
  S_Coords nn;
  trTransformVector(nn, n);
  nn.normalize();

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // PLACE FOR STUDENTS WORK - LABORATORIES
  // - complete back-faced triangle test
  S_Coords s(0 - aa.x, 0 - aa.y, -CAMERA_DIST - aa.z);
  double sk = s.x*nn.x + s.y*nn.y + s.z*nn.z;
  if (sk < 0) {
	return;
  }
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  // recompute color
  S_RGBA color;
  color.red = ROUND2BYTE(255 * material.red);
  color.green = ROUND2BYTE(255 * material.green);
  color.blue = ROUND2BYTE(255 * material.blue);

  // draw as wire model
  DrawLine(u1, v1, u2, v2, aa.z, bb.z, color);
  DrawLine(u2, v2, u3, v3, bb.z, cc.z, color);
  DrawLine(u3, v3, u1, v1, cc.z, aa.z, color);
  
}

///////////////////////////////////////////////////////////////////////////////
// ProjectObject function
// - draw object transformed with active transform matrix

void ProjectObject(const S_Material &material)
{
  T_Triangles::iterator end = triangles.end();
  for( T_Triangles::iterator it = triangles.begin(); it != end; ++it )
  {
    ProjectTriangle(vertices[it->v[0]],
                    vertices[it->v[1]],
                    vertices[it->v[2]],
                    normals[it->n],
                    material);
  }
}

///////////////////////////////////////////////////////////////////////////////
// function DrawScene() invoked from main.cpp
// - draw whole scene including placed objects

void DrawScene()
{
  // clear frame buffer
  ClearBuffers();

  // set projection matrix
  trProjectionPerspective(CAMERA_DIST, frame_w, frame_h);

  // initialize model matrix
  trLoadIdentity();

  // translate whole scene
  trTranslate(0.0, 0.0, scene_move_z);

  // rotate whole scene - in two axis only - mouse is 2D device only... :(
  trRotateX(scene_rot_x * 0.01);
  trRotateY(scene_rot_y * 0.01);
  
  // draw "mirror" with blue color
  ProjectLine(S_Coords(0,-2,-2), S_Coords(0, 2,-2), COLOR_BLUE);
  ProjectLine(S_Coords(0, 2,-2), S_Coords(0, 2, 2), COLOR_BLUE);
  ProjectLine(S_Coords(0, 2, 2), S_Coords(0,-2, 2), COLOR_BLUE);
  ProjectLine(S_Coords(0,-2, 2), S_Coords(0,-2,-2), COLOR_BLUE);

  // draw object with red color
  // add translation and rotation of object
  Matrix x = trGetMatrix();

  trScale(-1,1,1);
  trTranslate(obj_move_x * 0.015, obj_move_y * 0.015, 0.0);
  trRotateX(obj_rot_x * 0.01);
  trRotateY(obj_rot_y * 0.01);

  // now project and draw the object
  ProjectObject(MAT_RED);

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // PLACE FOR STUDENTS WORK - LABORATORIES
  // - complete drawing of second object symmetrically by the blue "mirror"
  // - use material MAT_RED2
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  
  trSetMatrix(x);

  trTranslate(obj_move_x * 0.015, obj_move_y * 0.015, 0.0);
  trRotateX(obj_rot_x * 0.01);
  trRotateY(obj_rot_y * 0.01);

  ProjectObject(MAT_RED2);  
}
///////////////////////////////////////////////////////////////////////////////
// File containing student functions
///////////////////////////////////////////////////////////////////////////////

#include "main.h"
#include "transform.h"
#include "model.h"

#include <cstdio>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// name spaces

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// function for drawing projected line, etc.
///////////////////////////////////////////////////////////////////////////////

// draw line defined in 3D
// applies transform matrix "model" and consequently "projection"
// a - first vertex in 3D
// b - second vertex
// color - drawing color
void ProjectLine(const S_Coords& a,
                 const S_Coords& b,
                 const S_RGBA &color)
{
  // line is projected by projection of its end points
  // first pally trsnforms of object and scene
  S_Coords aa, bb;
  trTransformVertex(aa, a);
  trTransformVertex(bb, b);

  // subsequently project to screen
  int u1,v1,u2,v2;
  trProjectVertex(u1, v1, aa);
  trProjectVertex(u2, v2, bb);

  DrawLine(u1, v1, u2, v2, aa.z, bb.z, color);
}

// draw triangle defined in 3D
// a,b,c - vertices
// n - normal
// material - material diffuse parameters of lightning model
void ProjectTriangle(const S_Coords& a,
                     const S_Coords& b,
                     const S_Coords& c,
                     const S_Coords& n,
                     const S_Material &material)
{
  // transform vertices with model matrix (object/scene manipulation)
  S_Coords aa, bb, cc;
  trTransformVertex(aa, a);
  trTransformVertex(bb, b);
  trTransformVertex(cc, c);

  // project triangle vertices to display
  int u1, v1, u2, v2, u3, v3;
  trProjectVertex(u1, v1, aa);
  trProjectVertex(u2, v2, bb);
  trProjectVertex(u3, v3, cc);

  // transform normal
  S_Coords nn;
  trTransformVector(nn, n);
  nn.normalize();

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // PLACE FOR STUDENTS WORK - LABORATORIES
  // - complete back-faced triangle test
  S_Coords t(0 - aa.x, 0 - aa.y, -CAMERA_DIST - aa.z);
  double sk = t.x*nn.x + t.y*nn.y + t.z*nn.z;
  if (sk < 0) {
	return;
  }
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  // recompute color
  S_RGBA color;
  color.red = ROUND2BYTE(255 * material.red);
  color.green = ROUND2BYTE(255 * material.green);
  color.blue = ROUND2BYTE(255 * material.blue);

  // draw as wire model
  DrawLine(u1, v1, u2, v2, aa.z, bb.z, color);
  DrawLine(u2, v2, u3, v3, bb.z, cc.z, color);
  DrawLine(u3, v3, u1, v1, cc.z, aa.z, color);
  
}

///////////////////////////////////////////////////////////////////////////////
// ProjectObject function
// - draw object transformed with active transform matrix

void ProjectObject(const S_Material &material)
{
  T_Triangles::iterator end = triangles.end();
  for( T_Triangles::iterator it = triangles.begin(); it != end; ++it )
  {
    ProjectTriangle(vertices[it->v[0]],
                    vertices[it->v[1]],
                    vertices[it->v[2]],
                    normals[it->n],
                    material);
  }
}

///////////////////////////////////////////////////////////////////////////////
// function DrawScene() invoked from main.cpp
// - draw whole scene including placed objects

void DrawScene()
{
  // clear frame buffer
  ClearBuffers();

  // set projection matrix
  trProjectionPerspective(CAMERA_DIST, frame_w, frame_h);

  // initialize model matrix
  trLoadIdentity();

  // translate whole scene
  trTranslate(0.0, 0.0, scene_move_z);

  // rotate whole scene - in two axis only - mouse is 2D device only... :(
  trRotateX(scene_rot_x * 0.01);
  trRotateY(scene_rot_y * 0.01);
  
  // draw "mirror" with blue color
  ProjectLine(S_Coords(0,-2,-2), S_Coords(0, 2,-2), COLOR_BLUE);
  ProjectLine(S_Coords(0, 2,-2), S_Coords(0, 2, 2), COLOR_BLUE);
  ProjectLine(S_Coords(0, 2, 2), S_Coords(0,-2, 2), COLOR_BLUE);
  ProjectLine(S_Coords(0,-2, 2), S_Coords(0,-2,-2), COLOR_BLUE);

  // draw object with red color
  // add translation and rotation of object
  Matrix x = trGetMatrix();

  trScale(-1,1,1);
  trTranslate(obj_move_x * 0.015, obj_move_y * 0.015, 0.0);
  trRotateX(obj_rot_x * 0.01);
  trRotateY(obj_rot_y * 0.01);

  // now project and draw the object
  ProjectObject(MAT_RED);

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // PLACE FOR STUDENTS WORK - LABORATORIES
  // - complete drawing of second object symmetrically by the blue "mirror"
  // - use material MAT_RED2
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  
  trSetMatrix(x);

  trTranslate(obj_move_x * 0.015, obj_move_y * 0.015, 0.0);
  trRotateX(obj_rot_x * 0.01);
  trRotateY(obj_rot_y * 0.01);

  ProjectObject(MAT_RED2);  
}
