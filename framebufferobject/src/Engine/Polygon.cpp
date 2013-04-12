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
	normal = _m*normal;
}

void Poly::TransformToPerspective( const Matrix& _m ) {
	for (int i=0;i<numVertices;i++)
		v[i].TransformToPerspective(_m);
	normal = _m*normal;
}

void Poly::TransformToPixel( const Matrix& _m ) {
	for (int i=0;i<numVertices;i++)
		v[i].Transform(_m);		// only transform the pixel coords (x,y,z)
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
	//YSort();	// sort the vertices by Y (screen-space)
	int y=0;
	if ( numVertices == 3 ) {
	}
	else {
	}
}

void Poly::Rasterize( const int y ) {
	//YSort();					// sort the vertices by Y (screen-space)
	float BC[4] = {0,0,0,0};		// boundary tests against y scanline
	int line[4] = {0,0,0,0};	// which lines cross y scanline
	int lines = 0;
	float a1,a2,ai;				// alphas for each crossing line (there can only be 2), and for inside scanline
	Vertex sv,ev,vi;			// Start and end scanline vertices, and rendering vertex

	if ( numVertices == 3 ) {
		for (int i=0;i<numVertices;i++) {
			BC[i] = v[i].y - y;		// + is above, - is below, 0 is on
		}
		line[0] =  ((BC[0] > 0 && BC[1] < 0 ) ||
					(BC[0] < 0 && BC[1] > 0 ) ) ? 1 : 0;
		line[1] =  ((BC[1] > 0 && BC[2] < 0 ) ||
					(BC[1] < 0 && BC[2] > 0 ) ) ? 1 : 0;
		line[2] =  ((BC[2] > 0 && BC[0] < 0 ) ||
					(BC[2] < 0 && BC[0] > 0 ) ) ? 1 : 0;
		lines = line[0] + line[1]*2 + line[2]*4;
		switch (lines) {
		case 0:
		case 1:		// These cases represent a degenerate triangle
		case 2:
		case 4:
		default:
			break;
		case 3:		// Lines 0 and 1 cross scanline
			a1 = BC[0]/(BC[0]-BC[1]);
			a2 = BC[1]/(BC[1]-BC[2]);
			sv = v[0] + (v[1]-v[0])*a1;
			ev = v[1] + (v[2]-v[1])*a2;
			break;
		case 5:		// Lines 0 and 2 cross scanline
			a1 = BC[0]/(BC[0]-BC[1]);
			a2 = BC[2]/(BC[2]-BC[0]);
			sv = v[0] + (v[1]-v[0])*a1;
			ev = v[2] + (v[0]-v[2])*a2;
			break;
		case 6:		// Lines 1 and 2 cross scanline
			a1 = BC[1]/(BC[1]-BC[2]);
			a2 = BC[2]/(BC[2]-BC[0]);
			sv = v[1] + (v[2]-v[1])*a1;
			ev = v[2] + (v[0]-v[2])*a2;
			break;
		}
		if ( sv.x > ev.x ) {	// need to flip start and end vertices
			Vertex temp = ev;
			ev = sv;
			sv = temp;
		}
		vi = sv;
		if ( vi.x < 0 ) {
			vi.x = 0;
		}
		if ( ev.x >= SIZE_X ) {
			ai = (sv.x-(SIZE_X-1))/((sv.x-(SIZE_X-1)) - (ev.x-(SIZE_X-1)));
			ev = sv + (ev-sv)*ai;
		}
		for (int x=vi.x;x<=ev.x;x++) {
			ai = (sv.x-x)/((sv.x-x) - (ev.x-x));
			vi = sv + (ev-sv)*ai;
			if ( vi.ez/vi.hw < z_buffer[x + y*SIZE_X] ) {
				z_buffer[x + y*SIZE_X] = vi.ez/vi.hw;
				switch ( rType ) {	// What are we interpolating/rendering?
				case FLAT:
					display_buffer[x + y*SIZE_X] = RGB_MAKE((int)(r*255),(int)(g*255),(int)(b*255));
					break;
				case COLORED:
					vi.r = vi.r/vi.hw;	// divide all interpolated values by hw 
					vi.g = vi.g/vi.hw;	// divide all interpolated values by hw 
					vi.b = vi.b/vi.hw;	// divide all interpolated values by hw 
					display_buffer[x + y*SIZE_X] = RGB_MAKE((int)(vi.r*255),(int)(vi.g*255),(int)(vi.g*255));
					break;
				case SMOOTH:
					break;
				case TEXTURED:
					vi.u = vi.u/vi.hw;	// divide all interpolated values by hw 
					vi.v = vi.v/vi.hw;	// divide all interpolated values by hw 
					display_buffer[x + y*SIZE_X] = texture[(int)vi.u + ((int)vi.v)*texwidth];
					break;
				case TEXTURED_SMOOTH:
					break;
				default:
					break;
				}
			}
		}
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

// Operator Overloads
Poly& Poly::operator= (const Poly& rhs) {
	for (int i=0;i<POLY_MAX_VERTICES;i++) 
		v[i] = rhs.v[i];

	numVertices = rhs.numVertices;
	
	texture = rhs.texture;
	texwidth = rhs.texwidth;

	r = rhs.r;
	g = rhs.g;
	b = rhs.b;

	rType = rhs.rType;
	tType = rhs.tType;

	normal = rhs.normal;
	visible = rhs.visible;
    return (*this); 
}