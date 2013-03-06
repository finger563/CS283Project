//
// Lighthouse3D.com Framebuffer Objects Sample
//
//
// This demo was built for learning purposes only.
// Some code could be severely optimised, but I tried to
// keep it as simple and clear as possible.
//
// The code comes with no warranties, use it at your own risk.
// You may use it, or parts of it, wherever you want.
//
// See README.TXT for third party lib license detais and instructions
//
// If you do use it I would love to hear about it. Just post a comment
// at Lighthouse3D.com

// Have Fun :-)

#include <math.h>
#include <string>


// include GLEW to access OpenGL functions
#include <GL/glew.h>

// GLUT is the toolkit to interface with the OS
#include <GL/freeglut.h>

// Use Very Simple Libs
#include "include/VSL/vslibs.h"


#define _RENDERBUFFER 0
#define _TEXTURE 1
#define _NONE 2
// select the type of attachments
#define _DEPTH _TEXTURE
#define _COLOR _TEXTURE


// a handle for the framebuffer object
GLuint fbo;
// texture handlers
GLuint texFBO[2];
// renderbuffer handlers
GLuint rb[2];

// VSL Stuff
VSMathLib *vsml;
VSShaderLib shader, shaderF;
VSFontLib vsfl2, vsfl;
VSResModelLib myModel, myModel2, myPlane;
unsigned int aSentence;

// Camera Position
float camX, camY, camZ;

// Camera Spherical Coordinates
// angles in degrees
// beta is vertical, alpha horizontal
float alpha = 30.0f, beta = 30.0f;
// radius, distance from the center
float r = 5.0f;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Frame counting and FPS computation
long myTime,timebase = 0,frame = 0;
char s[128];

// the rotation angle for the cow 
float cowAngle = 0.0;


// ------------------------------------------------------------
//
// Reshape Callback Function
//

void changeSize(int w, int h) {

	float ratio;
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	ratio = (1.0f * w) / h;

	// define projection matrix
	vsml->loadIdentity(VSMathLib::PROJECTION);
	vsml->perspective(53.13f, ratio, 0.1f, 10000.0f);
}


// ------------------------------------------------------------
//
// Render stuff
//



void renderScene(void) {

	// bind a framebuffer object
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	glClearColor(0.4f, 0.4f, 0.8f, 1.0f);
	// Set Drawing buffers
	GLuint attachments[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1,  attachments);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// our FBO os 1024x1024
	// set the viewport and projection matrix accordingly
	changeSize(1024, 1024);

	// load identity matrices
	vsml->loadIdentity(VSMathLib::VIEW);
	vsml->loadIdentity(VSMathLib::MODEL);
	// set camera to render cow
	vsml->lookAt(0.0f ,0.0f ,2.5, 0,0,0, 0,1,0);
						
	// use shader for pixel lighthing
	glUseProgram(shader.getProgramIndex());
	// render cow
	vsml->rotate(cowAngle, 0.0f, 1.0f, 0.0f);
	myModel.render();

	// get back to the default framebuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

#if (_COLOR == _RENDERBUFFER)

	// copy from rb[0] to texFBO[0]
	glCopyImageSubData(rb[0], GL_RENDERBUFFER, 0, 
						0,0,0, 
						texFBO[0], GL_TEXTURE_2D, 0, 
						0,0,0, 
						1024,1024,1);
#endif


	// clear default frame buffer for the whoel window
	changeSize(1024, 512);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// DRAW PLANE WITH TEXTURE FROM FBO	
	// render a quad on the left size of the window
	// with the framebuffer object result
	// set the viewport to the left half of the window
	glViewport(0,0,512,512);

	// use the fonts shader as its appropriate 
	// for non lit textured models
	glUseProgram(shaderF.getProgramIndex());
	// load identity matrices
	vsml->loadIdentity(VSMathLib::MODEL);
	vsml->loadIdentity(VSMathLib::VIEW);
	vsml->loadIdentity(VSMathLib::PROJECTION);

	// render plane
	// the original plane is horizontal
	// rotate it to make it vertical
	vsml->rotate(90, 1.0, 0.0, 0.0);
	myPlane.render();



	// DRAW CUBE MODEL USING THE FBO AS A TEXTURE
	// set the projection matrix
	changeSize(512,512);
	// set the viewport to the right half of the window
	glViewport(512, 0, 512, 512);
	// use the directional light shader

	// load identity matrices for VIEW and MODEL
	vsml->loadIdentity(VSMathLib::VIEW);
	vsml->loadIdentity(VSMathLib::MODEL);
	// set camera
	vsml->lookAt(camX, camY, camZ, 0,0,0, 0,1,0);
	// render cube, textured with cow!
	glUseProgram(shader.getProgramIndex());
	myModel2.render();
		
	//------------------------------------------
	// Done. Restoring Viewport
	changeSize(1024,512);
	// set the font shader
	glUseProgram(shaderF.getProgramIndex());

	// FPS computation
	frame++;
	myTime=glutGet(GLUT_ELAPSED_TIME);
	if (myTime - timebase > 1000) {
			sprintf(s,"FPS:%4.2f",
				frame*1000.0/(myTime-timebase));
		timebase = myTime;
		frame = 0;
		vsfl.prepareSentence(aSentence,s);
	}
	// render the sentence with FPS info
	vsfl.renderSentence(10,10,aSentence);

	glutSwapBuffers();

	cowAngle += 0.1f;
}


