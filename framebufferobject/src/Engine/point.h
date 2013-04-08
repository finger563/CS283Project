#ifndef Point_H
#define Point_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "constants.h"
#include "..\main.h"

class Point3D
{
public:
    float x,y,z;	// 3D coords of point
    Point3D() {x=y=z=0;}
    Point3D(const float _x,const float _y,const float _z) { x = _x; y = _y; z = _z; }
    ~Point3D(){}

    void Print() {}
    
    Point3D operator -() const { return Point3D(-x,-y,-z); }

    Point3D& operator= (const Point3D& rhs);
    
    Point3D operator* (const float rhs) const;
    
    Point3D operator/ (const float rhs) const;

    float operator* (const Point3D& rhs) const;

    Point3D operator+ (const Point3D& rhs) const;

    Point3D operator- (const Point3D& rhs) const;
};

typedef Point3D Vector3D;

inline static Vector3D normalize(const Vector3D rhs)
{
  float mag = sqrt(rhs.x*rhs.x + rhs.y*rhs.y + rhs.z*rhs.z);

    return Vector3D(rhs.x/mag,rhs.y/mag,rhs.z/mag);
}

inline static float magnitude(const Vector3D rhs)
{
	return sqrt(rhs.x*rhs.x + rhs.y*rhs.y + rhs.z*rhs.z);
}

inline static Vector3D crossProduct(const Vector3D a, const Vector3D b)
{
	return Vector3D( (a.y*b.z-a.z*b.y) , (a.z*b.x-a.x*b.z) , (a.x*b.y-a.y*b.x) );
}

class Point2D
{
public:
    float x,y;	// 2D coords of point
    Point2D() {x=y=0;}
    Point2D(const float _x,const float _y) { x = _x; y = _y; }
    ~Point2D(){}

    void Print() {}
    
    Point2D operator -() const { return Point2D(-x,-y); }

    Point2D& operator= (const Point2D& rhs);
    
    Point2D operator* (const float rhs) const;
    
    Point2D operator/ (const float rhs) const;

    float operator* (const Point2D& rhs) const;

    Point2D operator+ (const Point2D& rhs) const;

    Point2D operator- (const Point2D& rhs) const;
};

typedef Point2D Vector2D;

inline static float magnitude(const Vector2D rhs)
{
	return sqrt(rhs.x*rhs.x + rhs.y*rhs.y);
}

#endif