#ifndef Vertex_H
#define Vertex_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "..\main.h"
#include "matrix.h"

class Vertex
{
public:
    float x,y,z,w;		// 3D homogeneous coords
	float ex,ey,ez,ew;	// 3D homogenous eye/camera coords
	int u,v;			// 2D texture coords
	float r,g,b;		// RGB color (range = [0,1])
	float nx,ny,nz,nw;	// components of normal vector


    Vertex() {
		x=y=z=0;w=1;
		ex=ey=ez=0;ew=1;
		u=v=0;
		r=g=b=0;
		nx=ny=nz=0;nw=1;
	}
    Vertex(const float _x,const float _y,const float _z) { 
		x = _x; y = _y; z = _z; w = 1; 
		ex=ey=ez=0;ew=1;
		u=v=0;
		r=g=b=0;
		nx=ny=nz=0;nw=1;
	}
    Vertex(const float _x,const float _y,const float _z,const float _w) { 
		x = _x; y = _y; z = _z; w = _w; 
		ex=ey=ez=0;ew=1;
		u=v=0;
		r=g=b=0;
		nx=ny=nz=0;nw=1;
	}
    ~Vertex(){}

    void Print() {}
    
    Vertex operator -() const { return Vertex(-x,-y,-z,w); }

    Vertex& operator= (const Vertex& rhs);
    
    Vertex operator* (const float rhs) const;
    
    Vertex operator/ (const float rhs) const;

    float operator* (const Vertex& rhs) const;

    Vertex operator+ (const Vertex& rhs) const;

    Vertex operator- (const Vertex& rhs) const;
};

#endif	// Vertex_H