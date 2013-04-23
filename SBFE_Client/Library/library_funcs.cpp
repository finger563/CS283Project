
#include "library_funcs.h"


PFNGLGENBUFFERSARBPROC pglGenBuffersARB = 0;                     // VBO Name Generation Procedure
PFNGLBINDBUFFERARBPROC pglBindBufferARB = 0;                     // VBO Bind Procedure
PFNGLBUFFERDATAARBPROC pglBufferDataARB = 0;                     // VBO Data Loading Procedure
PFNGLBUFFERSUBDATAARBPROC pglBufferSubDataARB = 0;               // VBO Sub Data Loading Procedure
PFNGLDELETEBUFFERSARBPROC pglDeleteBuffersARB = 0;               // VBO Deletion Procedure
PFNGLGETBUFFERPARAMETERIVARBPROC pglGetBufferParameterivARB = 0; // return various parameters of VBO
PFNGLMAPBUFFERARBPROC pglMapBufferARB = 0;                       // map VBO procedure
PFNGLUNMAPBUFFERARBPROC pglUnmapBufferARB = 0;                   // unmap VBO procedure

// global variables
void *font = GLUT_BITMAP_8_BY_13;
GLuint pboIds[2];                   // IDs of PBO
GLuint textureId;                   // ID of texture
GLubyte* imageData = 0;             // pointer to texture buffer
int screenWidth;
int screenHeight;
float cameraAngleX;
float cameraAngleY;
float cameraDistance;
bool pboSupported;
int pboMode;
int drawMode = 0;
Timer timer, t1, t2;
float copyTime, updateTime;


///////////////////////////////////////////////////////////////////////////////
// initialize GLUT for windowing
///////////////////////////////////////////////////////////////////////////////
int initGLUT(int argc, char **argv) {
    // GLUT stuff for windowing
    // initialization openGL window.
    // it is called before any other GLUT routine
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_ALPHA); // display mode

    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);               // window size

    glutInitWindowPosition(100, 100);           // window location

    // finally, create a window with openGL context
    // Window will not displayed until glutMainLoop() is called
    // it returns a unique ID
    int handle = glutCreateWindow(argv[0]);     // param is the title of window

    // register GLUT callback functions
    glutDisplayFunc(displayCB);
    glutTimerFunc(20, timerCB, 20);             // set up timer callback for updating input state
    glutIdleFunc(idleCB);                       // redraw only every given millisec
    glutReshapeFunc(reshapeCB);
    glutKeyboardFunc(keyboardCB);
	glutKeyboardUpFunc(keyboardUpCB);
	glutSpecialFunc(specialKeyCB);			// used for the arrow keys etc.
	glutSpecialUpFunc(specialKeyUpCB);
    glutMouseFunc(mouseCB);
    glutMotionFunc(mouseMotionCB);		// used when the mouse moves and a mouse-button is held
	glutPassiveMotionFunc(mousePassiveMotionCB);	// used when the mouse moves and NO mouse-buttons are held
	
	//glutSetCursor(GLUT_CURSOR_NONE);		// used to hide the cursor

    return handle;
}


///////////////////////////////////////////////////////////////////////////////
// initialize OpenGL
// disable unused features
///////////////////////////////////////////////////////////////////////////////
void initGL() {
    //@glShadeModel(GL_SMOOTH);                    // shading mathod: GL_SMOOTH or GL_FLAT
    glShadeModel(GL_FLAT);                      // shading mathod: GL_SMOOTH or GL_FLAT
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment

    // enable /disable features
    //@glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    //@glEnable(GL_LIGHTING);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);

     // track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glClearColor(0, 0, 0, 0);                   // background color
    glClearStencil(0);                          // clear stencil buffer
    glClearDepth(1.0f);                         // 0 is near, 1 is far
    glDepthFunc(GL_LEQUAL);

    //@initLights();
}




