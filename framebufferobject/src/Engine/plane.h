#ifndef	Plane_H
#define	Plane_H

#include "geometricobject.h"

class Plane : public GeometricObject
{
public:
	POINT3D a;
	VECTOR3D normal;
	Plane(){a=POINT3D();normal=VECTOR3D();}
	Plane(const POINT3D _a, const VECTOR3D _n) {a=_a;normal=_n;}
	~Plane(){}
	void Print(){}
	bool Hit(const Ray* r,const float t0,const float t1,Record* rec) const
	{
		float t = ( a - r->origin ) * normal / ( r->dir * normal );
		if ( t > t0 && t < t1)
		{
			if ( t <= rec->t )
			{
				rec->normal = normal;
				rec->t = t;
				rec->obj_hit = (void *)this;
			}
			return true;
		}
		return false;
	}
	Box BoundingBox() const
	{
		return Box();
	}
};

#endif