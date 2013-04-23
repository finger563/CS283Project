#ifndef Camera_H
#define Camera_H

#include "point.h"
#include "matrix.h"

class Camera{

private:
	float theta,phi;
	Vector3D forward;
	Vector3D up;
	Vector3D right; 

	Vector3D position;

public:
	Camera();

	void Normalize();

	void ComputeAxes();

	void Translate(const Vector3D& v);

	void Rotate( const float _t, const float _p );

	Matrix GetWorldToCamera();

	float GetPhi() const { return phi; }
	float GetTheta() const { return theta; }
	void SetAngles( const float _t, const float _p );

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