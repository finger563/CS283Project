#ifndef	Polygon_H
#define	Polygon_H

#include "Vertex.h"
#include "../Textures.h"

#define POLY_MAX_VERTICES 4		// don't want anything other than tris and quads

// This makes rasterization much faster (precompute values)
enum PolyType {
    FLAT_TOP_RIGHT_T,
    FLAT_TOP_LEFT_T,
    FLAT_BOTTOM_RIGHT_T,
    FLAT_BOTTOM_LEFT_T,
    NORMAL_RIGHT_T,
    NORMAL_LEFT_T
};

// This tells the engine what type of rendering we want for this polygon
enum RenderType {
	FLAT,					// Color is average of the 3 vertices
	COLORED,				// Color is interpolated between vertices
	SMOOTH,					// Normals are interpolated (smooth shading)
	TEXTURED,				// Texture Coords are interpolated (no shading)
	TEXTURED_SMOOTH			// Texture coords and normals are interpolated
};

class Poly
{
public:
	Vertex v[POLY_MAX_VERTICES];
	int numVertices;
	
	const unsigned short* texture;			// for texturing
	int texwidth;							// width of texture

	float r,g,b;							// for flat coloring

	RenderType rType;		// Generally defined by the object, stored here for speed
    PolyType tType;			// for fast rendering, pre-compute

	Vector3D normal;
	bool visible;		// for backface culling

    Poly() {
		numVertices = 0;
		texture = defaulttexture;
		texwidth = defaulttexturewidth;
		r=g=b=1;
	}
    Poly(const Vertex& _v1,
		 const Vertex& _v2,
		 const Vertex& _v3,
		 const Vertex& _v4 = Vertex(),
		 const int vertices = 3,
		 const Vector3D& n = Vector3D(),
		 const RenderType rt = FLAT,
		 const Vector3D& rgb = Vector3D(1,1,1,1),
		 const unsigned short* tex = defaulttexture,
		 const int width = defaulttexturewidth) {
		v[0] = _v1;
		v[1] = _v2;
		v[2] = _v3;
		v[3] = _v4;
		numVertices = vertices;
		normal = n;
		rType = rt;
		texture = tex;
		texwidth = width;
		r=rgb.x;
		b=rgb.y;
		g=rgb.z;
	}
    ~Poly(){}

	// Polygon variable setter methods
	void SetTexture( const unsigned short *tex, 
					 const int width,
					 Point2D u1 = Point2D(),
					 Point2D u2 = Point2D(),
					 Point2D u3 = Point2D(),
					 Point2D u4 = Point2D()) { 
		texture = tex; texwidth = width; 
		v[0].u = u1.x;
		v[0].v = u1.y;
		v[1].u = u2.x;
		v[1].v = u2.y;
		v[2].u = u3.x;
		v[2].v = u3.y;
		v[3].u = u4.x;
		v[3].v = u4.y;
	}
	void SetRenderType( const RenderType rt ) { rType =rt; }
	void SetNormal( const Vector3D n ) { normal = n; }

	// General Transformation Methods, only operate on x,y,z,w of vertices
	void Transform( const Matrix& _m );
    void Translate( const Vector3D& _v );
    void Translate( const float _x, const float _y, const float _z );

	// Pipeline Transformation Methods
    void TransformToCamera( const Matrix& _m );
    void TransformToPerspective( const Matrix& _m );
	void TransformToPixel( const Matrix & _m );

	// Pipeline function methods
	void Clip( );
	void HomogeneousDivide( );

	// Rasterization Methods
	void Rasterize( );
	void Rasterize( const int y );

	// Helper Functions
	void YSort();
	void XSort();
	void ZSort();

	// Operator Overloads
	Poly& operator= (const Poly& rhs);
};

#endif		// Polygon_H