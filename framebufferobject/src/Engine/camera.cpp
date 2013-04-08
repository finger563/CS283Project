#include "camera.h"
#include <math.h>

Camera::Camera():up(0, 1, 0), forward(0, 0, 1), right(1, 0, 0), position(0, 0, 0){

}

//sets position
void Camera::Translate( Vector3D v)
{
	position = right*(v.x) + up*(v.y) + forward*(v.z) + position; 
}

void Camera::Rotate (Matrix m) //will we be passing the rotation matrix?? or should I set a default matrix in the constructor?
{
	 m.SetRotation(position.x, position.y, position.z);
	 rotation = m;
}

Matrix Camera::getRotation()
{
	return rotation;

}