// ------------------------------------------------------------
//
// Events from the Keyboard
//

void processKeys(unsigned char key, int xx, int yy)
{
	switch(key) {

		case 27:

			glutLeaveMainLoop();
			break;

		case 'm': glEnable(GL_MULTISAMPLE); break;
		case 'n': glDisable(GL_MULTISAMPLE); break;
				break;

	}

//  uncomment this if not using an idle func
//	glutPostRedisplay();
}


// ------------------------------------------------------------
//
// Mouse Events
//

void processMouseButtons(int button, int state, int xx, int yy)
{
	// start tracking the mouse
	if (state == GLUT_DOWN)  {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
	}

	//stop tracking the mouse
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha -= (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {
			r += (yy - startY) * 0.01f;
		}
		tracking = 0;
	}
}

// Track mouse motion while buttons are pressed

void processMouseMotion(int xx, int yy)
{

	int deltaX, deltaY;
	float alphaAux, betaAux;
	float rAux;

	deltaX =  - xx + startX;
	deltaY =    yy - startY;

	// left mouse button: move camera
	if (tracking == 1) {


		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0f)
			betaAux = 85.0f;
		else if (betaAux < -85.0f)
			betaAux = -85.0f;

		rAux = r;

		camX = rAux * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
		camZ = rAux * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
		camY = rAux *   						       sin(betaAux * 3.14f / 180.0f);
	}
	// right mouse button: zoom
	else if (tracking == 2) {

		alphaAux = alpha;
		betaAux = beta;
		rAux = r + (deltaY * 0.01f);
		if (rAux < 2.0)
			rAux = 2.0;

		camX = rAux * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
		camZ = rAux * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
		camY = rAux *   						       sin(betaAux * 3.14f / 180.0f);
	}


//  uncomment this if not using an idle func
//	glutPostRedisplay();
}


void mouseWheel(int wheel, int direction, int x, int y) {

	r += direction * 0.1f;
	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r *   						     sin(beta * 3.14f / 180.0f);

//  uncomment this if not using an idle func
//	glutPostRedisplay();
}




// --------------------------------------------------------
//
// Shader Stuff
//


GLuint setupShaders() {

	// Shader for fonts
	shaderF.init();
	shaderF.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/color.vert");
	shaderF.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/color.frag");

	// set semantics for the shader variables
	shaderF.setProgramOutput(0,"outputF");
	shaderF.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "position");
	shaderF.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "texCoord");

	shaderF.prepareProgram();
	VSGLInfoLib::getUniformsInfo(shaderF.getProgramIndex());

	// add sampler uniforms
	shaderF.setUniform("texUnit", 0);

	printf("InfoLog for Font Shader\n%s\n\n", shaderF.getAllInfoLogs().c_str());



	// Shader for models
	shader.init();

	shader.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/pixeldirdifambspec.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/pixeldirdifambspec.frag");

	// set semantics for the shader variables
	shader.setProgramOutput(0,"outputF");
	shader.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "position");
	shader.setVertexAttribName(VSShaderLib::NORMAL_ATTRIB,"normal");
	shader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "texCoord");


	shader.prepareProgram();
	VSGLInfoLib::getUniformsInfo(shader.getProgramIndex());

	// add sampler uniforms
	shader.setUniform("texUnit", 0);
	// direction towards the light
	float f[3] = {1.0f, 1.0f, 1.0f};
	shader.setUniform("lightDir", f);
	
	printf("InfoLog for Model Shader\n%s\n\n", shader.getAllInfoLogs().c_str());
	
	return(shader.isProgramLinked());
}


