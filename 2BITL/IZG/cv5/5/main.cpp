////////////////////////////////////////////////////////////////////////////////////////////////
// Zaklady pocitacove grafiky
// Laboratory #5
//
// Author: Michal Spanel (remixed by Lukas Marsik)
//
// Laboratory outline: 3D Transformations
//   1) How does it work
//   2) Common task
//      - Discard back-faced triangles (aka back-face culling)
//   3) Classified task
//      - Draw second object symmetrically by the blue "mirror"
//
// *) Application control:
//      mouse         - rotation and zoom of whole scene
//      shift + mouse - object translation
//      ctrl + mouse  - object rotation along its center
//      "f"         - full screen mode
//      "w"         - window mode
//      "Esc", ...  - terminate application
//
// Corrections:
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// include

#ifdef _WIN32
  #include <windows.h>
#endif

#include "main.h"

#include <GL/gl.h>
#include <GL/glut.h>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////
// name spaces

using namespace std;


///////////////////////////////////////////////////////////////////////////////
// global constants

// main window title
const char         * PROGRAM_TITLE = "IZG Labs #05, 3D transformation";

// standard input model
const char         * DEFAULT_MODEL = "cylinder.tri";

// maximal scene proximity to camera
const int          MAX_SCENE_MOVE = -CAMERA_DIST + 10;

///////////////////////////////////////////////////////////////////////////////
// global variables

S_RGBA             * frame_buffer = NULL;     // frame buffer (RGB values) memory pointer
int                frame_w = 0, frame_h = 0;  // frame buffer size

const int          default_frame_w   = 500;   // default window size
const int          default_frame_h   = 500;

///////////////////////////////////////////////////////////////////////////////
// global variables shared among main.cpp and student.cpp

int                obj_rot_x    = 0;          // camera position
int                obj_rot_y    = 0;

int                obj_move_x   = -100;       // object translation
int                obj_move_y   = 0;

int                scene_rot_x  = 0;          // object rotation
int                scene_rot_y  = 0;

int                scene_move_z = -985;       // scene translation


///////////////////////////////////////////////////////////////////////////////
// frame buffer
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// function clears frame buffer

void ClearBuffers()
{
  // buffer exists?
  assert(frame_buffer);

  // make all pixels black
  memset(frame_buffer, 0, frame_w * frame_h * sizeof(S_RGBA));
}

///////////////////////////////////////////////////////////////////////////////
// line culling with Liang-Barsky algorithm right before rendering to frame buffer
// return false if whole line is outside of window

