#ifndef	LIGHT_H
#define	LIGHT_H

#include "color.h"
#include "point3d.h"

class Light
{
public:
  Color lColor;
  POINT3D pos;
  float intensity;
  Light* next;
  Light(){lColor=Color();pos=POINT3D();intensity=0;next=NULL;}
  Light(const Color _color,const POINT3D _position,const float i){lColor=_color;pos=_position;intensity=i;next=NULL;}
  ~Light(){}

  Light& operator= (const Light& rhs)
  {
	  lColor	= rhs.lColor;
	  pos		= rhs.pos;
	  intensity = rhs.intensity;
          next          = rhs.next;
	  return (*this);
  }
};

#endif