// ------------------------------------------------------------
//
//  Framebufffer Object's stuff
//

void printFramebufferLimits() {

	int res;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &res);
	printf("Max Color Attachments: %d\n", res);

	glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &res);
	printf("Max Framebuffer Width: %d\n", res);

	glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &res);
	printf("Max Framebuffer Height: %d\n", res);

	glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &res);
	printf("Max Framebuffer Samples: %d\n", res);

	glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &res);
	printf("Max Framebuffer Layers: %d\n", res);

	glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &res);
	printf("Max Framebuffer Layers: %d\n", res);

}


void printFramebufferInfo(GLenum target, GLuint fbo) {

	glBindFramebuffer(target,fbo);

	int res;

	GLint buffer;
	int i = 0;
	do {
		glGetIntegerv(GL_DRAW_BUFFER0+i, &buffer);

		if (buffer != GL_NONE) {

			printf("Shader Output Location %d - color attachment %d\n", i, buffer - GL_COLOR_ATTACHMENT0);
			
			glGetFramebufferAttachmentParameteriv(target, buffer, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &res);
			printf("\tAttachment Type: %s\n", res==GL_TEXTURE?"Texture":"Render Buffer");
			glGetFramebufferAttachmentParameteriv(target, buffer, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &res);
			printf("\tAttachment object name: %d\n",res);
		}
		++i;

	} while (buffer != GL_NONE);
}


GLuint createRGBATexture(int w, int h) {

	GLuint tex;

	glGenTextures(1, &tex);

	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
						w,
						h, 
						0, GL_RGBA, GL_UNSIGNED_BYTE,
						NULL); 

	glBindTexture(GL_TEXTURE_2D, 0);

	return(tex);
}

GLuint createDepthTexture(int w, int h) {

	GLuint tex;

	glGenTextures(1, &tex);

	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
						w,
						h, 
						0, GL_DEPTH_COMPONENT, GL_FLOAT,
						NULL); 

	glBindTexture(GL_TEXTURE_2D, 0);

	return(tex);
}

GLuint prepareFBO(int w, int h, int colorCount) {

	GLuint fbo;
	// Generate one frame buffer
	glGenFramebuffers(1, &fbo);
	// bind it
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	// attach textures for colors
	for (int i = 0; i < colorCount; ++i) {
		texFBO[i] = createRGBATexture(w,h);
	
#if (_COLOR == _TEXTURE)
		glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, texFBO[i], 0);
#elif (_COLOR == _RENDERBUFFER)
		GLuint rbAux;
		glGenRenderbuffers(1, &rbAux);
		rb[i] = rbAux;
		glBindRenderbuffer(GL_RENDERBUFFER, rb[i]);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, w,h);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_RENDERBUFFER, rb[i]);
#endif
	}

#if (_DEPTH == _RENDERBUFFER)
	// attach renderbuffer for depth
	GLuint fb;
	glGenRenderbuffers(1, &fb);
	glBindRenderbuffer(GL_RENDERBUFFER, fb);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w,h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fb);

#elif (_DEPTH == _TEXTURE)
	// attach depth texture
	GLuint depthFBO;
	depthFBO = createDepthTexture(w,h);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthFBO, 0);
