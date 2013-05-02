
#include "Library\library_funcs.h"		// GL and GLUT functions needed for code

#include "..\Engine\main.h"
#include "..\Engine\Object.h"
#include "..\Engine\world.h"

int  display_z_buffer = 0;		// render z-buffer instead of display-buffer, toggled by 'b'

#include "ace/Thread_Mutex.h"
// lock variable for mutual exclusion
ACE_Thread_Mutex  lock;

Matrix worldToCamera=Matrix(),
	   perspectiveProjection=Matrix(),
	   projectionToPixel=Matrix();

std::list<Object> objectlist;		// used for the static world objects
std::list<Object> dynamiclist;		// used for dynamic objects received from server
std::list<Poly> renderlist;			// aggregate polygon list to be rendered

// INPUT HANDLING CODE ////////////////////////////////////////////////////////
bool keyStates[256] = {};				// for regular keyboard keys
bool keySpecialStates[256] = {};		// for "special" keyboard keys, i.e. arrow keys
bool mouseLeftClick = false;
bool mouseRightClick = false;
double mouseX,mouseY;
double theta = 0,
	  phi = 0;
void KeyOperations();

// EVERYTHING FOR CHAT CODE ///////////////////////////////////////////////////
#include "..\Engine\chat.h"

std::string userName = "";
std::string playermsg = "";
bool print = true;
bool typing = false;

void PrintChat();
void PrintDebug();

// INCLUDES AND DECLARATION FOR NETWORK CODE //////////////////////////////////
#include "..\Network\helper_funcs.h"

int RTT_ = 0;

Camera tempeye;

void SendChat(string sendstring);
void SendShot();
void SendMove();
void SendLeave();

void SendChat(string sendstring) {
	Message mymessage;
	mymessage.SetType(CHAT);
	mymessage.SetPlayer(player.Info());
	mymessage.SetContent(sendstring.c_str());
	event_handler.send(mymessage);
}

void SendShot() {
	if ( player.Registered() ) {
		Message mymessage;
		mymessage.SetType(SHOOT);
		Player_s info = player.Info();
		Camera eye = player.Eye();
		info.x = eye.GetPosition().x;
		info.y = eye.GetPosition().y;
		info.z = eye.GetPosition().z;
		info.theta = eye.GetTheta();
		info.phi = eye.GetPhi();
		mymessage.SetPlayer(info);
		event_handler.send(mymessage);
	}
}

void SendMove() {
	if ( player.Registered() ) {
		Message mymessage;
		mymessage.SetType(MOVE);
		Object_s myobj = Object_s();
		myobj.SetType(PLAYER);
		myobj.SetID(player.Info().id);
		myobj.SetContent(player.Info().name);

		myobj.x = player.Eye().GetPosition().x;
		myobj.y = player.Eye().GetPosition().y;
		myobj.z = player.Eye().GetPosition().z;
		myobj.theta = player.Eye().GetTheta();		// theta
		myobj.phi = player.Eye().GetPhi();		// phi
		mymessage.SetObject(myobj);
		event_handler.send(mymessage);
	}
	theta = 0;		// zero the delta's for the angles
	phi = 0;
}

void SendLeave() {
	Message mymessage;
	mymessage.SetType(LEAVE);
	mymessage.SetPlayer(player.Info());
	event_handler.send(mymessage);
}

