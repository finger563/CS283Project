#include "Polygon.h"

extern float z_buffer[SIZE_X*SIZE_Y];
extern short display_buffer[SIZE_X*SIZE_Y];

// General Transformation Methods, only operate on x,y,z,w of vertices
void Poly::Transform( const Matrix& _m ) {
	v[0].Transform(_m);
	v[1].Transform(_m);
	v[2].Transform(_m);
	if ( numVertices == 4 )
		v[3].Transform(_m);
	normal = normalize(_m*normal);
}

void Poly::Translate( const Vector3D& _v ) {
	v[0].Translate(_v);
	v[1].Translate(_v);
	v[2].Translate(_v);
	if ( numVertices == 4 )
		v[3].Translate(_v);
}

void Poly::Translate( const float _x, const float _y, const float _z ) {
	v[0].Translate(_x,_y,_z);
	v[1].Translate(_x,_y,_z);
	v[2].Translate(_x,_y,_z);
	if ( numVertices == 4 )
		v[3].Translate(_x,_y,_z);
}

// Pipeline Transformation Methods
void Poly::TransformToCamera( const Matrix& _m ) {
	v[0].TransformToCamera(_m);
	v[1].TransformToCamera(_m);
	v[2].TransformToCamera(_m);
	if ( numVertices == 4 )
		v[3].TransformToCamera(_m);
	normal = normalize(_m*normal);
}

void Poly::TransformToPerspective( const Matrix& _m ) {
	v[0].TransformToPerspective(_m);
	v[1].TransformToPerspective(_m);
	v[2].TransformToPerspective(_m);
	if ( numVertices == 4 )
		v[3].TransformToPerspective(_m);
	normal = normalize(_m*normal);
	visible = false;
	//Vector3D eye = Vector3D(0,0,-1);
	//Vector3D cull = eye-Vector3D(v[0].ex,v[0].ey,v[0].ez);
	//float test = cull*normal;
	if ( v[0].ez/v[0].hw > 0 ) {
		visible = true;	// Triangle is renderable
	}
}

void Poly::TransformToPixel( const Matrix& _m ) {
	v[0].Transform(_m);
	v[1].Transform(_m);
	v[2].Transform(_m);
	if ( numVertices == 4 )
		v[3].Transform(_m);		// only transform the pixel coords (x,y,z)
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
	v[0].HomogeneousDivide();
	v[1].HomogeneousDivide();
	v[2].HomogeneousDivide();
	if ( numVertices == 4 )
		v[3].HomogeneousDivide();
}

