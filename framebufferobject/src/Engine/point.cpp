#include "Point.h"

Point3D& Point3D::operator= (const Point3D& rhs) 
{
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    return (*this);   
}

Point3D Point3D::operator* (const float rhs) const 
{
    return Point3D(x*rhs,y*rhs,z*rhs);   
}

Point3D Point3D::operator/ (const float rhs) const 
{
    return Point3D(x/rhs,y/rhs,z/rhs);   
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

Point2D& Point2D::operator= (const Point2D& rhs) 
{
    x = rhs.x;
    y = rhs.y;
    return (*this);   
}

Point2D Point2D::operator* (const float rhs) const 
{
    return Point2D(x*rhs,y*rhs);   
}

Point2D Point2D::operator/ (const float rhs) const 
{
    return Point2D(x/rhs,y/rhs);   
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

