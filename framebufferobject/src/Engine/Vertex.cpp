#include "Vertex.h"

// General Transformation Methods, only operate on x,y,z,w
void Vertex::Transform( const Matrix& m ) {
	Vector3D v = Vector3D(x,y,z,w);
	v = m*v;
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
}

void Vertex::Translate( const Vector3D& v ) {
	x += v.x;
	y += v.y;
	z += v.z;
}

void Vertex::Translate( const float _x, const float _y, const float _z ) {
	x += _x;
	y += _y;
	z += _z;
}

// Operator Overloads
Vertex& Vertex::operator= (const Vertex& rhs) 
{
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
	w = rhs.w;
	ex = rhs.ex;
	ey = rhs.ey;
	ez = rhs.ez;
	u = rhs.u;
	v = rhs.v;
	r = rhs.r;
	g = rhs.g;
	b = rhs.b;
	nx = rhs.nx;
	ny = rhs.ny;
	nz = rhs.nz;
	hw = rhs.hw;
    return (*this);   
}

bool Vertex::operator== (const Vertex& rhs) const {
	for (int i=0;i<NUM_VERTEX_DATA;i++){
		if ( data[i] != rhs[i] )
			return false;
	}
	return true;
}