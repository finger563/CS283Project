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
void Poly::TransformToCamera( const Matrix& _m ) {
	for (int i=0;i<numVertices;i++)
		v[i].TransformToCamera(_m);
}

void Poly::TransformToPerspective( const Matrix& _m ) {
	for (int i=0;i<numVertices;i++)
		v[i].TransformToPerspective(_m);
}

// Pipeline function methods
void Poly::Clip( ) {
	// Can simplify this into the operations instead of
	// needing vectors
	Vector3D BC1 = Vector3D(1,0,0,0);	// X = 0, x
	Vector3D BC2 = Vector3D(-1,0,0,1);	// X = 1, w - x
	Vector3D BC3 = Vector3D(0,1,0,0);	// Y = 0, y
	Vector3D BC4 = Vector3D(0,-1,0,1);	// Y = 1, w - y
	Vector3D BC5 = Vector3D(0,0,1,0);	// Z = 0, z
	Vector3D BC6 = Vector3D(0,0,-1,1);	// Z = 1, w - z
}

void Poly::HomogeneousDivide( ) {
	for (int i=0;i<numVertices;i++)
		v[i].HomogeneousDivide();
}

// Rasterization Methods
void Poly::Rasterize( ) {
	YSort();	// sort the vertices by Y (screen-space)
	if ( numVertices == 3 ) {

	}
	else {
	}
}

void Poly::Rasterize( const int y ) {
	YSort();	// sort the vertices by Y (screen-space)
	if ( numVertices == 3 ) {
	}
	else {
	}
}

// Helper Functions
void Poly::YSort() {
	Vertex temp[POLY_MAX_VERTICES];
	temp[0] = v[0];
	temp[1] = v[1];
	temp[2] = v[2];
	temp[3] = v[3];
	if ( numVertices == 3 ) {
		if ( v[0].y < v[1].y ) {
			temp[0] = v[1];
			temp[1] = v[0];
		}
		if ( temp[0].y < v[2].y ) {	// v[2] has largest y
			temp[2] = temp[1];
			temp[1] = temp[0];
			temp[0] = v[2];
		}
		else if ( temp[1].y < v[2].y ) {	// v[2] has 2nd largest y
			temp[2] = temp[1];
			temp[1] = v[2];
		}
	}
	else {	// numVertices must equal 4
		if ( v[0].y < v[1].y ) {
			temp[0] = v[1];
			temp[1] = v[0];
		}
		if ( temp[0].y < v[2].y ) {	// v[2] has largest y
			temp[2] = temp[1];
			temp[1] = temp[0];
			temp[0] = v[2];
		}
		else if ( temp[1].y < v[2].y ) {	// v[2] has 2nd largest y
			temp[2] = temp[1];
			temp[1] = v[2];
		}
		if ( temp[0].y < v[3].y ) {
			temp[3] = temp[2];
			temp[2] = temp[1];
			temp[1] = temp[0];
			temp[0] = v[3];
		}
		else {
			if ( temp[1].y < v[3].y ) {
				temp[3] = temp[2];
				temp[2] = temp[1];
				temp[1] = v[3];
			}
			else if ( temp[2].y < v[3].y ) {
				temp[3] = temp[2];
				temp[2] = v[3];
			}
		}
	}
}

void Poly::XSort() {
}

void Poly::ZSort() {
}