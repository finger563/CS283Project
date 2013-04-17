CS283Project
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

The networked part of the code relies on ACE for the networking.  You can download ACE at
[download][acedl], and you need Perl which can can be downloaded [here][perldl]

[acedl]: http://download.dre.vanderbilt.edu
[perldl]: http://www.activestate.com

Follow these steps to install ACE (in Windows):
 
 * Uncompress ACE to C:\ACE_wrappers (or some other folder of your choice)
 * Define environment variable ACE_ROOT to be C:\ACE_wrappers (or whatever folder you chose)
 * Define environment variable MPC_ROOT to be %ACE_ROOT%\MPC
 * Update the path to be PATH=%PATH%;%ACE_ROOT%\bin;%ACE_ROOT%\lib
 * Create config.h in %ACE_ROOT%\ace directory, putting in the file:
   * #define ACE_HAS_STANDARD_CPP_LIBRARY 1
   * #include "ace/config-win32.h"
 * open command prompt, cd to %ACE_ROOT%\ace
 * run: mwc.pl -type vc10 ace.mwc
 * open the ACE_wrappers_vc10 project in the %ACE_ROOT% folder and build the solution.

