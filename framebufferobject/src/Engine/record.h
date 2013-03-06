#ifndef RECORD_H
#define RECORD_H

#include "point3d.h"
#include "color.h"
#include "light.h"

class Record
{
public:
  float t;
  Color kd,ka,ks,km;
  VECTOR3D normal;
  void *obj_hit;
  Record(){t=0;kd=ka=ks=km=Color();obj_hit=NULL;}
  ~Record(){}
  void Zero(){t=0;}
  void Set(const float _t){t=_t;}
  void Clear(){kd=ka=ks=km=Color();}
};

#endif