bool LiangBarskyLineCulling(int &x1, int &y1, int &x2, int &y2)
{
  // computer parameters dx, dy
  int dx = x2 - x1;
  int dy = y2 - y1;

  // computer parameters pi and qi
  int p[4], q[4];
  p[0] = -dx; q[0] = x1 - 1;
  p[1] =  dx; q[1] = frame_w - 2 - x1;
  p[2] = -dy; q[2] = y1 - 1;
  p[3] =  dy; q[3] = frame_h - 2 - y1;

  // set parameter ui to initial value
  double u1 = 0.0f;
  double u2 = 1.0f;

  // subsequently examine parameters pi a qi
  for( int i = 0; i < 4; ++i )
  {
    // computer parameter ri
    double ri = 0.0f;
    if (p[i] != 0)
      ri = double(q[i]) / double(p[i]);

    // is line aiming to window?
    if (p[i] < 0)
    {
      if (ri > u2)        // valid bounds check
        return false;
      else if (ri > u1)   // find new start point
        u1 = ri;
    }
    else if (p[i] > 0)      // line aims out of window
    {
      if (ri < u1)        // valid bounds check
        return false;
      else if (ri < u2)   // find new end point
        u2 = ri;
    }
    else if (q[i] < 0)      // line is out of window
    {
      return false;
    }
  }

  // computer parameters u1 and u2 at the end
  // compute final line coordinates x1,y1 and x2,y2
  if (u2 < 1)
  {
    double t1 = u2 * dx, t2 = u2 * dy;
    x2 = x1 + ROUND(t1);
    y2 = y1 + ROUND(t2);
  }
  if (u1 > 0)
  {
	double t1 = u1 * dx, t2 = u1 * dy;
	x1 = x1 + ROUND(t1);
	y1 = y1 + ROUND(t2);
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// rasterize line with given coordinates and color to frame buffer
// x1,y1,x2,y2 - line endpoints
// z1, z2 - line depth within scene
// color - drawing color

void DrawLine(int x1, int y1, int x2, int y2, double z1, double z2, const S_RGBA & color)
{
  // window culling
  if (!LiangBarskyLineCulling(x1, y1, x2, y2))
  {
    return;
  }

  // delta in x and y axis
  int dx = ABS(x2 - x1);
  int dy = ABS(y2 - y1);

  // x and y axis swap in rendering algorithm
  bool xy_swap = (dy > dx);
  if (xy_swap)
  {
    SWAP(x1, y1);
    SWAP(x2, y2);
    SWAP(dx, dy);
  }

  // appropriate point swap in x axis
  if (x1 > x2)
  {
    SWAP(x1, x2);
    SWAP(y1, y2);
  }

  // advance direction in y axis
  int step_y;
  if (y1 > y2) step_y = -1;
  else step_y = 1;

  // Bresenham algorithm constants
  int k1 = 2 * dy;
  int k2 = 2 * (dy - dx);
  int p  = 2 * dy - dx;

  // initial assignment of values
  int x = x1;
  int y = y1;

  // line rendering loop
  while (x <= x2)
  {
    if (!xy_swap)
    {
      PIXEL(x, y) = color;
    }
    else
    {
      PIXEL(y, x) = color;
    }

    x++;
    if (p < 0)
	{
      p += k1;
    }
    else
	{
      p += k2;
      y += step_y;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// function for initialization of application parameters

void onInit(void)
{
  // setup pixel storage format
  glPixelStorei(GL_UNPACK_ALIGNMENT,4);
  
  // background color setup
  glClearColor(0.0, 0.0, 0.0, 0.0);
}

///////////////////////////////////////////////////////////////////////////////
// functions handling window size change
// w, h - new window size

void onResize(int w, int h)
{
  // set visible region of OpenGL window to whole application window
  glViewport(0, 0, w, h);

  // set projection matrix modification
  glMatrixMode(GL_PROJECTION);

  // initialize projection matrix (set as identity matrix)
  glLoadIdentity();

  // set parallel projection to display content of OpenGL window
  glOrtho(0, w, 0, h, -1, 1);

  // allocation of new frame buffer memory
  S_RGBA *new_frame_buffer = new S_RGBA[w * h];
  assert(new_frame_buffer);

  // clear frame buffer
  memset(new_frame_buffer, 0, w * h * sizeof(S_RGBA));

  // eventual copy of previous frame buffer to the new one
  if( frame_buffer != NULL )
  {
    int       min_w;        // minimal frame buffer width
    int       min_h;        // minimal frame buffer height
    int       dh;           // difference of windows height, start of copy of new window
    int       frame_dh;     // difference of windows height, start of copy of new window

    // deetrmine minimal width and height of frame buffer
    min_w = (w < frame_w) ? w : frame_w;
    min_h = (h < frame_h) ? h : frame_h;

    // determine start of data to copy
    dh = (frame_h > h) ? 0 : (h - frame_h);
    frame_dh = (frame_h > h) ? (frame_h - h) : 0;

    // partial copy of frame buffer
    for( int i = 0; i < min_h; i++ )
	{
      memcpy((new_frame_buffer + ((i + dh) * w)), (frame_buffer + ((i + frame_dh) * frame_w)), min_w * sizeof(S_RGBA));
	}
  }

  // set new proportions frame buffer
  frame_w = w;
  frame_h = h;

  // delete eventual existing frame buffer memory
  if (frame_buffer != NULL)
  {
    delete[] frame_buffer;
  }

  // set new frame buffer as active
  frame_buffer = new_frame_buffer;
}

///////////////////////////////////////////////////////////////////////////////
// function handling redraw of window content

void onDisplay(void)
{
  // test frame buffer existence
  assert(frame_buffer != NULL);

  // clear bit planes of color buffer
  glClear(GL_COLOR_BUFFER_BIT);

  // Redraw scene!!!
  DrawScene();

  // set drawing to color RGBA OpenGL buffer
  glDrawBuffer(GL_COLOR);
  // set coordinates of left down corner of pixmap
  glRasterPos2i(0, 0);
  // draw pixmap
  glDrawPixels(frame_w, frame_h, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char *)frame_buffer);

  // rasterize, render implemented displaying commands
  glFlush();
  // swap displayed and rendered buffer 
  glutSwapBuffers();
}

///////////////////////////////////////////////////////////////////////////////
// function responsive to key press
// key - code of pressed key
// x, y - cursor coordinate when key pressed

void onKeyboard(unsigned char key, int x, int y)
{
   // previous window position parameters
  static int old_frame_h, old_frame_w, old_window_x, old_window_y;

  // convert upper case to lower case
  if (key >= 'A' && key <= 'Z')
    key += 'a' - 'A';

  // switch to pressed key
  switch (key)
  {
    // quit application
    case 27:                                // Escape key
    case 'q':
    case 'x':
      exit(0);
      break;

    // switch to full-screen mode
    case 'f':
      old_frame_h = frame_h;
      old_frame_w = frame_w;
      old_window_x = glutGet(GLUT_WINDOW_X);
      old_window_y = glutGet(GLUT_WINDOW_Y);
      glutFullScreen();
      break;

    // return from full-screen mode
    case 'w':
      // restore window size
      if( old_frame_w > 0 )
      {
        glutPositionWindow(old_window_x, old_window_y);
        glutReshapeWindow(old_frame_w, old_frame_h);
      }
      else
      { // default frame buffer dimensions
        glutReshapeWindow(default_frame_w, default_frame_h);
      }
      break;

	default:
      break;
  }

  // invoke OpenGL window redraw
  glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////////////////
// function responsive to mouse button press
// button - code of pressed button
// state - button state - pressed/released
// x, y - cursor coordinate when button pressed

int old_mouse_x = 0, old_mouse_y = 0;   // previous mouse position, used for computation of rotation
int modifiers = 0, stav = 0;            // state of mouse buttons

void onMouseClick(int button, int state, int x, int y)
{
  // remember mouse position
  old_mouse_x = x;
  old_mouse_y = y;

  if (button == GLUT_LEFT_BUTTON) {             // left button activates rotation
    stav = (state == GLUT_DOWN) ? 1 : 0;        // test if pressed
  }
  else if (button == GLUT_RIGHT_BUTTON) {       // right button activates translation 
    stav = (state == GLUT_DOWN) ? 2 : 0;        // test if pressed
  }
  else {
    stav = 0;
  }
  
  modifiers = glutGetModifiers();
}

///////////////////////////////////////////////////////////////////////////////
// function responsive to mouse move (without pressed buttons)
// x, y - current cursor position

void onMouseMotion(int x, int y)
{
  if (modifiers & GLUT_ACTIVE_CTRL) {
    // control key pressed -> object rotation
    obj_rot_y += x - old_mouse_x;
    obj_rot_x -= y - old_mouse_y;
  }
  else if (modifiers & GLUT_ACTIVE_SHIFT) {
    // shift key pressed -> object translation
    obj_move_x += x - old_mouse_x;
    obj_move_y += old_mouse_y - y;
  }
  else if (stav == 1) {
    // scene rotation
    scene_rot_y += x - old_mouse_x;
    scene_rot_x -= y - old_mouse_y;
  }
  else if (stav == 2) {
    // camera translation
    scene_move_z += old_mouse_y - y;
    if (scene_move_z < MAX_SCENE_MOVE) scene_move_z = MAX_SCENE_MOVE;
  }

  old_mouse_x = x;
  old_mouse_y = y;

  glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////////////////
// main application function
// argc - count of command line parameters
// argv - array of command line parameters

int main(int argc, char **argv)
{
  // load model
  const char *model = (argc == 2) ? argv[1] : DEFAULT_MODEL;
  if( !LoadModel(model) )
  {
    std::cerr << "Error: Failed to read input model!" << std::endl;
    exit(1);
  }

  // initialize GLUT library
  glutInit(&argc, argv);

  // set graphic mode of window
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
  // set initial dimensions of application window
  glutInitWindowSize(default_frame_w, default_frame_h);
  // create main application window
  glutCreateWindow(PROGRAM_TITLE);

  // initialize program parameters
  onInit();
  
  // force calling of onResize function - necessary for buffer initialization of buffers
  onResize(1,1);

  // register function handling redraw
  glutDisplayFunc(onDisplay);
  // register function responsive to window size change
  glutReshapeFunc(onResize);
  // register function responsive to key press
  glutKeyboardFunc(onKeyboard);
  // register function responsive to mouse button press
  glutMouseFunc(onMouseClick);
  // register function responsive to mouse move
  glutMotionFunc(onMouseMotion);

  // infinite loop calling registered functions
  // - it is no longer infinite in latest Freeglut implementation ;-)
  glutMainLoop();

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
