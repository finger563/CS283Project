#ifndef	Sphere_H
#define	Sphere_H

#include "geometricobject.h"

class Sphere : public GeometricObject
{
public:
  POINT3D center;
  float radius;

  Sphere(){center=POINT3D();radius=0;}
  Sphere(const POINT3D c,const float r) {center=c;radius=r;}
  ~Sphere(){}
  void Print(){}
  bool Hit(const Ray* r,const float t0,const float t1,Record* rec) const
  {
    float A,B,C;
    A = r->dir * r->dir;
    B = r->dir*(r->origin-center);
    C = ((r->origin-center)*(r->origin-center)-radius*radius) ;
    float desc = B*B - A*C;

    if (desc>0.0f)
    {
      float _t0 = ((-B)+sqrt(desc) )/(A);
      float _t1 = ((-B)-sqrt(desc) )/(A);
      if (_t0>_t1)
      {
          float tmp;
          tmp = _t1;
          _t1 = _t0;
          _t0 = tmp;
      }
      if ( _t0>=t0 && _t0<=t1)
      {
		if ( _t0<=rec->t )
		{
			rec->t = _t0;
			VECTOR3D normal = (r->origin + r->dir*_t0 - center)/radius;
			normal = normalize(normal);
			rec->normal = normal;
			rec->obj_hit = (void *)this;
		}
		return true;
      }
    }
    return false;
  }
  Box BoundingBox() const
  {
    VECTOR3D min = center - VECTOR3D(radius,radius,radius);
    VECTOR3D max = center + VECTOR3D(radius,radius,radius);
    return Box(min,max);
  }
};

#endif