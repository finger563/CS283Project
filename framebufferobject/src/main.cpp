
#include "main.h"

#include "Library\library_funcs.h"		// GL and GLUT functions needed for code

#include "Engine\Object.h"
#include "Engine\camera.h"

short display_buffer[SIZE_X*SIZE_Y];
float z_buffer[SIZE_X*SIZE_Y];

// These are all values toggled by user input
int  rotx = 0,		// rotation about x axis, toggled by 'x'
	 roty = 0,		// rotation about y axis, toggled by 'y'
	 rotz = 0,		// rotation about z axis, toggled by 'z'
	 display_z_buffer = 0;		// render z-buffer instead of display-buffer, toggled by 'b'

Poly testpoly = Poly(Vertex(-6.666,6.666,0,1,0,0),
					 Vertex(13.333,6.666,0,1,1,0),
					 Vertex(-6.666,-13.333,0,1,0,1),
					 Vertex(),3,Vector3D(0,0,-1),TEXTURED);

Object testobj = Object(box,boxtexwidth,boxtexheight,Vector3D(),Point3D(-10,-5,15));
Object testobj2 = Object(box,boxtexwidth,boxtexheight,Vector3D(),Point3D(10,-5,15));
Object testobj3 = Object(floorlarge,floorlargewidth,floorlargeheight);
Object testobj4 = Object(testpoly,box,boxtexwidth,boxtexheight,Vector3D(),Point3D(0,0,15));

Matrix worldToCamera=Matrix(),
	   perspectiveProjection=Matrix(),
	   projectionToPixel=Matrix();

std::list<Object> objectlist;
std::list<Object> dynamiclist;
std::list<Poly> renderlist;


Matrix rmz = Matrix(), 
        rmy = Matrix(),
		neg_rmy = Matrix(),
        rmx = Matrix(),
        rmxy = Matrix(),
		rmxz = Matrix(),
		rmyz = Matrix(),
        rmxyz = Matrix();

float rot_angle = 3.141/60;
		
Matrix rot;		// debug/testing for rotating objects

Camera camera;

//shoot will be the projectile
Object shot = Object(box,boxtexwidth,boxtexheight,Vector3D(),Point3D(0, 0, 5));

// EVERYTHING FOR CHAT CODE ///////////////////////////////////////////////////
#include "Engine\chat.h"

std::string userName = "";
std::string playermsg = "";
bool print = true;
bool typing = false;

void PrintChat();

// INCLUDES AND DECLARATION FOR NETWORK CODE //////////////////////////////////
#include "Network\helper_funcs.h"

void SendChat(string sendstring);
void SendShot();
void SendMove(const Point3D& pos);
void SendLeave();

void RotateCamera(int x, int y) {
    float theta = (float)(x - glutGet(GLUT_WINDOW_WIDTH)/2.0)/((float)GLUT_WINDOW_WIDTH/2.0);
	float phi = -(float)(y - glutGet(GLUT_WINDOW_HEIGHT)/2.0)/((float)GLUT_WINDOW_HEIGHT/2.0);

	Camera tempcamera = player.Eye();
	tempcamera.Rotate(theta,phi);
    glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
#ifdef SERVER_CONTROLS_HEADING
	Message mymessage;
	mymessage.Type(MOVE);
	Object_s myobj = Object_s();
	myobj.SetType(PLAYER);
	myobj.SetID(player.Info().id);
	myobj.SetContent(player.Info().name);	

	myobj.x = tempcamera.GetPosition().x;
	myobj.y = tempcamera.GetPosition().y;
	myobj.z = tempcamera.GetPosition().z;
	myobj.hx = tempcamera.GetTheta();		// theta
	myobj.hy = tempcamera.GetPhi();		// phi
	myobj.hz = 0;		// not used
	mymessage.Object(myobj);
	event_handler.send(mymessage);
#else
	player.Eye(tempcamera);
#endif
}

void SendChat(string sendstring) {
	Message mymessage;
	mymessage.Type(CHAT);
	mymessage.Player(player.Info());
	mymessage.Content(sendstring.c_str());
	event_handler.send(mymessage);
}

