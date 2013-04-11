#ifndef	Polygon_H
#define	Polygon_H

#include "Vertex.h"
#include "../Sprites/defaulttexture.h"

#define POLY_MAX_VERTICES 4		// don't want anything other than tris and quads

enum PolyType {
    FLAT_TOP_RIGHT
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
	Vertex v1,v2,v3,v4;						// Only support Quads and Triangles
	
	const unsigned short* texture;			// for texturing
	int texwidth;							// width of texture

	RenderType rType;		// Generally defined by the object, stored here for speed
    PolyType tType;			// for fast rendering, pre-compute

	Vector3D normal;
	bool visible;		// for backface culling

    Poly() {
		texture = defaulttexture;
		texwidth = defaulttexturewidth;
	}
    Poly(const Vertex& _v1,
		 const Vertex& _v2,
		 const Vertex& _v3,
		 const Vertex& _v4 = Vertex(),
		 const Vector3D& n = Vector3D(),
		 const RenderType rt = FLAT,
		 const unsigned short* tex = defaulttexture,
		 const int width = defaulttexturewidth) {
		v1 = _v1;
		v2 = _v2;
		v3 = _v3;
		v4 = _v4;
		normal = n;
		rType = rt;
		texture = tex;
		texwidth = width;
	}
    ~Poly(){}

	// Polygon variable setter methods
	void SetTexture( const unsigned short *tex, const int width ) { texture = tex; texwidth = width; }

	// General Transformation Methods, only operate on x,y,z,w of vertices
	void Transform( const Matrix& m );
    void Translate( const Vector3D& v );
    void Translate( const float x, const float y, const float z );

	// Pipeline Transformation Methods
    void TransformToCamera( const Matrix& m );
    void TransformToPerspective( const Matrix& m );

	// Pipeline function methods
	void Clip( );

	// Rasterization Methods
	void Rasterize( );
};

#endif		// Polygon_H