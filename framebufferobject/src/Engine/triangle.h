#ifndef	Triangle_H
#define	Triangle_H

#include "matrix.h"

enum TriangleType { // left/right directions w.r.t s2
    FLAT_TOP_RIGHT,
    FLAT_TOP_LEFT,
    FLAT_BOTTOM_RIGHT,
    FLAT_BOTTOM_LEFT,
    NORMAL_RIGHT,
    NORMAL_LEFT
};

class Triangle
{
public:
    Point a,b,c;        // object coordinates for triangle
    Vector normal;
    short color;
    TriangleType type;  // for fast rendering, pre-compute

    // Screen space variables
    Point s1,s2,s3,s4;      // screen space coordinates for triangle (y-sorted)
    float m12,m13,m23;      // screen space slopes (dx/dy)
    float sx,ex;            // start and end x-pixel coordinates
	float sz,ez;			// start and end 1/z world coordinates of object (per scanline)
    float dzx,dz13,dz12,dz23,dz14,dz43;    // z-slopes for 1/z-buffering

    Triangle(){a=b=c=Point();normal=Vector();}
    Triangle(const Point _a,const Point _b,const Point _c, const Vector n, short clr) {
        a=_a;b=_b;c=_c;normal=n;color=clr;
    }
    ~Triangle(){}
    void Print(){}

    void Rotate( const Matrix& m );
    void Translate( const Vector& v);

    void TransformToScreen( const Matrix& m);
    void DrawFilledZbuffer( const int y );
    void DrawWireframe ( const Matrix& m );
    void DrawFilled ( void );
};

#endif