// Rasterization Methods
void Poly::Rasterize( ) {
	float BC[4] = {0,0,0,0};		// boundary tests against y scanline
	int line[4] = {0,0,0,0};	// which lines cross y scanline
	int lines = 0;
	float a1,a2,ai;				// alphas for each crossing line (there can only be 2), and for inside scanline
	Vertex sv,ev,vi;			// Start and end scanline vertices, and rendering vertex
	int y=0;

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
					display_buffer[x + y*SIZE_X] = texture[(int)(vi.u*texwidth) + ((int)(vi.v*texheight))*texwidth];
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

void Poly::Rasterize( const int y ) {
	if ( y > MaxY() || y < MinY() )
		return;
	float BC[4] = {0,0,0,0};		// boundary tests against y scanline
	int line[4] = {0,0,0,0};	// which lines cross y scanline
	int lines = 0;
	float a1,a2,ai;				// alphas for each crossing line (there can only be 2), and for inside scanline
	Vertex sv,ev,vi;			// Start and end scanline vertices, and rendering vertex
	
	for (int i=0;i<numVertices;i++) {
		BC[i] = v[i].y - y;		// + is above, - is below, 0 is on
	}

	if ( numVertices == 3 ) {
		line[0] =  ((BC[0] > 0 && BC[1] < 0 ) ||
					(BC[0] < 0 && BC[1] > 0 ) ) ? 1 : 0;
		line[1] =  ((BC[1] > 0 && BC[2] < 0 ) ||
					(BC[1] < 0 && BC[2] > 0 ) ) ? 1 : 0;
		line[2] =  ((BC[2] > 0 && BC[0] < 0 ) ||
					(BC[2] < 0 && BC[0] > 0 ) ) ? 1 : 0;
		lines = line[0] + line[1]*2 + line[2]*4;
		switch (lines) {
		default:	// These cases represent a degenerate triangle
			return;	// therefore do not do anything else
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
	}
	else {		// Poly is a QUAD
		line[0] =  ((BC[0] > 0 && BC[1] < 0 ) ||
					(BC[0] < 0 && BC[1] > 0 ) ) ? 1 : 0;
		line[1] =  ((BC[1] > 0 && BC[2] < 0 ) ||
					(BC[1] < 0 && BC[2] > 0 ) ) ? 1 : 0;
		line[2] =  ((BC[2] > 0 && BC[3] < 0 ) ||
					(BC[2] < 0 && BC[3] > 0 ) ) ? 1 : 0;
		line[3] =  ((BC[3] > 0 && BC[0] < 0 ) ||
					(BC[3] < 0 && BC[0] > 0 ) ) ? 1 : 0;
		lines = line[0] + line[1]*2 + line[2]*4 + line[3]*8;
		switch (lines) {
		default:	// These cases represent a degenerate triangle
			return;	// therefore do not do anything else
		case 3:		// Lines 0 and 1 cross scanline
			a1 = BC[0]/(BC[0]-BC[1]);
			a2 = BC[1]/(BC[1]-BC[2]);
			sv = v[0] + (v[1]-v[0])*a1;
			ev = v[1] + (v[2]-v[1])*a2;
			break;
		case 5:		// Lines 0 and 2 cross scanline
			a1 = BC[0]/(BC[0]-BC[1]);
			a2 = BC[2]/(BC[2]-BC[3]);
			sv = v[0] + (v[1]-v[0])*a1;
			ev = v[2] + (v[3]-v[2])*a2;
			break;
		case 6:		// Lines 1 and 2 cross scanline
			a1 = BC[1]/(BC[1]-BC[2]);
			a2 = BC[2]/(BC[2]-BC[3]);
			sv = v[1] + (v[2]-v[1])*a1;
			ev = v[2] + (v[3]-v[2])*a2;
			break;
		case 9:		// Lines 0 and 3 cross scanline
			a1 = BC[0]/(BC[0]-BC[1]);
			a2 = BC[3]/(BC[3]-BC[0]);
			sv = v[0] + (v[1]-v[0])*a1;
			ev = v[3] + (v[0]-v[3])*a2;
			break;
		case 10:	// Lines 1 and 3 cross scanline
			a1 = BC[1]/(BC[1]-BC[2]);
			a2 = BC[3]/(BC[3]-BC[0]);
			sv = v[1] + (v[2]-v[1])*a1;
			ev = v[3] + (v[0]-v[3])*a2;
			break;
		case 12:	// Lines 2 and 3 cross scanline
			a1 = BC[2]/(BC[2]-BC[3]);
			a2 = BC[3]/(BC[3]-BC[0]);
			sv = v[2] + (v[3]-v[2])*a1;
			ev = v[3] + (v[0]-v[3])*a2;
			break;
		}
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
	if ( floor(vi.x) == ceil(ev.x) )
		return;
	for (int x=ceil(vi.x);x<=floor(ev.x);x++) {
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
				if ( vi.u < 0 || vi.u > 1 )
					vi.u = 0;
				if ( vi.v < 0 || vi.v > 1 )
					vi.v = 0;
				display_buffer[x + y*SIZE_X] = texture[(int)(vi.u*(texwidth-1)) + ((int)(vi.v*(texheight-1)))*texwidth];
				break;
			case TEXTURED_SMOOTH:
				break;
			default:
				break;
			}
		}
	}
}

// Helper Functions
float Poly::MinX() {
	float min = 10000;
	for (int i=0;i<numVertices;i++) {
		if ( min > v[i].x )
			min = v[i].x;
	}
	return min;
}

float Poly::MinY() {
	float min = 10000;
	for (int i=0;i<numVertices;i++) {
		if ( min > v[i].y )
			min = v[i].y;
	}
	return min;
}

float Poly::MinZ() {
	float min = 10000;
	for (int i=0;i<numVertices;i++) {
		if ( min > v[i].z )
			min = v[i].z;
	}
	return min;
}

float Poly::MaxX() {
	float max = 0;
	for (int i=0;i<numVertices;i++) {
		if ( max < v[i].x )
			max = v[i].x;
	}
	return max;
}

float Poly::MaxY() {
	float max = 0;
	for (int i=0;i<numVertices;i++) {
		if ( max < v[i].y )
			max = v[i].y;
	}
	return max;
}

float Poly::MaxZ() {
	float max = 0;
	for (int i=0;i<numVertices;i++) {
		if ( max < v[i].z )
			max = v[i].z;
	}
	return max;
}

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