// MAIN FUNCTION ///////////////////////////////////////////////////////////////
saint ACE_TMAIN(int argc, ACE_TCHAR *argv[]) {    
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

	projectionToPixel.data[3][0] = (double)SIZE_X*0.5;	// translate x
	projectionToPixel.data[3][1] = (double)SIZE_Y*0.5;	// translate y
	projectionToPixel.data[0][0] = (double)SIZE_X*0.5;	// scale x
	projectionToPixel.data[1][1] = (double)SIZE_Y*0.5;	// scale y

	userName = string(player.Info().name) + ":";

	//while ( !player.Registered() );	// do nothing until player accepts us

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

// copy an image data to texture buffer, this updates what is rendered
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
	std::list<Object_s> dynamic = player.Objects();
	Object tempobj;
	for (std::list<Object_s>::iterator it = dynamic.begin(); it != dynamic.end(); it++) {
		switch ( it->type ) {
		case PLAYER:
			tempobj.GeneratePlayer(Point3D(it->x,it->y,it->z),it->theta,it->phi);
			break;
		case SHOT:
			tempobj.GenerateShot(Point3D(it->x,it->y,it->z),it->theta,it->phi);
			break;
		default:
			break;
		}
		tempobj.SetVelocity(Vector3D(it->vx,it->vy,it->vz));
		dynamiclist.push_back(tempobj);
	}

	for (std::list<Object>::iterator it = dynamiclist.begin(); it != dynamiclist.end(); it++) {
		it->updateList();
		Vector3D tmppos = it->GetPosition() - player.Eye().GetPosition();
		it->TranslateTemp(tmppos);
		worldToCamera.SetIdentity();
		worldToCamera = worldToCamera*player.Eye().GetWorldToCamera();
		it->TransformToCamera( worldToCamera );
		it->TransformToPerspective( perspectiveProjection );
		std::list<Poly> templist = it->GetRenderList();
		renderlist.splice(renderlist.end(), templist);
	}

	for (std::list<Object>::iterator it = objectlist.begin(); it != objectlist.end(); it++) {
		it->updateList();
		Vector3D tmppos = it->GetPosition() - player.Eye().GetPosition();
		it->TranslateTemp(tmppos);
		worldToCamera.SetIdentity();
		worldToCamera = worldToCamera*player.Eye().GetWorldToCamera();
		it->TransformToCamera( worldToCamera );
		it->TransformToPerspective( perspectiveProjection );
		std::list<Poly> templist = it->GetRenderList();
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

void KeyOperations() {
	if ( !typing ) {
		Vector3D movevector = Vector3D();
		Camera eye = player.Eye();

		if (keyStates['w'] || keyStates['W']) { // Up
			movevector = movevector + (Vector3D(0,0,1));
		}

		if (keyStates['s'] || keyStates['S']) { // down
			movevector = movevector + (Vector3D(0,0,-1));
		}

		if (keyStates['a'] || keyStates['A']) { // left
			movevector = movevector + (Vector3D(-1,0,0));
		}

		if (keyStates['d'] || keyStates['D']) { // right
			movevector = movevector + (Vector3D(1,0,0));
		}

		if (keyStates[' ']) { // space
			eye.SetPosition(eye.GetPosition().x,
							eye.GetPosition().y + 1,
							eye.GetPosition().z);
		}

		if (keyStates['c']) {
			eye.SetPosition(eye.GetPosition().x,
							eye.GetPosition().y - 1,
							eye.GetPosition().z);
		}

		if (keyStates['b']) {
			display_z_buffer = !display_z_buffer;
		}
	
		eye.Translate(movevector);
		player.Eye(eye);
	}
}

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
	
	ss << "Health: " << player.Info().life << ends;
	drawString(ss.str().c_str(), glutGet(GLUT_WINDOW_WIDTH) - 8*strlen(ss.str().c_str()), 1, color, font);
	ss.str("");

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

void PrintDebug() {
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

	ss << "Position: ("<<player.Eye().GetPosition().x<<
					","<<player.Eye().GetPosition().y<<
					","<<player.Eye().GetPosition().z<<
					")" << ends;
	drawString(ss.str().c_str(), 1, glutGet(GLUT_WINDOW_HEIGHT)-13, color, font);
	ss.str("");
	ss << "Orientation: ("<<player.Eye().GetTheta()<<
						","<<player.Eye().GetPhi()<< ")" << ends;
	drawString(ss.str().c_str(), 1, glutGet(GLUT_WINDOW_HEIGHT)-13*2, color, font);
	ss.str("");
	ss << "RTT: " << RTT_ << " ms." << ends;
	drawString(ss.str().c_str(), 1, glutGet(GLUT_WINDOW_HEIGHT)-13*3, color, font);
	ss.str("");
    ss << "Updating Time: " << updateTime << " ms" << ends;
    drawString(ss.str().c_str(), 1, glutGet(GLUT_WINDOW_HEIGHT)-13*4, color, font);
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

	PrintChat();
#if defined(ON_SCREEN_DEBUG)
	PrintDebug();
#endif

    //printTransferRate();
    glPopMatrix();
    glutSwapBuffers();
}

void reshapeCB(int width, int height) {
    screenWidth = width;
    screenHeight = height;
    toPerspective();
}

void KeyboardTimerCB(int millisec) {
    glutTimerFunc(millisec, KeyboardTimerCB, millisec);
	KeyOperations();
    glutPostRedisplay();
}

void NetworkTimerCB(int millisec) {
	ACE_hrtime_t elapsed;
	player.GetMicroseconds(elapsed);
	if ( elapsed < MIN_NETWORK_UPDATE_TIME*1000 ) 
		millisec = MIN_NETWORK_UPDATE_TIME;
	else
		millisec = elapsed/1000;
	if ( millisec > MAX_NETWORK_UPDATE_TIME )
		millisec = MAX_NETWORK_UPDATE_TIME;
	RTT_=millisec;
	player.StopTimer();
	player.ResetTimer();
	SendMove();
	player.StartTimer();
    glutTimerFunc(millisec, NetworkTimerCB, millisec);
}

void idleCB() {
    glutPostRedisplay();
}

void keyboardCB(unsigned char key, int x, int y) {
	keyStates[key] = true;
	Vector3D movevector = Vector3D();
    switch(key) {
    case 27: // ESCAPE
		if(!typing) {
			SendLeave();	// Notify server that we are leaving
			player.Leave();
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

	if (keyStates['t']) { //chat function
		typing = true;
		return;
	}
}

void specialKeyCB(int key, int x, int y) {
	keySpecialStates[key] = true;
}

void keyboardUpCB(unsigned char key, int x, int y) {
	keyStates[key] = false;
}

void specialKeyUpCB(int key, int x, int y) {
	keySpecialStates[key] = false;
}

void mouseCB(int button, int state, int x, int y) {
    mouseX = x;
    mouseY = y;

    if(button == GLUT_LEFT_BUTTON) {
        if(state == GLUT_DOWN) {
            mouseLeftClick = true;
			SendShot();
        }
        else if(state == GLUT_UP)
            mouseLeftClick = false;
    }

    else if(button == GLUT_RIGHT_BUTTON) {
        if(state == GLUT_DOWN) {
            mouseRightClick = true;
        }
        else if(state == GLUT_UP)
            mouseRightClick = false;
    }
}

bool warped = false;		// without this, the mouse warp (capture) generates endless loop

void mouseMotionCB(int x, int y) {
}

void mousePassiveMotionCB(int x, int y) {
	if ( !warped ) {
		theta = (double)(x - glutGet(GLUT_WINDOW_WIDTH)/2.0)/((double)GLUT_WINDOW_WIDTH);
		phi = -(double)(y - glutGet(GLUT_WINDOW_HEIGHT)/2.0)/((double)GLUT_WINDOW_HEIGHT);
		if ( theta > 2.0*3.141592 ) {
			theta = theta - 2.0*3.141592;
		}
		else if ( theta < -2.0*3.141592 ) {
			theta = theta + 2.0*3.141592;
		}
		if ( phi > 3.141592/2.0 ) {
			phi = 3.141592/2.0;
		}
		else if ( phi < -3.141592/2.0 ) {
			phi = -3.141592/2.0;
		}
		//tempeye.Rotate(theta,phi);
		Camera eye = player.Eye();
		eye.Rotate(theta,phi);
		player.Eye(eye);
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
		warped = true;
	}
	else
		warped = false;
}

void exitCB() {
    clearSharedMem();
}
