#ifndef	BOX_H
#define BOX_H

class Box
{
public:
  POINT3D p1,p2;
  Box(){p1=p2=POINT3D();}
  Box(const POINT3D _p1,const POINT3D _p2)
  {
    p1 = _p1;
    p2 = _p2;
  }
  ~Box(){}
};

#endif