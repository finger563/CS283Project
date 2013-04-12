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

The networked part of the code has not been implemented yet, but it will most likely
use ACE/TAO for the networking library.  