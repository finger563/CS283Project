#include "Polygon.h"

extern float z_buffer[SIZE_X*SIZE_Y];
extern short display_buffer[SIZE_X*SIZE_Y];

// General Transformation Methods, only operate on x,y,z,w of vertices
void Poly::Transform( const Matrix& m ) {
	v1.Transform(m);
	v2.Transform(m);
	v3.Transform(m);
	v4.Transform(m);
	normal = m*normal;
}

void Poly::Translate( const Vector3D& v ) {
	v1.Translate(v);
	v2.Translate(v);
	v3.Translate(v);
	v4.Translate(v);
}

void Poly::Translate( const float x, const float y, const float z ) {
	v1.Translate(x,y,z);
	v2.Translate(x,y,z);
	v3.Translate(x,y,z);
	v4.Translate(x,y,z);
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