#include "Polygon.h"

extern float z_buffer[SIZE_X*SIZE_Y];
extern short display_buffer[SIZE_X*SIZE_Y];

// General Transformation Methods, only operate on x,y,z,w of vertices
void Poly::Transform( const Matrix& _m ) {
	for (int i=0;i<numVertices;i++)
		v[i].Transform(_m);
	normal = _m*normal;
}

void Poly::Translate( const Vector3D& _v ) {
	for (int i=0;i<numVertices;i++)
		v[i].Translate(_v);
}

void Poly::Translate( const float _x, const float _y, const float _z ) {
	for (int i=0;i<numVertices;i++)
		v[i].Translate(_x,_y,_z);
}

// Pipeline Transformation Methods
void Poly::TransformToCamera( const Matrix& m ) {
}

void Poly::TransformToPerspective( const Matrix& m ) {
}

// Pipeline function methods
void Poly::Clip( ) {
}

// Rasterization Methods
void Poly::Rasterize( ) {
}