void SendShot() {
	Message mymessage;
	mymessage.Type(SHOOT);
	Player_s info = player.Info();
	Camera eye = player.Eye();
	info.x = eye.GetPosition().x;
	info.y = eye.GetPosition().y;
	info.z = eye.GetPosition().z;
	info.hx = eye.GetForward().x;
	info.hy = eye.GetForward().y;
	info.hz = eye.GetForward().z;
	mymessage.Player(info);
	event_handler.send(mymessage);
}

void SendMove(const Point3D& pos) {
	Message mymessage;
	mymessage.Type(MOVE);
	Object_s myobj = Object_s();
	myobj.SetType(PLAYER);
	myobj.SetID(player.Info().id);
	myobj.SetContent(player.Info().name);
	Camera eye = player.Eye();

	Vector3D head = eye.GetForward();
	Vector3D right = eye.GetRight();
	Vector3D up = eye.GetUp();
	Vector3D trans = player.Eye().GetPosition() + head*pos.z + right*pos.x + up*pos.y;
	
	myobj.x = trans.x;
	myobj.y = trans.y;
	myobj.z = trans.z;
	myobj.hx = eye.GetTheta();		// theta
	myobj.hy = eye.GetPhi();		// phi
	myobj.hz = 0;		// not used
	mymessage.Object(myobj);
	event_handler.send(mymessage);
}

void SendLeave() {
	Message mymessage;
	mymessage.Type(LEAVE);
	mymessage.Player(player.Info());
	event_handler.send(mymessage);
}

///////////////////////////////////////////////////////////////////////////////
int ACE_TMAIN(int argc, ACE_TCHAR *argv[]) {    
	// parse the arguments
	ACE_DEBUG ((LM_DEBUG,
				"Player: parse the args\n"));
	if (parse_args (argc,argv) == -1) {
		ACE_ERROR ((LM_ERROR, ACE_TEXT ("(%P|%t) %p\n"), 
					ACE_TEXT ("parse failed")));
		return 0;
	}
	
	// grab a thread manager (we get the global singleton instance)
	ACE_Thread_Manager *thr_mgr = ACE_Thread_Manager::instance ();
	FuncArg_t *args;
	// declare a variable to hold the array of thread IDs
	ACE_thread_t  tid [1];
	
	// now spawn off NUM_THREADS passing it the thread entry function
	// and the argument we want to pass it. Here I am passing it a
	// topology. If you want addtional things to be passed, either have
	// them globally defined, which is not elegant, or have a complex
	// data structure that holds all the info you want to be passed.
	int group_id = 
		thr_mgr->spawn_n (tid,          // all the threads created
							1,			// tell how many
							(ACE_THR_FUNC) thread_func,  // entry point to the thread
							0,    // argument you want to pass
							THR_NEW_LWP | THR_JOINABLE
							// we want threads whose return status can be
							// monitored. Moreover, we want a lightweight
							// process.
							);

	if (group_id == -1) {
		ACE_ERROR ((LM_ERROR, ACE_TEXT ("(%P|%t) %p\n"), 
					ACE_TEXT ("spawn_n")));
		return -1;
	}

    rmz.data[0][0] = cos(rot_angle);
    rmz.data[0][1] = -sin(rot_angle);
    rmz.data[1][0] = sin(rot_angle);
    rmz.data[1][1] = cos(rot_angle);
    
    rmy.data[0][0] = cos(rot_angle);
    rmy.data[0][2] = -sin(rot_angle);
    rmy.data[2][0] = sin(rot_angle);
    rmy.data[2][2] = cos(rot_angle);

    neg_rmy.data[0][0] = cos(-rot_angle);
    neg_rmy.data[0][2] = -sin(-rot_angle);
    neg_rmy.data[2][0] = sin(-rot_angle);
    neg_rmy.data[2][2] = cos(-rot_angle);
    
    rmx.data[1][1] = cos(rot_angle);
    rmx.data[1][2] = -sin(rot_angle);
    rmx.data[2][1] = sin(rot_angle);
    rmx.data[2][2] = cos(rot_angle);
    
    rmxy = rmx*rmy;
	rmxz = rmx*rmz;
	rmyz = rmy*rmz;
    rmxyz = rmz*rmxy;

	// Structure of a transformation matrix:
	// ( r=rotation, p=projection, t=translation )
	// [ x y z w ]	| r r r p | = [ x' y' z' w']
	//				| r r r p |
	//				| r r r p |
	//				| t t t s |
	//				or 
	// | r r r t | | x | = | x' |
	// | r r r t | | y |   | y' |
	// | r r r t | | z |   | z' |
	// | p p p s | | w |   | w' |

	// We use ROW vector notation
		
	perspectiveProjection.data[3][2] = -1;	// translate z
	perspectiveProjection.data[2][3] = 1;	// project z

	projectionToPixel.data[3][0] = (float)SIZE_X*0.5;	// translate x
	projectionToPixel.data[3][1] = (float)SIZE_Y*0.5;	// translate y
	projectionToPixel.data[0][0] = (float)SIZE_X*0.5;	// scale x
	projectionToPixel.data[1][1] = (float)SIZE_Y*0.5;	// scale y

	testobj.generateCube();

	testobj2.generateCube();
	testobj2.SetRenderType(COLORED);

	testobj3.generateFloor(75,-10);

	//projectile code
	shot.projectileInit(camera.GetForward());

	objectlist.push_back(testobj);
	objectlist.push_back(testobj2);	
	objectlist.push_back(testobj3);
	objectlist.push_back(testobj4);

	userName = string(player.Info().name) + ":";

    initSharedMem();

    // register exit callback
    atexit(exitCB);

    // init GLUT and GL
    initGLUT(argc, argv);
    initGL();

    // get OpenGL info
    glInfo glInfo;
    glInfo.getInfo();
    //glInfo.printSelf();

    // init 2 texture objects
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, IMAGE_WIDTH, IMAGE_HEIGHT, 0, PIXEL_FORMAT, GL_UNSIGNED_BYTE, (GLvoid*)imageData);
    glBindTexture(GL_TEXTURE_2D, 0);

