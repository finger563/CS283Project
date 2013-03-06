#ifndef	Color_H
#define	Color_H

#include <math.h>

class Color
{
public:
  float r,g,b,a;
  Color(){r=0;g=0;b=0;a=0;}
  Color(const float _r,const float _g,const float _b,const float _a){r=_r;g=_g;b=_b;a=_a;}
  ~Color(){}
  void Clear(){r=g=b=a=0;}
  
  Color& operator= (const Color& rhs)
  {
      r = rhs.r;
      b = rhs.b;
      g = rhs.g;
      a = rhs.a;
      return (*this);   
  }
  Color operator* (const Color& rhs) const 
  {
      float _r = r*rhs.r;
      float _b = b*rhs.b;
      float _g = g*rhs.g;
      float _a = a*rhs.a;
      return Color(_r,_g,_b,_a);
  }
  Color operator* (const float& rhs) const 
  {
      float _r = r*rhs;
      float _b = b*rhs;
      float _g = g*rhs;
      float _a = a*rhs;
      return Color(_r,_g,_b,_a);
  }
  Color operator/ (const float& rhs) const 
  {
      float _r = r/rhs;
      float _b = b/rhs;
      float _g = g/rhs;
      float _a = a/rhs;
      return Color(_r,_g,_b,_a);
  }
  Color operator+ (const Color& rhs) const 
  {
      float _r = r+rhs.r;
      float _b = b+rhs.b;
      float _g = g+rhs.g;
      float _a = a+rhs.a;
      return Color(_r,_g,_b,_a);
  }
  Color operator- (const Color& rhs) const 
  {
      float _r = r-rhs.r;
	  if (_r<0.0f) _r = 0.0f;
      float _b = b-rhs.b;
	  if (_b<0.0f) _b = 0.0f;
      float _g = g-rhs.g;
	  if (_g<0.0f) _g = 0.0f;
      float _a = a-rhs.a;
	  if (_a<0.0f) _a = 0.0f;
      return Color(_r,_g,_b,_a);
  }
  Color& operator+= (const Color& rhs) 
  {
      r = r+rhs.r;
      b = b+rhs.b;
      g = g+rhs.g;
      a = a+rhs.a;
      return *this;
  }
  Color& operator*= (const float& rhs)
  {
      r = r*rhs;
      b = b*rhs;
      g = g*rhs;
      a = a*rhs;
      return *this;
  }
  Color& operator/= (const float& rhs)
  {
      r = r/rhs;
      b = b/rhs;
      g = g/rhs;
      a = a/rhs;
      return *this;
  }
  bool operator> (const Color&rhs) const 
  {
  }
  bool operator< (const Color&rhs) const 
  {
  }
  bool operator== (const Color&rhs) const 
  {
	  if (r==rhs.r&&g==rhs.g&&b==rhs.b)
		  return true;
	  return false;
  }
  bool operator== (float rhs) const 
  {
	  if (r==rhs&&g==rhs&&b==rhs)
		  return true;
	  return false;
  }
  bool operator!= (float rhs) const 
  {
	  if (r!=rhs||g!=rhs||b!=rhs)
		  return true;
	  return false;
  }

  Color powc(float gamma) const 
  {
	  return Color(pow(r,gamma),pow(g,gamma),pow(b,gamma),pow(a,gamma));
  }
};

#endif