#include "world.h"

//constructor
World::World(): id(0)
{
	library(id);
}

//alternate constructor
World::World(long worldID): id(worldID)
{
	library(id);
}

//destructor
World::~World(){}

//allows world change
void World::changeWorld(long worldID)
{
	id = worldID;
	library(id);
}

void World::masterClear()
{
	if(!master.empty())
			master.clear();
}

//clear Temp
void World::tempClear()
{
	if(!temp.empty())
			temp.clear();
}

//adds correct world to temp list
void World::library(long worldID)
{
	switch ( worldID ) {
	case 0:
		masterClear();
		basicWorld();
		updateList();
		break;
	default:
		masterClear();
		tempClear();
	
	}
}

//Updates Temp list with any changes to the master list
bool World::updateList()
{
	tempClear();

	if(!master.empty())
	{
		//makes copy from master to temp
		for(std::list<Object>::iterator it = master.begin(); it != master.end(); ++it)
		{
			temp.push_back(*it);
		}
	
		if(!temp.empty())
			return true;
		else
			return false;
	}


	return false;
}

//returns renderList
std::list<Object> World::getRenderList()
{
	return temp;
}

///////////////////////////////////////////////////////
///////////////// Custom Worlds///////////////////////
//////////////////////////////////////////////////////

//Pre-condition: master list is already cleared
//basic world
void World::basicWorld()
{
	Poly testpoly = Poly(Vertex(-6.666,6.666,0,1,0,0),
					 Vertex(13.333,6.666,0,1,1,0),
					 Vertex(-6.666,-13.333,0,1,0,1),
					 Vertex(),3,Vector3D(0,0,-1),TEXTURED);

	Object testobj = Object(box,boxtexwidth,boxtexheight,Vector3D(),Point3D(-10,-5,15));
	Object testobj2 = Object(box,boxtexwidth,boxtexheight,Vector3D(),Point3D(10,-5,15));
	Object testobj3 = Object(floorlarge,floorlargewidth,floorlargeheight);
	Object testobj4 = Object(testpoly,box,boxtexwidth,boxtexheight,Vector3D(),Point3D(0,0,15));

	testobj.generateCube();

	testobj2.generateCube();
	testobj2.SetRenderType(COLORED);

	testobj3.generateFloor(75,-10);

	master.push_back(testobj);
	master.push_back(testobj2);	
	master.push_back(testobj3);
	master.push_back(testobj4);
}