#include "Point.h"

Point& Point::operator= (const Point& rhs) 
{
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    return (*this);   
}

Point Point::operator* (const float rhs) const 
{
    return Point(x*rhs,y*rhs,z*rhs);   
}

Point Point::operator/ (const float rhs) const 
{
    return Point(x/rhs,y/rhs,z/rhs);   
}

float Point::operator* (const Point& rhs) const 
{
    return (x*rhs.x + y*rhs.y + z*rhs.z);
}

Point Point::operator+ (const Point& rhs) const 
{
    return Point(x+rhs.x,y+rhs.y,z+rhs.z);
}

Point Point::operator- (const Point& rhs) const 
{
    return Point(x-rhs.x,y-rhs.y,z-rhs.z);
}

inline static Vector normalize(const Vector rhs)
{
  float mag = sqrt(rhs.x*rhs.x + rhs.y*rhs.y + rhs.z*rhs.z);

    return Vector(rhs.x/mag,rhs.y/mag,rhs.z/mag);
}

inline static float magnitude(const Vector rhs)
{
	return sqrt(rhs.x*rhs.x + rhs.y*rhs.y + rhs.z*rhs.z);
}

inline static Vector crossProduct(const Vector a, const Vector b)
{
	return Vector( (a.y*b.z-a.z*b.y) , (a.z*b.x-a.x*b.z) , (a.x*b.y-a.y*b.x) );
}
