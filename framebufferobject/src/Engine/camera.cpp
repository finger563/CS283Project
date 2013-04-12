#include "camera.h"
#include <math.h>

Camera::Camera():up(0, 1, 0), forward(0, 0, 1), right(1, 0, 0), position(0, 0, 0){

}

void Camera::Normalize() {
	up = up/up.w;
	forward = forward/forward.w;
	right = right/right.w;
}

Point3D Camera::getPosition() {
	return position;
}

//sets position
void Camera::Translate( Vector3D v)
{
	position = right*(v.x) + up*(v.y) + forward*(v.z) + position; 
}

void Camera::setRotation(Matrix m) //will we be passing the rotation matrix?? or should I set a default matrix in the constructor?
{
	 rotation = m;
}

void Camera::Rotate (Matrix m) //will we be passing the rotation matrix?? or should I set a default matrix in the constructor?
{
	 rotation = m*rotation;
	 
	 //rotate each vector by rotation
	 up = m * up;
	 forward = m * forward;
	 right = m * right;
	 Normalize();
}

void Camera::Rotate (float x, float y, float z) 
{
	 Matrix m;
	 m.SetRotation(x, y, z);
	 rotation = rotation*m;

	 //rotate each vector by rotation
	 up = m * up;
	 forward = m * forward;
	 right = m * right;
	 Normalize();
}

void Camera::Rotate (float x, Vector3D v) 
{
	 Matrix m;
	 m.SetRotation(x,v);
	 rotation = rotation*m;

	 //rotate each vector by rotation
	 up = m * up;
	 forward = m * forward;
	 right = m * right;
	 Normalize();
}

Matrix Camera::getRotation()
{
	return rotation;
}

