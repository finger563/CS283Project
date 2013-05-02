CS283Project : Super Block Fighter Extreme
============

CS 283 Networked 3D Game Project

This project is written by William Emfinger and Paul McNeil for Vanderbilt's 
spring 2013 CS283 Computer Networks class. Though extensive, this project was completely coded
by the two of us during the length of this course. Completing it successfully required that we met
at least once a week from the beginning of the semester.

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

README Contents
---------------
* Overall Game Design
* Network System Design
* Graphics System Design
* Features
* TODO
* System Requirements
* Getting the project files
* Running the Server
* Running the game (client)
* Results

Overall Game Design
-------------------
The game is designed as an old-school fast paced first person shooter, in the likes of the original Quake.  Players can move and fire quickly, and the objective in the game is to frag (kill) the other players without getting fragged yourself.  There is a chat system which lets players communicate (trash-talk) each other, and the server lets all players know who kills whom.  

Network System Design
---------------------
The networking part of the system is designed such that players initiate a connection to the server using a REGISTER message.  If the server has slots available, it will send an ACCEPT message to the player, which contains the world ID the server is running, as well as the player's starting position, heading, and health (life).  The server keeps track of all players that are currently connected and dissemenates the currently active players 
to the joining player.  

Players can fire a shot by issuing a SHOOT message to the server.  The server will then send a CREATE object message which will create that shot on all players' clients.  Any shots which are currently active when a player joins a game will also be sent to the player's client.  The server handles the shot's collision detection, and therefore has the final (only) say in whether a player was hit by a shot or not.  

The server has a periodic callback routine which does shot collision detection against all players and all objects in the (static) world.  If a shot collides with an object, the shot is removed from the server's dynamic object list and a REMOVE message is sent to all players.  Moreover, if the object the shot collides with is a player, the server decrements the player's life.  If a player's life reaches 0, the server sends a CHAT message to all players, informing them that <player 1> was killed by <player 2>.  It then sends an ACCEPT message to that player again to reset them to a new spawn location and to reset their life meter.  Players can communicate with each other using CHAT messages, which the server does not keep track of, instead the server simply relays all chat messages to all other players.  If the shot does not collide with anything, the shot's life is decremented.  In this way, shots become inactive after a certain amount of time and can then be removed, so that the number of objects the server has to keep track of does not grow ad infinitum.  

