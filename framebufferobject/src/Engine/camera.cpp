#include "camera.h"
#include <math.h>

Camera::Camera():up(0, 1, 0), forward(0, 0, 1), right(1, 0, 0), position(0, 0, 0){

}

void Camera::Normalize() {
	up = normalize(up);
	forward = normalize(forward);
	right = normalize(right);
}

void Camera::Translate( const Vector3D& v) {
	position = right*(v.x) + up*(v.y) + forward*(v.z) + position; 
}

void Camera::Rotate (const Matrix& m) {
	 rotation = rotation*m;
	 
	 up = m * up;
	 forward = m * forward;
	 right = m * right;
	 Normalize();
}

void Camera::Rotate (const float x, const float y, const float z) {
	 Matrix m;
	 m.SetRotation(x, y, z);
	 rotation = m*rotation;

	 up = m * up;
	 forward = m * forward;
	 right = m * right;
	 Normalize();
}

void Camera::Rotate (const float x, const Vector3D& v) {
	 Matrix m;
	 m.SetRotation(x,v);
	 rotation = m*rotation;

	 up = m * up;
	 forward = m * forward;
	 right = m * right;
	 Normalize();
}

Matrix Camera::GetWorldToCamera() {
	return rotation.Transpose();
}

Matrix Camera::GetRotation() {
	return rotation;
}

void Camera::SetRotation(const Matrix& m) {
	 rotation = m;
}

Point3D Camera::GetPosition() {
	return position;
}

void Camera::SetPosition( const float x, const float y, const float z ) {
	position.x = x;
	position.y = y;
	position.z = z;
}

Point3D Camera::GetForward() {
	return forward;
}

void Camera::SetForward( const float x, const float y, const float z) {
	forward.x = x;
	forward.y = y;
	forward.z = z;
}

Point3D Camera::GetUp() {
	return up;
}

void Camera::SetUp( const float x, const float y, const float z) {
	up.x = x;
	up.y = y;
	up.z = z;
}

Point3D Camera::GetRight() {
	return right;
}

void Camera::SetRight( const float x, const float y, const float z) {
	right.x = x;
	right.y = y;
	right.z = z;
}
