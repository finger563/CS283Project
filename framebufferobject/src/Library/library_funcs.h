#ifndef LIBRARY_FUNCS_H
#define LIBRARY_FUNCS_H

#include "../main.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stack>

using std::stringstream;
using std::cout;
using std::cin;
using std::endl;
using std::ends;

// in order to get function prototypes from glext.h, define GL_GLEXT_PROTOTYPES before including glext.h
#define GL_GLEXT_PROTOTYPES

// include GLEW to access OpenGL functions
#include <GL/glew.h>

const GLenum PIXEL_FORMAT = GL_BGRA;

// GLUT is the toolkit to interface with the OS
#include <GL/freeglut.h>

#include "glext.h"
#include "glInfo.h"                             // glInfo struct
#include "Timer.h"


// GLUT CALLBACK functions ////////////////////////////////////////////////////
void displayCB();
void reshapeCB(int w, int h);
void timerCB(int millisec);
void idleCB();
void keyboardCB(unsigned char key, int x, int y);
void specialKeyCB(int key, int x, int y);
void mouseCB(int button, int stat, int x, int y);
void mouseMotionCB(int x, int y);

// CALLBACK function when exit() called ///////////////////////////////////////
void exitCB();

void initGL();
int  initGLUT(int argc, char **argv);
bool initSharedMem();
void clearSharedMem();
void initLights();
void updatePixels(GLubyte* dst, int size);
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
void drawString(const char *str, int x, int y, float color[4], void *font);
void drawString3D(const char *str, float pos[3], float color[4], void *font);
void showInfo();
void showTransferRate();
void printTransferRate();
void toPerspective();

// function pointers for PBO Extension
// Windows needs to get function pointers from ICD OpenGL drivers,
// because opengl32.dll does not support extensions higher than v1.1.
#ifdef _WIN32
extern PFNGLGENBUFFERSARBPROC pglGenBuffersARB;                     // VBO Name Generation Procedure
extern PFNGLBINDBUFFERARBPROC pglBindBufferARB;                     // VBO Bind Procedure
extern PFNGLBUFFERDATAARBPROC pglBufferDataARB;                     // VBO Data Loading Procedure
extern PFNGLBUFFERSUBDATAARBPROC pglBufferSubDataARB;               // VBO Sub Data Loading Procedure
extern PFNGLDELETEBUFFERSARBPROC pglDeleteBuffersARB;               // VBO Deletion Procedure
extern PFNGLGETBUFFERPARAMETERIVARBPROC pglGetBufferParameterivARB; // return various parameters of VBO
extern PFNGLMAPBUFFERARBPROC pglMapBufferARB;                       // map VBO procedure
extern PFNGLUNMAPBUFFERARBPROC pglUnmapBufferARB;                   // unmap VBO procedure
#define glGenBuffersARB           pglGenBuffersARB
#define glBindBufferARB           pglBindBufferARB
#define glBufferDataARB           pglBufferDataARB
#define glBufferSubDataARB        pglBufferSubDataARB
#define glDeleteBuffersARB        pglDeleteBuffersARB
#define glGetBufferParameterivARB pglGetBufferParameterivARB
#define glMapBufferARB            pglMapBufferARB
#define glUnmapBufferARB          pglUnmapBufferARB
#endif

// global variables
extern void *font;
extern GLuint pboIds[2];                   // IDs of PBO
extern GLuint textureId;                   // ID of texture
extern GLubyte* imageData;             // pointer to texture buffer
extern int screenWidth;
extern int screenHeight;
extern bool mouseLeftDown;
extern bool mouseRightDown;
extern float mouseX, mouseY;
extern float cameraAngleX;
extern float cameraAngleY;
extern float cameraDistance;
extern bool pboSupported;
extern int pboMode;
extern int drawMode;
extern Timer timer, t1, t2;
extern float copyTime, updateTime;

#endif