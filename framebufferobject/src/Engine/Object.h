#ifndef	OBJECT_H
#define	OBJECT_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <list>
#include "triangle.h"

class Object
{
public:

	//Constructor
	Object();

	//Alternate Constructor
	Object(Triangle poly, const unsigned short* texture, Vector3D vel = Vector3D(), 
		Vector3D head = Vector3D(), Point3D pos = Point3D());
	
	//Destructor
	~Object();

	//Updates Temp last with any changes to the master list
	bool updateList();

	//Updates Temp list to whatever list is passed (i.e. Render list)
	bool updateList(std::list<Triangle> poly);

	//Clears Temp list
	void Object::clearTemp();

	//add polygon to lists
	void add(Triangle poly);

	//Generates cube with default size being 5
	//5 here is half the length of the cube
	//total length will = 10 but 5 works well with polygons
	void generateCube(float size = 5);

	//Generates tetrahedron
	void generateTetra(float size = 5);

	//fileParser()<-- future function
	bool Update(int time);

	//sets velocity
	bool setVel(Vector3D vector);
	
	//sets heading
	bool setHead(Vector3D head);

	//sets position
	bool setPosition(Point3D pos);

	//assumes that only the temp list is being passed through
	std::list<Triangle> Rotate(std::list<Triangle> poly, Matrix& m);

	//assumes that only the temp list is being passed through
	std::list<Triangle> Translate(std::list<Triangle> poly, Vector3D& v);

	//transforms polygons to screen
	//assumes that only the temp list is being passed through
	//may need to add return feature to TransformtoScreen()
	std::list<Triangle> TransformToScreen(std::list<Triangle> poly, Matrix& m);

	//returns final render list
	std::list<Triangle> getRenderList();

private:

	std::list<Triangle> master;
	std::list<Triangle> temp;
	Vector3D heading, velocity; 
	Point3D position;
	const unsigned short* tex;
	//Object* next;

};

#endif