#ifdef _WIN32
    // check PBO is supported by your video card
    if(glInfo.isExtensionSupported("GL_ARB_pixel_buffer_object")) {
        // get pointers to GL functions
        glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
        glBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
        glBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
        glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)wglGetProcAddress("glBufferSubDataARB");
        glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
        glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)wglGetProcAddress("glGetBufferParameterivARB");
        glMapBufferARB = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
        glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");

        // check once again PBO extension
        if(glGenBuffersARB && glBindBufferARB && glBufferDataARB && glBufferSubDataARB &&
           glMapBufferARB && glUnmapBufferARB && glDeleteBuffersARB && glGetBufferParameterivARB) {
            pboSupported = true;
            pboMode = 1;    // using 1 PBO
            cout << "Video card supports GL_ARB_pixel_buffer_object." << endl;
        }
        else {
            pboSupported = false;
            pboMode = 0;    // without PBO
            cout << "Video card does NOT support GL_ARB_pixel_buffer_object." << endl;
        }
    }

#else // for linux, do not need to get function pointers, it is up-to-date
    if(glInfo.isExtensionSupported("GL_ARB_pixel_buffer_object")) {
        pboSupported = true;
        pboMode = 1;
        cout << "Video card supports GL_ARB_pixel_buffer_object." << endl;
    }
    else {
        pboSupported = false;
        pboMode = 0;
        cout << "Video card does NOT support GL_ARB_pixel_buffer_object." << endl;
    }