#endif
	// check if everything is OK
	GLenum e = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	switch (e) {
	
		case GL_FRAMEBUFFER_UNDEFINED:
			printf("FBO Undefined\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT :
			printf("FBO Incomplete Attachment\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT :
			printf("FBO Missing Attachment\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER :
			printf("FBO Incomplete Draw Buffer\n");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED :
			printf("FBO Unsupported\n");
			break;
		case GL_FRAMEBUFFER_COMPLETE:
			printf("FBO OK\n");
			break;
		default:
			printf("FBO Problem?\n");
	}

	if (e != GL_FRAMEBUFFER_COMPLETE)
		return (0);
	// unbind fbo
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

//	printFramebuffersInfo(fbo);

	return fbo;
}

// ------------------------------------------------------------
//
// Model loading and OpenGL setup
//



int init()
{

	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r *   						     sin(beta * 3.14f / 180.0f);

	// load models
	if (myModel2.load("models/cube2.obj") && 
		myModel.load("models/cow.obj") &&
		myPlane.load("models/plane.obj")) {

		// "Material" will be the block name in the shaders
		// with the material properties
		myModel.setMaterialBlockName("Material");
		myModel2.setMaterialBlockName("Material");

		// some GL settings
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_MULTISAMPLE);

		// prepare an FBO 1024x1024 with a single color attachment
		fbo = prepareFBO(1024, 1024, 1);
		// fbo = 0 means that there was a problem building the fbo
		if (fbo == 0) {
			printf("Error in FBO creation\n");
			return false;
		}

		printFramebufferInfo(GL_DRAW_FRAMEBUFFER, fbo);

		// set the result of the FBO as the texture in both
		// the cube and plane objects
		myModel2.setTexture(0, texFBO[0]);
		myPlane.setTexture(0, texFBO[0]);

		return true;
	}
	else {
		printf("%s\n",myModel.getErrors().c_str());
		return false;
	}

}


void initVSL() {

//	 Init VSML
	vsml = VSMathLib::getInstance();
	// the shaders will use a block named Matrices
	vsml->setUniformBlockName("Matrices");
	// these are the matrices we will use in the shaders
	// in here we are setting their semantics
	vsml->setUniformName(VSMathLib::PROJ_VIEW_MODEL, "projViewModelMatrix");
	vsml->setUniformName(VSMathLib::NORMAL, "normalMatrix");
	vsml->setUniformName(VSMathLib::VIEW_MODEL, "viewModelMatrix");
	vsml->setUniformName(VSMathLib::VIEW, "viewMatrix");

	// Init VSFL Fonts
	vsfl.load("fonts/couriernew10");
	vsfl.setFixedFont(true);
	vsfl.setMaterialBlockName("Material");
	aSentence = vsfl.genSentence();
}

// ------------------------------------------------------------
//
// Main function
//


int main(int argc, char **argv) {

//  GLUT initialization
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA|GLUT_MULTISAMPLE);

	// Context and profile defs
	glutInitContextVersion (3, 3);
	glutInitContextProfile (GLUT_CORE_PROFILE );
	glutInitContextFlags(GLUT_DEBUG);

	glutInitWindowPosition(100,100);
	glutInitWindowSize(1024,512);
	glutCreateWindow("Lighthouse3D - OpenGL Framebuffer objects demo");

//  Callback Registration
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);

//	Mouse and Keyboard Callbacks
	glutKeyboardFunc(processKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);

	glutMouseWheelFunc ( mouseWheel ) ;

//	return from main loop
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

//	Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();
	if (glewIsSupported("GL_VERSION_3_3"))
		printf("Ready for OpenGL 3.3\n");
	else {
		printf("OpenGL 3.3 not supported\n");
		exit(1);
	}

   printf ("Vendor: %s\n", glGetString (GL_VENDOR));
   printf ("Renderer: %s\n", glGetString (GL_RENDERER));
   printf ("Version: %s\n", glGetString (GL_VERSION));
   printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
   int param;
   glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &param);
   if (param == GL_CONTEXT_CORE_PROFILE_BIT)
	   printf("Context Profile: Core\n");
   else
	   printf("Context Profile: Compatibility\n");
   glGetIntegerv(GL_CONTEXT_FLAGS, &param);

   printFramebufferLimits();

	if (!setupShaders())
		exit(1);

	if (!init()) {
		printf("Could not Load the Model\n");
		exit(1);
	}

	initVSL();

	//  GLUT main loop
	glutMainLoop();

	return(0);

}