///////////////////////////////////////////////////////////////////////////////
// write 2d text using GLUT
// The projection matrix must be set to orthogonal before call this function.
///////////////////////////////////////////////////////////////////////////////
void drawString(const char *str, int x, int y, float color[4], void *font) {
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
    glDisable(GL_TEXTURE_2D);

    glColor4fv(color);          // set text color
    glRasterPos2i(x, y);        // place text position

    // loop all characters in the string
    while(*str) {
        glutBitmapCharacter(font, *str);
        ++str;
    }

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPopAttrib();
}




///////////////////////////////////////////////////////////////////////////////
// draw a string in 3D space
///////////////////////////////////////////////////////////////////////////////
void drawString3D(const char *str, float pos[3], float color[4], void *font) {
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
    glDisable(GL_TEXTURE_2D);

    glColor4fv(color);          // set text color
    glRasterPos3fv(pos);        // place text position

    // loop all characters in the string
    while(*str) {
        glutBitmapCharacter(font, *str);
        ++str;
    }

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPopAttrib();
}



///////////////////////////////////////////////////////////////////////////////
// initialize global variables
///////////////////////////////////////////////////////////////////////////////
bool initSharedMem() {
    screenWidth = SCREEN_WIDTH;
    screenHeight = SCREEN_HEIGHT;

    cameraAngleX = cameraAngleY = 0;
    cameraDistance = CAMERA_DISTANCE;

    drawMode = 0; // 0:fill, 1: wireframe, 2:points

    // allocate texture buffer
    imageData = new GLubyte[DATA_SIZE];
    memset(imageData, 0, DATA_SIZE);

    return true;
}


///////////////////////////////////////////////////////////////////////////////
// clean up shared memory
///////////////////////////////////////////////////////////////////////////////
void clearSharedMem()
{
    // deallocate texture buffer
    delete [] imageData;
    imageData = 0;

    // clean up texture
    glDeleteTextures(1, &textureId);

    // clean up PBOs
    if(pboSupported)
    {
        glDeleteBuffersARB(2, pboIds);
    }
}


///////////////////////////////////////////////////////////////////////////////
// initialize lights
///////////////////////////////////////////////////////////////////////////////
void initLights()
{
    // set up light colors (ambient, diffuse, specular)
    GLfloat lightKa[] = {.2f, .2f, .2f, 1.0f};  // ambient light
    GLfloat lightKd[] = {.7f, .7f, .7f, 1.0f};  // diffuse light
    GLfloat lightKs[] = {1, 1, 1, 1};           // specular light
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

    // position the light
    float lightPos[4] = {0, 0, 20, 1}; // positional light
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_LIGHT0);                        // MUST enable each light source after configuration
}


///////////////////////////////////////////////////////////////////////////////
// set camera position and lookat direction
///////////////////////////////////////////////////////////////////////////////
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(posX, posY, posZ, targetX, targetY, targetZ, 0, 1, 0);
}

///////////////////////////////////////////////////////////////////////////////
// display info messages
///////////////////////////////////////////////////////////////////////////////
void showInfo() {
    // backup current model-view matrix
    glPushMatrix();                     // save current modelview matrix
    glLoadIdentity();                   // reset modelview matrix

    // set to 2D orthogonal projection
    glMatrixMode(GL_PROJECTION);     // switch to projection matrix
    glPushMatrix();                  // save current projection matrix
    glLoadIdentity();                // reset projection matrix
    gluOrtho2D(0, screenWidth, 0, screenHeight); // set to orthogonal projection

    float color[4] = {1, 1, 1, 1};

    stringstream ss;
    //ss << "PBO: ";
    //if(pboMode == 0)
    //    ss << "off" << ends;
    //else if(pboMode == 1)
    //    ss << "1 PBO" << ends;
    //else if(pboMode == 2)
    //    ss << "2 PBOs" << ends;

    //drawString(ss.str().c_str(), 1, screenHeight-TEXT_HEIGHT, color, font);
    //ss.str(""); // clear buffer

    ss << std::fixed << std::setprecision(3);
    ss << "Updating Time: " << updateTime << " ms" << ends;
    drawString(ss.str().c_str(), 1, screenHeight-(2*TEXT_HEIGHT), color, font);
    ss.str("");

 //   ss << "Copying Time: " << copyTime << " ms" << ends;
 //   drawString(ss.str().c_str(), 1, screenHeight-(3*TEXT_HEIGHT), color, font);
 //   ss.str("");

	//ss << "Press SPACE key to toggle PBO on/off." << ends;
	//drawString(ss.str().c_str(), 1, 1, color, font);

    // unset floating format
    ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);

    // restore projection matrix
    glPopMatrix();                   // restore to previous projection matrix

    // restore modelview matrix
    glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
    glPopMatrix();                   // restore to previous modelview matrix
}

