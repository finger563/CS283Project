#ifndef CAMERA_H
#define	CAMERA_H

#include "point3d.h"
#include "ray.h"

class Eye
{
public:
  POINT3D e;
  VECTOR3D u,v,w;
  Eye(){e= POINT3D();u=v=w=VECTOR3D();}
  Eye(const POINT3D _e,const VECTOR3D _u,const VECTOR3D _v,const VECTOR3D _w){e=_e;u=_u;v=_v;w=_w;}
  ~Eye(){}
  void Print(){}
  Eye& operator = (const Eye& rhs)
  {
	  e = rhs.e;
	  u = rhs.u;
	  v = rhs.v;
	  w = rhs.w;
	  return (*this);
  }
};

#endif