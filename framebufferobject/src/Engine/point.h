#ifndef Point_H
#define Point_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "constants.h"

class Point
{
public:
    float x,y,z;
    Point() {x=y=z=0;}
    Point(const float _x,const float _y,const float _z) { x = _x; y = _y; z = _z; }
    ~Point(){}

    void Print() {}
    
    Point operator -() const { return Point(-x,-y,-z); }

    Point& operator= (const Point& rhs);
    
    Point operator* (const float rhs) const;
    
    Point operator/ (const float rhs) const;

    float operator* (const Point& rhs) const;

    Point operator+ (const Point& rhs) const;

    Point operator- (const Point& rhs) const;
};

typedef Point Vector;

inline static Vector normalize(const Vector rhs);

inline static float magnitude(const Vector rhs);

inline static Vector crossProduct(const Vector a, const Vector b);

#endif