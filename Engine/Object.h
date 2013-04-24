#ifndef	OBJECT_H
#define	OBJECT_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <list>
#include "Polygon.h"

class Object
{
public:
	
	//Constructor
	Object();

	//Alternate Constructor
	Object(const unsigned short* texture, const int texWid, const int texHgt, Vector3D vel = Vector3D(0,0,0), 
		Point3D pos = Point3D(0,0,0), float r = 0);

	//Alternate Constructor
	Object(Poly poly, const unsigned short* texture, const int texWid, const int texHgt, Vector3D vel = Vector3D(0,0,0), 
		Point3D pos = Point3D(0,0,0), float r = 0);

	//Destructor
	~Object(){}

	//Updates Temp last with any changes to the master list
	bool updateList();

	//Updates Temp list to whatever list is passed (i.e. Render list)
	bool updateList(std::list<Poly> poly);

	// Rotates master list (i.e. in object space) to heading given by theta/phi
	void RotateToHeading();
	void RotateTempToHeading();

	//Clears Temp list
	void clearTemp();

	//Temp list operations
	void RotateTemp(const Matrix& m);
	void TranslateTemp(const Vector3D& v);

	//add polygon to lists
	void add(Poly poly);

	//Generates cube with with sidelength = size*2
	void GenerateCube(float size = 5);

	//Generates tetrahedron
	void GenerateTetra(float size = 5);

	// Generates a floor at depth, with sidelength = length
	void GenerateFloor(float length = 50, float depth = -10);

	void GenerateShot(Vector3D pos, float theta_, float phi_);
	
	void GeneratePlayer(Vector3D pos, float theta_, float phi_, 
		const unsigned short* texture = box, const int texWid=boxtexwidth, const int texHgt=boxtexheight);

	//fileParser()<-- future function

	bool UpdateTime(int time);

	bool SetVelocity(const Vector3D& vector);

	bool SetPosition(const Point3D& pos);
	Point3D GetPosition(void) const;

	bool SetRadius(float r);
	float GetRadius(void) const;

	// sets rendertype for all polygons
	bool SetRenderType( RenderType rt );

	//assumes that only the temp list is being passed through
	void Rotate(Matrix& m);

	//assumes that only the temp list is being passed through
	void Translate(Vector3D& v);

	// Pipeline functions
	void TransformToCamera(Matrix& m);
	void TransformToPerspective(Matrix& m);
	void TransformToPixel(Matrix& m);	
	std::list<Poly> GetRenderList() const;

	void projectileInit(Vector3D head, Vector3D pos = Vector3D(0, 0, 0));

	bool CollidesWith(const Object& b);

private:

	std::list<Poly> master;
	std::list<Poly> temp;
	Point3D position;
	Vector3D heading, velocity; 
	float theta,phi;
	float radius;
	const unsigned short* tex;
	int texWidth;
	int texHeight;
	size_t counter;
	bool kill;
	//Object* next;
};

#endif