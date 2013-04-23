#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "viewplane.h"
#include "color.h"
#include "Object.h"
#include "tracer.h"

using namespace std;

class World
{

private: 
	std::list<Object> master;
	std::list<Object> temp;
	long id;

public:

	//constructor
	World();

	//alternate constructor
	World(long worldID);

	//destructor
	~World();

	//allows world change
	void changeWorld(long worldID);

	//clear Master
	void masterClear();

	//clear Temp
	void tempClear();

	//adds correct world to temp list
	void library(long worldID);

	//Updates Temp list with any changes to the master list
	bool updateList();

	//returns renderList
	std::list<Object> getRenderList();

	///////////////////////////////////////////////////////
	///////////////// Custom Worlds///////////////////////
	//////////////////////////////////////////////////////

	//basic layout
	void basicWorld();
};



#endif