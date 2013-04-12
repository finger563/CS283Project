#include "Point.h"

Point3D& Point3D::operator= (const Point3D& rhs) 
{
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
	w = rhs.w;
    return (*this);   
}

Point3D Point3D::operator* (const float rhs) const 
{
    return Point3D(x*rhs,y*rhs,z*rhs,w*rhs);   
}

Point3D Point3D::operator/ (const float rhs) const 
{
    return Point3D(x/rhs,y/rhs,z/rhs,w/rhs);   
}

float Point3D::operator* (const Point3D& rhs) const 
{
    return (x*rhs.x + y*rhs.y + z*rhs.z);
}

Point3D Point3D::operator+ (const Point3D& rhs) const 
{
    return Point3D(x+rhs.x,y+rhs.y,z+rhs.z);
}

Point3D Point3D::operator- (const Point3D& rhs) const 
{
    return Point3D(x-rhs.x,y-rhs.y,z-rhs.z);
}

// POINT2D

Point2D& Point2D::operator= (const Point2D& rhs) 
{
    x = rhs.x;
    y = rhs.y;
	w = rhs.w;
    return (*this);   
}

Point2D Point2D::operator* (const float rhs) const 
{
    return Point2D(x*rhs,y*rhs,w*rhs);   
}

Point2D Point2D::operator/ (const float rhs) const 
{
    return Point2D(x/rhs,y/rhs,w/rhs);   
}

float Point2D::operator* (const Point2D& rhs) const 
{
    return (x*rhs.x + y*rhs.y);
}

Point2D Point2D::operator+ (const Point2D& rhs) const 
{
    return Point2D(x+rhs.x,y+rhs.y);
}

Point2D Point2D::operator- (const Point2D& rhs) const 
{
    return Point2D(x-rhs.x,y-rhs.y);
}