#endif

    if(pboSupported) {
        // create 2 pixel buffer objects, you need to delete them when program exits.
        // glBufferDataARB with NULL pointer reserves only memory space.
        glGenBuffersARB(2, pboIds);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pboIds[0]);
        glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, DATA_SIZE, 0, GL_STREAM_DRAW_ARB);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pboIds[1]);
        glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, DATA_SIZE, 0, GL_STREAM_DRAW_ARB);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    }

    // start timer, the elapsed time will be used for updateVertices()
    timer.start();

    // the last GLUT call (LOOP)
    // window will be shown and display callback is triggered by events
    // NOTE: this call never return main().
    glutMainLoop(); /* Start GLUT event-processing loop */

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// copy an image data to texture buffer, this updates what is rendered
///////////////////////////////////////////////////////////////////////////////
void updatePixels(GLubyte* dst, int size) {
    static int color = 0;

    if(!dst)
        return;

    int* ptr = (int*)dst;
	
	for (int y=0;y<SIZE_Y;y++)
		for (int x = 0; x<SIZE_X;x++) {
			display_buffer[x + y*SIZE_X] = BACKGROUND_COLOR;
			z_buffer[x + y*SIZE_X] = DEFAULT_Z_BUFFER;
		}

	renderlist.clear();

	dynamiclist.clear();
	Object_s* dynamic = player.Objects();
	Object tempobj;
	while ( dynamic != NULL ) {
		switch ( dynamic->type ) {
		case PLAYER:
			tempobj = Object(box,boxtexwidth,boxtexheight);
			tempobj.generateCube();
			tempobj.SetRenderType(TEXTURED);
			break;
		case SHOT:
			tempobj = Object();
			tempobj.generateCube(1.0);
			tempobj.SetRenderType(FLAT);
			break;
		default:
			break;
		}
		tempobj.setPosition(Point3D(dynamic->x,dynamic->y,dynamic->z));
		tempobj.setVel(Vector3D(dynamic->hx,dynamic->hy,dynamic->hz));
		dynamiclist.push_back(tempobj);
		dynamic = dynamic->next;
	}
	for (std::list<Object>::iterator it = dynamiclist.begin(); it != dynamiclist.end(); it++) {
		it->updateList();
		Vector3D tmppos = it->getPosition() - player.Eye().GetPosition();
		it->TranslateTemp(tmppos);
		worldToCamera.SetIdentity();
		worldToCamera = worldToCamera*player.Eye().GetWorldToCamera();
		it->TransformToCamera( worldToCamera );
		it->TransformToPerspective( perspectiveProjection );
		std::list<Poly> templist = it->getRenderList();
		renderlist.splice(renderlist.end(), templist);
	}

	for (std::list<Object>::iterator it = objectlist.begin(); it != objectlist.end(); it++) {
		it->updateList();
		Vector3D tmppos = it->getPosition() - player.Eye().GetPosition();
		it->TranslateTemp(tmppos);
		worldToCamera.SetIdentity();
		worldToCamera = worldToCamera*player.Eye().GetWorldToCamera();
		it->TransformToCamera( worldToCamera );
		it->TransformToPerspective( perspectiveProjection );
		std::list<Poly> templist = it->getRenderList();
		renderlist.splice(renderlist.end(), templist);
	}

	for (std::list<Poly>::iterator it = renderlist.begin(); it != renderlist.end(); it++) {
		it->Clip();
		//if ( !it->visible ) {
		//	it = renderlist.erase(it);
		//	goto TEST;
		//}
		it->HomogeneousDivide();
		it->TransformToPixel( projectionToPixel );
		it->SetupRasterization( );		// for speed optimization
	}

    for (int y=SIZE_Y-1;y>=0;y--) {
		for (std::list<Poly>::iterator it = renderlist.begin(); it != renderlist.end(); it++) {
			it->RasterizeFast(y);
		}
	}

	for (std::list<Object>::iterator it = objectlist.begin(); it != objectlist.end(); ++it) {
		it->Rotate(rot);	// purely for debugging fun
	}
	
    // copy 4 bytes at once
    for(int i = 0; i < IMAGE_HEIGHT; ++i) {
        for(int j = 0; j < IMAGE_WIDTH; ++j) {	// 0xAARRGGBB
			if (display_z_buffer) {
				*ptr = ((int)(z_buffer[j+i*SIZE_X]) << 16) 
					+ ((int)(z_buffer[j+i*SIZE_X]) << 8)
					+ (int)(z_buffer[j+i*SIZE_X]);
			}
			else {
				*ptr = ((int)RED_RGB(display_buffer[j+i*SIZE_X]) << 16) 
					+ ((int)GRN_RGB(display_buffer[j+i*SIZE_X]) << 8)
					+ (int)BLU_RGB(display_buffer[j+i*SIZE_X]);
			}
            ++ptr;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// display chat messages
///////////////////////////////////////////////////////////////////////////////
void PrintChat() {
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

	int numChats;
	std::list<string> conversation = player.Chat().Conversation();
	numChats = ( conversation.size() > player.Chat().NumChatsDisplayed() ) ? 
		player.Chat().NumChatsDisplayed() : conversation.size() ;
	std::list<string>::reverse_iterator riter = conversation.rbegin();

	for (int i=0; i < numChats; i++) {
		ss << *riter << ends;
		++riter;
		drawString(ss.str().c_str(), 1, 16+16*i, color, font);
		ss.str("");
	}

	if ( typing ) {
		if ( !playermsg.empty() ) {
			ss << userName << playermsg << ends;
			drawString(ss.str().c_str(), 1, 1, color, font); //positions at the bottom
		}
		else {
			ss << userName << ends;
			drawString(ss.str().c_str(), 1, 1, color, font); //positions at the bottom
		}
	}
	ss.str("");
    // unset floating format
    ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);

    // restore projection matrix
    glPopMatrix();                   // restore to previous projection matrix

    // restore modelview matrix
    glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
    glPopMatrix();                   // restore to previous modelview matrix
}

//=============================================================================
// CALLBACKS
//=============================================================================

void displayCB() {
    static int index = 0;
    int nextIndex = 0;                  // pbo index used for next frame

    if(pboMode > 0) {
        // "index" is used to copy pixels from a PBO to a texture object
        // "nextIndex" is used to update pixels in a PBO
        if(pboMode == 1) {
            // In single PBO mode, the index and nextIndex are set to 0
            index = nextIndex = 0;
        }
        else if(pboMode == 2) {
            // In dual PBO mode, increment current index first then get the next index
            index = (index + 1) % 2;
            nextIndex = (index + 1) % 2;
        }

        // start to copy from PBO to texture object ///////
        t1.start();

        // bind the texture and PBO
        glBindTexture(GL_TEXTURE_2D, textureId);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pboIds[index]);

        // copy pixels from PBO to texture object
        // Use offset instead of ponter.
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, PIXEL_FORMAT, GL_UNSIGNED_BYTE, 0);

        // measure the time copying data from PBO to texture object
        t1.stop();
        copyTime = t1.getElapsedTimeInMilliSec();
        ///////////////////////////////////////////////////


        // start to modify pixel values ///////////////////
        t1.start();

        // bind PBO to update pixel values
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pboIds[nextIndex]);

        // map the buffer object into client's memory
        // Note that glMapBufferARB() causes sync issue.
        // If GPU is working with this buffer, glMapBufferARB() will wait(stall)
        // for GPU to finish its job. To avoid waiting (stall), you can call
        // first glBufferDataARB() with NULL pointer before glMapBufferARB().
        // If you do that, the previous data in PBO will be discarded and
        // glMapBufferARB() returns a new allocated pointer immediately
        // even if GPU is still working with the previous data.
        glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, DATA_SIZE, 0, GL_STREAM_DRAW_ARB);
        GLubyte* ptr = (GLubyte*)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
        if(ptr) {
            // update data directly on the mapped buffer
            updatePixels(ptr, DATA_SIZE);
            glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB); // release pointer to mapping buffer
        }

        // measure the time modifying the mapped buffer
        t1.stop();
        updateTime = t1.getElapsedTimeInMilliSec();
        ///////////////////////////////////////////////////

        // it is good idea to release PBOs with ID 0 after use.
        // Once bound with 0, all pixel operations behave normal ways.
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    }
    else {
        ///////////////////////////////////////////////////
        // start to copy pixels from system memory to textrure object
        t1.start();

        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, PIXEL_FORMAT, GL_UNSIGNED_BYTE, (GLvoid*)imageData);

        t1.stop();
        copyTime = t1.getElapsedTimeInMilliSec();
        ///////////////////////////////////////////////////


        // start to modify pixels /////////////////////////
        t1.start();
        updatePixels(imageData, DATA_SIZE);
        t1.stop();
        updateTime = t1.getElapsedTimeInMilliSec();
        ///////////////////////////////////////////////////
    }


    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // save the initial ModelView matrix before modifying ModelView matrix
    glPushMatrix();

    // tramsform camera
    glTranslatef(0, 0, -cameraDistance);
    glRotatef(cameraAngleX, 1, 0, 0);   // pitch
    glRotatef(cameraAngleY, 0, 1, 0);   // heading

    // draw a point with texture
    glBindTexture(GL_TEXTURE_2D, textureId);
    glColor4f(1, 1, 1, 1);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);   glVertex3f( 1.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);   glVertex3f( 1.0f,  1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f);   glVertex3f(-1.0f,  1.0f, 0.0f);
    glEnd();

    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

	if(print) {
		PrintChat();
	}
	else {		// draw info messages
		showInfo();
		//showTransferRate();
	}
	
    //printTransferRate();
    glPopMatrix();
    glutSwapBuffers();
}

