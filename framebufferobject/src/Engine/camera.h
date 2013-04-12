#ifndef Camera_H
#define Camera_H

#include "point.h"
#include "matrix.h"

class Camera{

private:
 Vector3D forward;
 Vector3D up;
 Vector3D right; 
 Vector3D position;

 Matrix rotation; 

 
//Up is a vector perpendicular to eye, that specifys the top of the camera, 
//by altering it you could like spin the screen or something for an effect if you want.
 
//float x, y, z; //x,y,z are just the position of the camera,
//all values needed for gluLookAt, the ones to pay attention to are the 2 vectors. but the vectors are where the meat is at.  

//just how far the visible range is. If it's 500, objects more than 500 pixels away wont be rendered on the screen
//may help our math problem??
//float dist;

public:

//bleh constructor
Camera();

void Normalize();

void Translate(Vector3D v);

void setRotation (Matrix m);

void Rotate (Matrix m);

void Rotate (float x, float y, float z);

void Rotate (float x, Vector3D v);

Matrix getRotation();

Point3D getPosition();

Point3D getForward();



////this is a function to rotate the camera using the vector's ROTATE method
//void rotate(direction dir, float deg){
//if(dir==UP){
//
////the temporary vector is perpendicular to both up and eye.
//Vector temp = eye.cross(up);
//
//eye.rotate(deg, temp);
//up.rotate(deg, temp);
////then we rotate up and eye
//}
//if(dir==DOWN){
//Vector temp = eye.cross(up);
//eye.rotate(-deg, temp);
//up.rotate(-deg, temp);
//}
////same here
//if(dir==LEFT){
//Vector temp = up;
////temporary vector is the same as up
//eye.rotate(deg, temp);
//up.rotate(deg, temp);
////we really dont need to rotate up, just eye. But i'm too lazy to delete it
//}
//if(dir==RIGHT){
//Vector temp = up;
//eye.rotate(-deg, temp);
//up.rotate(-deg, temp);
//}
////same here
//}
//void move(direction dir, float dis){
////this moves the camera forward or backward depending on it's current rotation. 
////since the EYE vector has a length of 1 even after you rotate it, 
////we just have to move X Y and Z by the eye vector * the speed. 
////If you wanted, you could stick code for LEFT and RIGHT and move it based on the cross between up and eye too. 
////Really simple if you think about it. UP and DOWN could be based off of the up vector even! The possibilities are endless
//
//if(dir==FORWARD){
//x += eye.x*dis;
//y += eye.y*dis;
//z += eye.z*dis;
//}
//if(dir==BACKWARD){
//x -= eye.x*dis;
//y -= eye.y*dis;
//z -= eye.z*dis;
//}
//}
};
#endif