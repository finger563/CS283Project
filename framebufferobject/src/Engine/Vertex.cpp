#include "Vertex.h"

length& length::operator= (const length& rhs) 
{
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
	w = rhs.w;
    return (*this);   
}

length length::operator* (const float rhs) const 
{
    return length(x*rhs,y*rhs,z*rhs,w*rhs);   
}

length length::operator/ (const float rhs) const 
{
    return length(x/rhs,y/rhs,z/rhs,w/rhs);   
}

float length::operator* (const length& rhs) const 
{
    return (x*rhs.x + y*rhs.y + z*rhs.z);
}

length length::operator+ (const length& rhs) const 
{
    return length(x+rhs.x,y+rhs.y,z+rhs.z);
}

length length::operator- (const length& rhs) const 
{
    return length(x-rhs.x,y-rhs.y,z-rhs.z);
}