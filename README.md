CS283Project : Super Block Fighter Extreme
============

CS 283 Networked 3D Game Project

This project is written by William Emfinger and Paul Mcneil for Vanderbilt's 
spring 2013 CS283 Computer Networks class.

The program is a networked 3D first person game in which the player can use the 
mouse and keyboard to move and look in a 3D virtual environment.  The player can
connect to a server, and will be able to chat with, see, and interact with other
players on that server.  

The rendering code for this projet is written from scratch, and only uses a small
subset of OpenGL in order to have access to a pixel buffer.  The vector/matrix 
manipulation, the polygon transformations and rasterizations, and all of the 
camera and object code is entirely written from scratch.  As such, this graphics
library (contained within ./src/Engine) is portable to any device which has a C++ 
compiler.  Modifications to the code to work in a C only environment are not difficult.

System Requirements
-------------------
For the solution/project, it is recommended that you use MSVS2010.

The graphics interface part of the code is handled by the GLUT dll which is included with the project.
This dll handles the interface to the GPU for rendering to the screen.  The graphics library 
is also entirely contained in the code, relying on the GLUT dll only for access to a pixel buffer.

The networked part of the code relies on ACE for the networking.  You can download ACE
[here][acedl], and you need Perl which can can be downloaded [here][perldl]

[acedl]: http://download.dre.vanderbilt.edu
[perldl]: http://www.activestate.com/activeperl/downloads

Install Perl first, then install and build ACE.
Follow these steps to install ACE (in Windows):
 
 * Uncompress ACE to C:\ACE_wrappers (or some other folder of your choice)
 * Define environment variable ACE_ROOT to be C:\ACE_wrappers (or whatever folder you chose)
 * Define environment variable MPC_ROOT to be %ACE_ROOT%\MPC
 * Update the path to be PATH=%PATH%;%ACE_ROOT%\bin;%ACE_ROOT%\lib
 * Create config.h in %ACE_ROOT%\ace directory, putting in the file:
 
	\#define ACE_HAS_STANDARD_CPP_LIBRARY 1

	\#include "ace/config-win32.h"
	
 * open command prompt, 
 
	cd %ACE_ROOT%\ace
	mwc.pl -type vc10 ace.mwc
	
 * open the ACE_wrappers_vc10 project in the %ACE_ROOT% folder and build 
   the solution (build both Debug and Release).

Getting the project files
-------------------------
 
Currently the code is set up as a MSVS 2010 solution containing two projects
 * SBFE_Client
 * SBFE_Server

Running the Server
------------------
To run the server, simply run 
	
	SBFE_Server -i <ip address>:<port number>

If you do not provide the -i argument, the server will default to 127.0.0.1:9999.
Also, if you want to show the command line options, just run

	SBFE_Server -?

Running the Game
----------------
To run the game, simply run 
	
	SBFE_Client -i <ip address>:<port number> -n <user name>

If you do not provide the -i argument, the client will try to connect to 127.0.0.1:9999.
The username is used for chat display both locally and on other clients.  User name deafults
to TestUser. Also, if you want to show the command line options, just run

	SBFE_Client -?

Controls for the game:
 * w/a,s/d: move forward/backward, strafe left/right
 * space: move camera up
 * c: move camera down
 * q/e: rotate camera left/right
 * t: initiate chat (during chat, keyboard commands are disabled)
 * enter (during chat): send message
 * backspace (during chat): remove last character added
 * escape: quit program or (during chat) exit chat
 * Mouse left click: fire projectile
 
FEATURES
--------
 * Chat over network with usernames & chat history
 * Server selects spawn location for players
 * Player can move about freely in virtual world and fire projectiles
 * Server correctly tracks connected/registered players and handles terminated connections
 * Perspective-correct texturemapping, normal interpolation, z-buffering, and color interpolation
 * Support for different rendering types for objects
 * Extendable messaging format
 * Extendable rendering engine with support for advanced rendering techniques

TODO
----
 * Update server code to keep track of players & shots
   * Send all currently active dynamic objects to players that join later
   * Update all dynamic objects using timeout function (i.e. periodically)
 * Use mouse movement for camera rotation
 * Finish SHOOT,CREATE,MOVE,LEAVE,REMOVE server/client implementations
 * Collision detection
 * Physics (for jumping)
 * Build world
 * Have server randomly choose spawn points for players.
 * Allow killing of characters and their subsequent respawn
 * Lighting system (smooth shading, shadowing, etc.)
