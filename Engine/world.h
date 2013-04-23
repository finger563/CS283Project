#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "viewplane.h"
#include "color.h"
#include "geometricobject.h"
#include "tracer.h"

using namespace std;

class World
{
public:
	ViewPlane					vp;
	Color						background_color;
	vector<GeometricObject>		*objects;
	Tracer						*tracer_ptr;

	World();
	~World();

	void Build(void);

	void AddObject(const GeometricObject &object_ptr);

	Record Hit_bare_bones_objects(const Ray& ray) const;

	void RenderScene(void);

	void OpenWindow(const int hres, const int vres) const;

	void DisplayPixel(const int row,
		const int column,
		const Color& pixel_color) const;
};

inline void
	World::AddObject(const GeometricObject &object_ptr) {
		objects->push_back(object_ptr);
}

#endif