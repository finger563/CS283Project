#ifndef Vertex_H
#define Vertex_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "..\main.h"
#include "matrix.h"

#define NUM_VERTEX_DATA	16

class Vertex
{
public:
	union {
		struct {
			float x,y,z,w;		// 3D homogeneous coords
			float ex,ey,ez;		// 3D homogenous eye/camera coords ( w for this system is h1 )
			float u,v;			// 2D texture coords ( w for this system is h1 )
			float r,g,b;		// RGB color (range = [0,1]), ( w for this system is h1 )
			float nx,ny,nz;		// components of normal vector ( w for this system is h1 )
			float hw;			// homogeneous coordinate for interpolated values ( 1/~Pw )
		};
		float data[NUM_VERTEX_DATA];
	};

    Vertex() {
		for (int i=0;i<NUM_VERTEX_DATA;i++)
			data[i] = 0;
		w=hw=1;
	}
    Vertex(const float _x,const float _y,const float _z, const float _w = 1) { 
		x = _x; y = _y; z = _z; w = _w; 
		ex=ey=ez=0;
		u=v=0;
		r=g=b=0;
		nx=ny=nz=0;
		hw=_w;
	}
    ~Vertex(){}

    void Print() {}

	// Variable Setting Methods
	void SetColor(const float _r, const float _g, const float _b) { r=_r;g=_g;b=_b; }
	void SetTextureCoords(const float _u, const float _v) { u=_u;v=_v; }
	void SetEyeCoords(const float _x, const float _y, const float _z) { ex=_x;ey=_y;ez=_z; }
	void SetNormalCoords(const float _x, const float _y, const float _z) { nx=_x;ny=_y;nz=_z; }
	
	// General Transformation Methods, only operate on x,y,z,w
	void Transform( const Matrix& _m );
    void Translate( const Vector3D& _v );
    void Translate( const float _x, const float _y, const float _z );
	
	// Pipeline Transformation Methods
    void TransformToCamera( const Matrix& _m );
    void TransformToPerspective( const Matrix& _m );
	void HomogeneousDivide( );

	// Operator Overloads
	Vertex& operator= (const Vertex& rhs);
	bool operator== (const Vertex& rhs) const;
	float& operator[](const int i) { return data[i]; }
	float operator[](const int i) const { return data[i]; }
};

#endif	// Vertex_H