///////////////////////////////////////////////////////////////////////////////
// display transfer rates
///////////////////////////////////////////////////////////////////////////////
void showTransferRate() {
    static Timer timer;
    static int count = 0;
    static stringstream ss;
    double elapsedTime;

    // backup current model-view matrix
    glPushMatrix();                     // save current modelview matrix
    glLoadIdentity();                   // reset modelview matrix

    // set to 2D orthogonal projection
    glMatrixMode(GL_PROJECTION);        // switch to projection matrix
    glPushMatrix();                     // save current projection matrix
    glLoadIdentity();                   // reset projection matrix
    //gluOrtho2D(0, IMAGE_WIDTH, 0, IMAGE_HEIGHT); // set to orthogonal projection
    gluOrtho2D(0, screenWidth, 0, screenHeight); // set to orthogonal projection

    float color[4] = {1, 1, 0, 1};

    // update fps every second
    elapsedTime = timer.getElapsedTime();
    if(elapsedTime < 1.0) {
        ++count;
    }
    else {
        ss.str("");
        ss << std::fixed << std::setprecision(1);
        ss << "Transfer Rate: " << (count / elapsedTime) * DATA_SIZE / (1024 * 1024) << " MB" << ends; // update fps string
        ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
        count = 0;                      // reset counter
        timer.start();                  // restart timer
    }
    drawString(ss.str().c_str(), 200, 286, color, font);

    // restore projection matrix
    glPopMatrix();                      // restore to previous projection matrix

    // restore modelview matrix
    glMatrixMode(GL_MODELVIEW);         // switch to modelview matrix
    glPopMatrix();                      // restore to previous modelview matrix
}



///////////////////////////////////////////////////////////////////////////////
// print transfer rates
///////////////////////////////////////////////////////////////////////////////
void printTransferRate() {
    const double INV_MEGA = 1.0 / (1024 * 1024);
    static Timer timer;
    static int count = 0;
    static stringstream ss;
    double elapsedTime;

    // loop until 1 sec passed
    elapsedTime = timer.getElapsedTime();
    if(elapsedTime < 1.0)
    {
        ++count;
    }
    else
    {
        cout << std::fixed << std::setprecision(1);
        cout << "Transfer Rate: " << (count / elapsedTime) * DATA_SIZE * INV_MEGA << " MB/s. (" << count / elapsedTime << " FPS)\n";
        cout << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
        count = 0;                      // reset counter
        timer.start();                  // restart timer
    }
}



///////////////////////////////////////////////////////////////////////////////
// set projection matrix as orthogonal
///////////////////////////////////////////////////////////////////////////////
void toOrtho() {
    // set viewport to be the entire window
    glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

    // set orthographic viewing frustum
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, screenWidth, 0, screenHeight, -1, 1);

    // switch to modelview matrix in order to set scene
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}



///////////////////////////////////////////////////////////////////////////////
// set the projection matrix as perspective
///////////////////////////////////////////////////////////////////////////////
void toPerspective() {
    // set viewport to be the entire window
    glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

    // set perspective viewing frustum
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (float)(screenWidth)/screenHeight, 1.0f, 1000.0f); // FOV, AspectRatio, NearClip, FarClip

    // switch to modelview matrix in order to set scene
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