void reshapeCB(int width, int height) {
    screenWidth = width;
    screenHeight = height;
    toPerspective();
}

void timerCB(int millisec) {
    glutTimerFunc(millisec, timerCB, millisec);
    glutPostRedisplay();
}

void idleCB() {
    glutPostRedisplay();
}

void keyboardCB(unsigned char key, int x, int y) {
	Vector3D movevector = Vector3D();

    switch(key) {
    case 27: // ESCAPE
		if(!typing) {
			SendLeave();	// Notify server that we are leaving
			exit(0);
		}
		else {
			typing = false;
			playermsg.clear();
		}
        break;

	case 13: //ENTER
		if(typing) {
			if( !playermsg.empty() ) {
				string chatstring = userName + playermsg;
				player.AddChat(chatstring);
				SendChat(chatstring);
				playermsg.clear();
			}
			return;
		}
		break;
	case 8: //BACKSPACE
		if(typing) {
			if ( playermsg.size() > 0 )
				playermsg.erase(playermsg.size()-1);
			return;
		}
        break;
	}

	if(typing) {
		playermsg += key;
		return;
	}

	switch(key) {
	case 't': //chat function
		typing = true;
		//print = true;
		break;

	case 'w': // Up
		movevector = (Vector3D(0,0,1));
		break;

	case 's': // down
		movevector = (Vector3D(0,0,-1));
		break;

	case 'a': // left
		movevector = (Vector3D(-1,0,0));
		break;

	case 'd': // right
		movevector = (Vector3D(1,0,0));
		break;

	case ' ': // space
		movevector = (Vector3D(0,1,0));
		break;

	case 'c': // c
		movevector = (Vector3D(0,-1,0));
		break;

    case 'p':
        if(pboSupported)
            pboMode = ++pboMode % 3;
        cout << "PBO mode: " << pboMode << endl;
         break;
	
	case 'b':
	case 'B':
		display_z_buffer = !display_z_buffer;
		break;

	case 'x':
	case 'X':
		rotx = !rotx;
		break;

	case 'y':
	case 'Y':
		roty = !roty;
		break;

	case 'z':
	case 'Z':
		rotz = !rotz;
		break;

    case 'm': // switch rendering modes (fill -> wire -> point)
    case 'M':
        drawMode = ++drawMode % 3;
        if(drawMode == 0)        // fill mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
        }
        else if(drawMode == 1)  // wireframe mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        else                    // point mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        break;

    default:
        ;
    }

	switch ( rotx + roty*2 + rotz*4 ) {
	case 0:
		rot.SetIdentity();
		break;
	case 1:
		rot = rmx;
		break;
	case 2:
		rot = rmy;
		break;
	case 3:
		rot = rmxy;
		break;
	case 4:
		rot = rmz;
		break;
	case 5:
		rot = rmxz;
		break;
	case 6:
		rot = rmyz;
		break;
	case 7:
		rot = rmxyz;
		break;
	default:
		rot = rmxyz;
	}
	
	SendMove(movevector);
}

void specialKeyCB(int key, int x, int y) {
    switch(key) {
	case GLUT_KEY_UP:
		break;

	case GLUT_KEY_DOWN:
		break;

	case GLUT_KEY_LEFT:
		break;

	case GLUT_KEY_RIGHT:
		break;
	}
}

void mouseCB(int button, int state, int x, int y) {
    mouseX = x;
    mouseY = y;
	Camera tempcamera = player.Eye();

    if(button == GLUT_LEFT_BUTTON) {
        if(state == GLUT_DOWN) {
            mouseLeftDown = true;
			SendShot();
        }
        else if(state == GLUT_UP)
            mouseLeftDown = false;
    }

    else if(button == GLUT_RIGHT_BUTTON) {
        if(state == GLUT_DOWN) {
            mouseRightDown = true;
        }
        else if(state == GLUT_UP)
            mouseRightDown = false;
    }
}

bool warped = false;

void mouseMotionCB(int x, int y) {
    if(mouseLeftDown) {
    }
    if(mouseRightDown) {
    }
	if ( !warped ) {
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
		RotateCamera(x,y);
		warped = true;
	}
	else
		warped = false;
}

void exitCB() {
    clearSharedMem();
}
