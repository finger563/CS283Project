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
	Object(const unsigned short* texture, const int texWid, Vector3D vel = Vector3D(0,0,0), 
		Point3D pos = Point3D(0,0,0));

	//Alternate Constructor
	Object(Triangle poly, const unsigned short* texture, const int texWid, Vector3D vel = Vector3D(0,0,0), 
		Point3D pos = Point3D(0,0,0));

	//Destructor
	~Object();

	//Updates Temp last with any changes to the master list
	bool updateList();

	//Updates Temp list to whatever list is passed (i.e. Render list)
	bool updateList(std::list<Triangle> poly);

	//Clears Temp list
	void clearTemp();

	//Rotate Temp list
	void rotateTemp(Matrix m);

	//add polygon to lists
	void add(Triangle poly);

	//Generates cube with default size being 5
	//5 here is half the length of the cube
	//total length will = 10 but 5 works well with polygons
	void generateCube(float size = 5);

	//Generates tetrahedron
	void generateTetra(float size = 5);

	void generateFloor(float length = 50, float depth = -10);

	//fileParser()<-- future function
	bool updateTime(int time);

	//sets velocity
	bool setVel(Vector3D vector);

	//sets position
	bool setPosition(Point3D pos);

	//gets Position
	Point3D getPosition(void);

	//assumes that only the temp list is being passed through
	std::list<Triangle> Rotate(Matrix& m);

	//assumes that only the temp list is being passed through
	std::list<Triangle> Translate(Vector3D& v);

	//transforms polygons to screen
	//assumes that only the temp list is being passed through
	//may need to add return feature to TransformtoScreen()
	std::list<Triangle> TransformToScreen(Matrix& m);

	//returns final render list
	std::list<Triangle> getRenderList();

private:

	std::list<Triangle> master;
	std::list<Triangle> temp;
	Vector3D heading, velocity; 
	Point3D position;
	const unsigned short* tex;
	int texWidth;
	//Object* next;

};

#endif