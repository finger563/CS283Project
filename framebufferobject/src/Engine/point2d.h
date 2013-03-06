#ifndef POINT2D_H
#define POINT2D_H

#include <math.h>

class POINT2D
{
public:
    float x,y;
    POINT2D() {x=y=0;}
    POINT2D(float _x,float _y)
    {
        x = _x;
        y = _y;
    }
    ~POINT2D(){}

    void Print(){}
    
    POINT2D operator -() const { return POINT2D(-x,-y); }

    POINT2D& operator= (const POINT2D& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        return (*this);   
    }
    
    POINT2D operator* (const float& rhs)
    {
        return POINT2D(x*rhs,y*rhs);   
    }
    
    POINT2D operator/ (const float& rhs)
    {
        return POINT2D(x/rhs,y/rhs);   
    }

    inline float operator* (const POINT2D& rhs)
    {
        return (x*rhs.x + y*rhs.y);
    }

    POINT2D operator+ (const POINT2D& rhs)
    {
        return POINT2D(x+rhs.x,y+rhs.y);
    }

    POINT2D operator- (const POINT2D& rhs)
    {
        return POINT2D(x-rhs.x,y-rhs.y);
    }
};

typedef POINT2D VECTOR2D;

inline static VECTOR2D normalize(VECTOR2D rhs)
{
  float mag = sqrt(rhs.x*rhs.x + rhs.y*rhs.y);

    return VECTOR2D(rhs.x/mag,rhs.y/mag);
}

inline static float magnitude(VECTOR2D rhs)
{
	return sqrt(rhs.x*rhs.x + rhs.y*rhs.y);
}

#endif