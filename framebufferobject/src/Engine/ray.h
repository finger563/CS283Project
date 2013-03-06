#ifndef	RAY_H
#define	RAY_H

#include "point3d.h"

class Ray
{
public:
	POINT3D origin;
	VECTOR3D dir;
	Ray(){origin = POINT3D();dir = VECTOR3D();}
	Ray(const POINT3D o, const VECTOR3D d){origin=o;dir=d;}
	~Ray(){}
	void Print(){}
	Ray& operator= (const Ray& rhs)
	{
		origin = rhs.origin;
		dir = rhs.dir;
		return (*this);   
	}
};

#endif