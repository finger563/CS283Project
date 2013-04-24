CS283Project : Super Block Fighter Extreme
============

CS 283 Networked 3D Game Project

This project is written by William Emfinger and Paul Mcneil for Vanderbilt's 
spring 2013 CS283 Computer Networks class.

The program is a networked 3D first person game in which the player can use the 
mouse and keyboard to move and look and shoot in a 3D virtual environment.  The player can
connect to a server, and can chat with, see, and interact with other
players on that server.  The server determines the world which the players load, and runs
the collision detection on the dynamic objects (players and shots), maintaining the 
state for all dynamic objects in the game.  

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

You should set up both projects (client & server) as startup projects, so that they both start when debugging.
This means that when you run/debug the solution, you get both parts of the network code 
running at the same time and can debug both parts at the same time if you want.  Moreover,
by right-clicking on the SBFE_Client project in the solution explorer and selecting Debug->start 
new instance, you can spawn more clients to do more thorough network testing without the need
for any command windows to go to the exe folder and start them.

However, should you wish to run the exe's from the command window (i.e. without MSVS running),
you can do so by opening a command window (cygwin,cmd, etc.) and cd-ing into the Release directory
of the solution.  You need to make sure that freeglut.dll (in the SBFE_Client project folder) and 
ACEd.dll (wherever you installed ACE) are both either in the PATH or are in the Release folder.  
Assuming they are, you can then run the executables from the command line.

Running the Server
------------------
To run the server, simply run 
	
	SBFE_Server -i <ip address>:<port number> - l <Level ID number>

If you do not provide the -i argument, the server will default to 127.0.0.1:9999. The Level ID
defaults to 0, which is a test world.  Worlds available are:
 * 0 = Test world
 * 1 = Room with crates
 * 2 = Series of rooms

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
 * Mouse movement: rotate camera
 * Mouse left click: fire projectile
 * space: move camera up
 * c: move camera down
 * t: initiate chat (during chat, keyboard commands are disabled)
 * enter (during chat): send message
 * backspace (during chat): remove last character added
 * escape: quit program or (during chat) exit chat
 
FEATURES
--------
 * Chat over network with usernames & chat history
 * Server selects spawn location for players
 * Player can move about freely in virtual world and fire projectiles using mouse and keyboard
 * Server correctly tracks connected/registered players and handles terminated connections
 * Server correctly tracks dynamic objects (shots/players)
   * Sends all currently active dynamic objects to players that join later
   * Dynamic objects must be created by the server (including all players and shots); clients request creation
   * Propagates player movements to other connected clients using AMI-style interface (like QUAKE)
   * Gracefully handles players leaving (either with a LEAVE command or when they kill process with interrupt).
 * Perspective-correct texturemapping, normal interpolation, z-buffering, and color interpolation
 * Support for different rendering types for objects
 * Extendable messaging format
 * Extendable rendering engine with support for advanced rendering techniques

TODO
----
 * Finish collision detection
 * Physics (for jumping)
 * Build world
 * Have server randomly choose spawn points for players.
 * Allow killing of characters and their subsequent respawn
 * Lighting system (smooth shading, shadowing, etc.)
 * Sprite system for particles/explosions/HUD
