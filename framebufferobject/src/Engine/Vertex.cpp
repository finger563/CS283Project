#include "Vertex.h"

// General Transformation Methods, only operate on x,y,z,w
void Vertex::Transform( const Matrix& _m ) {
	Vector3D v = Vector3D(x,y,z,w);
	v = _m*v;
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
}

void Vertex::Translate( const Vector3D& _v ) {
	x += _v.x;
	y += _v.y;
	z += _v.z;
}

void Vertex::Translate( const float _x, const float _y, const float _z ) {
	x += _x;
	y += _y;
	z += _z;
}

// Pipeline Transformation Methods
void Vertex::TransformToCamera( const Matrix& _m ) {
	Vector3D v = Vector3D(x,y,z,w);
	v = _m*v;
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;

	ex = x;
	ey = y;
	ez = z;

	hw = 1;
}

void Vertex::TransformToPerspective( const Matrix& _m ) {
	Vector3D v = Vector3D(x,y,z,w);
	v = _m*v;
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
}

void Vertex::HomogeneousDivide( ) {
	for (int i=0;i<NUM_VERTEX_DATA;i++)
		data[i] = data[i]/w;
}

// Operator Overloads
Vertex& Vertex::operator= (const Vertex& rhs) 
{
	for (int i=0;i<NUM_VERTEX_DATA;i++)
		data[i] = rhs[i];
    return (*this);   
}

bool Vertex::operator== (const Vertex& rhs) const {
	for (int i=0;i<NUM_VERTEX_DATA;i++){
		if ( data[i] != rhs[i] )
			return false;
	}
	return true;
}

Vertex Vertex::operator- (const Vertex& rhs) const{
	Vertex v;
	for (int i=0;i<NUM_VERTEX_DATA;i++)
		v[i] = data[i] - rhs[i];
	return v;
}

Vertex Vertex::operator+ (const Vertex& rhs) const {
	Vertex v;
	for (int i=0;i<NUM_VERTEX_DATA;i++)
		v[i] = data[i] + rhs[i];
	return v;
}

Vertex Vertex::operator* (const float rhs) const {
	Vertex v;
	for (int i=0;i<NUM_VERTEX_DATA;i++)
		v[i] = data[i]*rhs;
	return v;
}