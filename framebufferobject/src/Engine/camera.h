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

public:
	Camera();

	void Normalize();

	void Translate(const Vector3D& v);

	void Rotate (const Matrix& m);

	void Rotate (const float x, const float y, const float z);

	void Rotate (const float x, const Vector3D& v);

	Matrix GetWorldToCamera();

	Matrix GetRotation();
	void SetRotation (const Matrix& m);

	Point3D GetPosition();
	void SetPosition(const float x, const float y, const float z);

	Point3D GetForward();
	void SetForward(const float x, const float y, const float z);
	
	Point3D GetUp();
	void SetUp(const float x, const float y, const float z);
	
	Point3D GetRight();
	void SetRight(const float x, const float y, const float z);
};
#endif