Players peiodicially send MOVE commands to the server to update the server of their position and heading.  When the
server gets a MOVE message from a player, propagates the message to all players.  When players receive a MOVE message,
they do one of two things.  If the object to be moved corresponds to them (as indicated by the object's ID matching their ID and the object's TYPE being PLAYER), then they update their current health (since the server controls the player's health).  If the object does not correspond to them, then they simply update the corresponding object in their local dynamic object list with the new position and heading.  The types of objects which clients and the server track dynamically are SHOT and PLAYER.  

The period at which the players send MOVE messages to the server is determined by the RTT the players measure from 
sending a MOVE message to the server and getting a corresponding MOVE message back (with their ID and TYPE).  

If players exit their clients, they send LEAVE messages to the server to inform the server they are leaving the game.  When the server receives a LEAVE message, it sends a REMOVE object to all other players, informing them that the player has left the game.  Moreover, if a client force-quits their game (meaning a LEAVE message is not sent to the server), the server detects the closure of the client's connection and removes the player from the server's data structures and sends the REMOVE message to all other players.  

For reference, the message types are listed here:
 * REGISTER: Players REGISTER with the server initially
 * ACCEPT: Then the server responds using ACCEPT with a player ID, and the initial position/heading/life as well as world ID
 * CHAT: Players send CHAT messages with username and text to server, it relays the message to other players
 * SHOOT: Players send SHOOT command to Server, it replies/relays CREATE message to *all* players
 * CREATE: Server sends CREATE message when a new object is created by a player (shot, player, etc.)
 * MOVE: Server sends MOVE message when a created object needs to be moved (i.e. shot update, player update, etc.). Players send MOVE messages to the server when the player needs to move (i.e. keyboard/mouse input)
 * LEAVE: Player sends LEAVE message when they disconnect from the server
 * REMOVE: Then the server removes the player from it's list and sends REMOVE to remove the object from other players' games

Graphics System Design
----------------------
The graphics system is written from scratch and contains the complete graphics pipline as it would have existed in the days of Quake.  This means that many functions performed by modern graphics libraries and GPUs are implmented instead in the engine itself.  This makes the engine slower than it could have been had it leveraged those libraries and hardware, but it also means that the graphics engine is portable to any system with a screen and a C/C++ compiler, since the graphics code is running entirely on the CPU.

The way the graphics pipline works is that objects are composed of polygons (generally 3-point polygons, but the engine supports 4-point polygons for clipping).  The world contains many objects, each with their own position and heading with respect to the world's origin.  The player has a camera which allows them to view the world.  This camera has its own position and heading (which defines its own coordinate axes).  

To view an object on the screen, the objet must first be translated and rotated from its own (object-space) coordinates, which have the center of the object at the origin, to the object's world-space position and heading.  After all objects in the world have had this transformation applied to them, they must then be translated and rotated by the player's camera's inverse translation and rotation.  This transformation centers the camera at the origin, facing in the positive z-axis.  The object is then back-face culled, so that it will return a render list of only polygons which face the camera (i.e. the dot-product of their normal with the view vector is greater than 0).  This render list of polygons is then perspective transformed (divide by z) to correctly perspective project the polygon onto the 2D viewplane from the 3D camera view-space.  These polygons are then clipped against the x-y plane (z=0 plane), and they are transformed to pixel space.  Finally, the polygons are rasterized to the display buffer by the polygon rasterization part of the engine.  

Throughout this process, homogeneous coordinates are used (i.e. 4-D coordinates are used to represent a 3-D point in space).  This allows matrix tranformations to represent translation and projection, not just rotation.  Moreover, the homogeneous coordinate system allows us to properly interpolate polygon vertex attributes accross the polygon as we rasterize it.  Example interpolants are: 
 * the vertices' camera-space z-coordinate for z-buffering
 * vertex color values
 * vertex texture coordinates
 * vertex normal vectors (for smooth shading and height displacement)

To keep the engine as portable as possible (i.e. to reduce the memory footprint at the cost of speed), the engine rasterizes one line at a time, as opposed to the full display buffer at once.  This allows it to be ported to devices which have nice screens, but not enough memory for both a full display buffer and a full z-buffer. 

Here is an overview of the engine with the steps that an object goes through on the pipline:
 * Object to World space transformation
 * World to Camera space transformation
 * Camera to Perspective space transformation
 * Back-face culling on each object  ( at this point each object returns a list of visible polygons )
 * Polygon clipping on the complete visible list
 * Homogeneous coordinate division
 * View-space to Pixel-space transformation (translation and scaling)
 * Rasterization

FEATURES
--------
 * Chat over network with usernames & chat history
 * Server selects spawn location for players, as well as world that is loaded
 * Player can move about freely in virtual world and fire projectiles using mouse and keyboard
   * Players measure RTT for their movement messages and meter their updates to the server based on this time.
 * Server correctly tracks connected/registered players and handles terminated connections
   * Players can be killed by other players' shots (or their own shots) and will respawn upon death.
   * All players see <Player X> was killed by <Player Y> in their chat window.  
 * Server correctly tracks dynamic objects (shots/players)
   * Sends all currently active dynamic objects to players that join later
   * Dynamic objects must be created by the server (including all players and shots); clients request creation
   * Propagates player movements to other connected clients using AMI-style interface (like QUAKE)
   * Gracefully handles players leaving (either with a LEAVE command or when they kill process with interrupt).
   * Handles collision detection for shot-world and shot-player collisions
 * Perspective-correct texturemapping, normal interpolation, z-buffering, and color interpolation
 * Support for different rendering types for objects
 * Extendable messaging format
 * Extendable rendering engine with support for advanced rendering techniques

TODO
----
 * Physics (for jumping) : on client side.
 * Implement restricted player movement (player-world collision detection) : client side
 * Finish world 1 implementation.  Work on World 2 implementation
 * Have server randomly choose spawn points for players.
 * Lighting system (smooth shading, shadowing, etc.)
 * Sprite system for particles/explosions/HUD

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
 * Define environment variables

		ACE_ROOT = C:\ACE_wrappers (or whatever folder you chose)
		MPC_ROOT = %ACE_ROOT%\MPC

 * Update the path to be 

		PATH=%PATH%;%ACE_ROOT%\bin;%ACE_ROOT%\lib
		
 * Create config.h in %ACE_ROOT%\ace directory, putting in the file: 
	
		#define ACE_HAS_STANDARD_CPP_LIBRARY 1 
		#include "ace/config-win32.h"
	
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

Methods
-------------------
The program was tested on two tiers: localized testing and intranet testing.

Localized testing occurred often throughout the development of the program. Each time a new feature was added, 
it was integrated into the system to ensure continuity. These changes were tested on at least two different systems. 
Following the localized testing, intranet testing occurred to evaluate the messaging protocol and state synchronization.

Intranet testing occurred on the ISIS and Featheringhill intranets. It consisted of running the program on two independent systems.
The systems would then join to test the functionality of each game feature. Special attention was paid to the chat,
player and object movements, and players entrance/exit from a game.

Results
-------------------
Localized testing served to eliminate many client-side inconsistencies in the code. Issues such as visual glitches and 
rotation limitations were corrected to these tests. Localized state synchronization testing revealed latencies as well
as messaging errors. As a result of repetitive testing, the game properly implements all of the client-size functionality 

Intranet testing served to eliminate the remaining errors that could not be tested locally such as router delays.
Testing on the ISIS network was unsuccessful due to its firewall and other security protocols. However, testing on the 
Featheringill network was successful. Player and object movements synchronized reasonible across the intranet as did 
inter-player messaging. The program was tested on two tiers: localized testing and intranet testing.

Localized testing occurred often throughout the development of the program. Each time a new feature was added, 
it was integrated into the system to ensure continuity. These changes were tested on at least two different systems. 
Following the localized testing, intranet testing occurred to evaluate the messaging protocol and state synchronization.

Intranet testing occurred on the ISIS and Featheringhill intranets. It consisted of running the program on two independent systems.
The systems would then join to test the functionality of each game feature. Special attention was paid to the chat,
player and object movements, and players entrance/exit from a game.

Results
-------------------
Localized testing served to eliminate many client-side inconsistencies in the code. Issues such as visual glitches and 
rotation limitations were corrected to these tests. Localized state synchronization testing revealed latencies as well
as messaging errors. As a result of repetitive testing, the game properly implements all of the client-size functionality 

Intranet testing served to eliminate the remaining errors that could not be tested locally such as router delays.
Testing on the ISIS network was unsuccessful due to its firewall and other security protocols. However, testing on the 
Featheringill network was successful. Player and object movements synchronized reasonable across the intranet as did 
inter-player messaging.

