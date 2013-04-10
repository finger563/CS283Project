#include "Vertex.h"

Vertex& Vertex::operator= (const Vertex& rhs) 
{
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
	w = rhs.w;
    return (*this);   
}

Vertex Vertex::operator* (const float rhs) const 
{
    return Vertex(x*rhs,y*rhs,z*rhs,w*rhs);   
}

Vertex Vertex::operator/ (const float rhs) const 
{
    return Vertex(x/rhs,y/rhs,z/rhs,w/rhs);   
}

float Vertex::operator* (const Vertex& rhs) const 
{
    return (x*rhs.x + y*rhs.y + z*rhs.z);
}

Vertex Vertex::operator+ (const Vertex& rhs) const 
{
    return Vertex(x+rhs.x,y+rhs.y,z+rhs.z);
}

Vertex Vertex::operator- (const Vertex& rhs) const 
{
    return Vertex(x-rhs.x,y-rhs.y,z-rhs.z);
}