#include "camera.h"
#include <math.h>
class Vector{
public:
float x, y, z;
//simple enough, these 3 values represent the vector <x,y,z>
public:
Vector(float xi, float yi, float zi):x(xi), y(yi), z(zi){}
Vector():x(0),y(0),z(1){}
//Just some constructors, self-explanatory
///////
Vector dot(Vector b){
return Vector(x*b.x, y*b.y, z*b.z);
}
//dot product. Not used in my camera class but it's in here cause i plan to add more to this class
Vector cross(Vector b){
return Vector(y*b.z-z*b.y, z*b.x-x*b.z, x*b.y-y*b.x);
}
//cross product. Important in finding a vector perpendicular to 2 others The next method is the important one
void rotate(float a, Vector b){
//this is for rotation about another vector. a = angle. b = the vector to rotate about
a *= 3.1415/180;
//convert angle to radians
float s = sin(a);
float c = cos(a);
float t = 1-cos(a);
//3 trig functions to simplify the code a bit so i dont have to type too much stuff and so it only needs to call each trig function once
double tX = x*(t*b.x*b.x+c) + y*(t*b.x*b.y-s*b.z) + z*(t*b.x*b.z+s*b.y);
double tY = x*(t*b.x*b.y+s*b.z) + y*(t*b.y*b.y+c) + z*(t*b.y*b.z-s*b.x);
double tZ = x*(t*b.x*b.z-s*b.y) + y*(t*b.y*b.z+s*b.x) + z*(t*b.z*b.z+c);
//this multiplies the current matrix be a rotation matrix (thanks to delta again for the rotation matrix). It stores the values in 3 functions (which really should be a new vector, but whatever it's simpler this way)
x = tX;
y = tY;
z = tZ;
//store the temporary values into this one's new values
}
};
//Well that's the math part. If you really want to understand the math (way too much to explain and stuff i dont quite know exactly why, just google "rotation matrices" and "how to multiply matrices" (treat the vector like a 1x3 matrix)
//Now for the camera part.
enum direction{
UP,
DOWN,
LEFT,
RIGHT,
FORWARD,
BACKWARD
};
//if you dont know what an enum is look it up
class Camera{
private:
Vector eye;
Vector up;
float x, y, z;
//all values needed for gluLookAt, the ones to pay attention to are the 2 vectors. x,y,z are just the position of the camera, but the vectors are where the meat is at. eye is the vector that specifys what direction the camera is facing. Up is a vector perpendicular to eye, that specifys the top of the camera, by altering it you could like spin the screen or something for an effect if you want.
float dist;
//just how far the visible range is. If it's 500, objects more than 500 pixels away wont be rendered on the screen
public:
Camera():x(0.00),y(0.0),z(0.0),dist(1000){
eye = *(new Vector(0.0, 0.0, -1.0));
up = *(new Vector(0.0, 1.0, 0.0));
}
//bleh constructor
void update(){
gluLookAt(x , y , z,
eye.x*dist, eye.y*dist, eye.z*dist,
up.x, up.y, up.z);
}
//gluLookAt. Puts the camera at x,y,z and makes it face eye and the tiop is up. Simple enough.
void rotate(direction dir, float deg){
//this is a function to rotate the camera using the vector's ROTATE method
if(dir==UP){
Vector temp = eye.cross(up);
//the temporary vector is perpendicular to both up and eye.
eye.rotate(deg, temp);
up.rotate(deg, temp);
//then we rotate up and eye
}
if(dir==DOWN){
Vector temp = eye.cross(up);
eye.rotate(-deg, temp);
up.rotate(-deg, temp);
}
//same here
if(dir==LEFT){
Vector temp = up;
//temporary vector is the same as up
eye.rotate(deg, temp);
up.rotate(deg, temp);
//we really dont need to rotate up, just eye. But i'm too lazy to delete it
}
if(dir==RIGHT){
Vector temp = up;
eye.rotate(-deg, temp);
up.rotate(-deg, temp);
}
//same here
}
void move(direction dir, float dis){
//this moves the camera forward or backward depending on it's current rotation. since the EYE vector has a length of 1 even after you rotate it, we just have to move X Y and Z by the eye vector * the speed. If you wanted, you could stick code for LEFT and RIGHT and move it based on the cross between up and eye too. Really simple if you think about it. UP and DOWN could be based off of the up vector even! The possibilities are endless
if(dir==FORWARD){
x += eye.x*dis;
y += eye.y*dis;
z += eye.z*dis;
}
if(dir==BACKWARD){
x -= eye.x*dis;
y -= eye.y*dis;
z -= eye.z*dis;
}
}
};