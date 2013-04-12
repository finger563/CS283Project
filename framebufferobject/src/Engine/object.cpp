#include "object.h"


//Constructor
Object::Object()
{
	velocity = Vector3D(0,0,0);
	position = Point3D(0,0,0);
}

//Alternate texture, veloctity, heading, position
Object::Object(const unsigned short* texture, const int texWid, Vector3D vel, Point3D pos)
	{
		velocity = vel;
		position = pos;
		tex = texture;
		texWidth = texWid;
	}
//Alternate Constructor
Object::Object(Triangle poly, const unsigned short* texture, const int texWid, Vector3D vel, 
		Point3D pos)
{
	velocity = vel;
	position = pos;
	tex = texture;
	texWidth = texWid;

	//use Set Texture command instead...
	poly.SetTexture(texture, texWidth);
	tex = texture;
	master.push_back(poly);
	temp.push_back(poly);
}

//generate() method switch statements??

//Updates Temp list with any changes to the master list
bool Object::updateList()
{

	//if(!temp.empty() || master.empty())
		clearTemp();

	if(!master.empty())
	{
		//makes copy from master to temp
		for(std::list<Triangle>::iterator it = master.begin(); it != master.end(); ++it)
		{
			temp.push_back(*it);
		}
	
		if(!temp.empty())
			return true;
		else
			return false;
	}


	return true;
}


//Updates Temp list with any changes to the master list
bool Object::updateList(std::list<Triangle> poly)
{
	//if(!temp.empty() || poly.empty())
		clearTemp();

	if(!poly.empty())
	{
		//makes copy from master to temp
		for(std::list<Triangle>::iterator it = poly.begin(); it != poly.end(); ++it)
		{
			temp.push_back(*it);
		}
	
		if(!temp.empty())
			return true;
		else
			return false;
	}

	return true;
}

//Replaces Temp list with render list
void Object::clearTemp()
{
	//empties temp list
	temp.clear();
}

//Rotate Temp list
void Object::rotateTemp(Matrix m)
{
	for(std::list<Triangle>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		it->Rotate(m);
	}
}

//add polygon to lists
void Object::add(Triangle poly)
{
	master.push_back(poly);
	updateList();
}

//generates cube
void Object::generateCube(float size)
{
	//assume texture is set and set texture 
	Triangle tri1 = Triangle( Point3D(size, size, size),Point3D(size, -size, size), 
					Point3D(-size, size, size), Vector3D(0,0,1),Point2D(0,0),Point2D(0,texWidth),Point2D(texWidth,0)),

			tri2 = Triangle(Point3D(size, -size, size), Point3D(-size, size, size),
					Point3D(-size, -size, size), Vector3D(0,0,1),Point2D(0,texWidth),Point2D(texWidth,0),Point2D(texWidth,texWidth)),

			tri3 = Triangle( Point3D(size, size, -size), Point3D(size, -size, -size),
					Point3D(-size, size, -size), Vector3D(0,0,-1),Point2D(texWidth,0),Point2D(texWidth,texWidth),Point2D(0,0)),

			tri4 = Triangle( Point3D(size, -size, -size),Point3D(-size, size, -size),
					Point3D(-size, -size, -size), Vector3D(0,0,-1),Point2D(texWidth,texWidth),Point2D(0,0),Point2D(0,texWidth)),
			
			tri5 = Triangle( Point3D(size, size, -size), Point3D(size, size, size),
					Point3D(size, -size, size),Vector3D(1,0,0),Point2D(0,0),Point2D(texWidth,0),Point2D(texWidth,texWidth)),

			tri6 = Triangle( Point3D(size, size, -size),Point3D(size, -size, -size),
					Point3D(size, -size, size),Vector3D(1,0,0),Point2D(0,0),Point2D(0,texWidth),Point2D(texWidth,texWidth)),

			tri7 = Triangle( Point3D(-size, size, -size),Point3D(-size, size, size),
					Point3D(-size, -size, -size),Vector3D(-1,0,0),Point2D(texWidth,0),Point2D(0,0),Point2D(texWidth,texWidth)),

			tri8 = Triangle( Point3D(-size, size, size),Point3D(-size, -size, size),
					Point3D(-size, -size, -size),Vector3D(-1,0,0),Point2D(0,0),Point2D(0,texWidth),Point2D(texWidth,texWidth)),

			tri9 = Triangle( Point3D(size, size, size),Point3D(-size, size, size),
					Point3D(size, size, -size),Vector3D(0,1,0),Point2D(texWidth,0),Point2D(0,0),Point2D(texWidth,texWidth)),

			tri10 = Triangle( Point3D(-size, size, -size),Point3D(-size, size, size),
					Point3D(size, size, -size),Vector3D(0,1,0),Point2D(0,texWidth),Point2D(0,0),Point2D(texWidth,texWidth)),

			tri11 = Triangle( Point3D(size, -size, size),Point3D(size, -size, -size),
					Point3D(-size, -size, -size),Vector3D(0,-1,0),Point2D(texWidth,texWidth),Point2D(texWidth,0),Point2D(0,0)),

			tri12 = Triangle( Point3D(size, -size, size),Point3D(-size, -size, size),
					Point3D(-size, -size, -size),Vector3D(0,-1,0),Point2D(texWidth,texWidth),Point2D(0,texWidth),Point2D(0,0));
	  
	master.clear();

	//stores in objects master list
	master.push_back(tri1);
	master.push_back(tri2);
	master.push_back(tri3);
	master.push_back(tri4);
	master.push_back(tri5);
	master.push_back(tri6);
	master.push_back(tri7);
	master.push_back(tri8);
	master.push_back(tri9);
	master.push_back(tri10);
	master.push_back(tri11);
	master.push_back(tri12);

	for(std::list<Triangle>::iterator it = master.begin(); it != master.end(); ++it)
	{
		it->SetTexture(tex, texWidth);
	}

	updateList(); 
}

//generates tetrahedron
void Object::generateTetra(float size)
{
	Point3D p1 = Point3D(size,0,-size/sqrt(2.0)),
			p2 = Point3D(-size,0,-size/sqrt(2.0)),
			p3 = Point3D(0,size,size/sqrt(2.0)),
			p4 = Point3D(0,-size,size/sqrt(2.0));

	Triangle tri1 = Triangle( p1, p2, p3, Vector3D(0,0,1),Point2D(0,0),Point2D(0,texWidth),Point2D(texWidth,0)),

			 tri2 = Triangle( p2, p3, p4, Vector3D(0,0,1),Point2D(0,texWidth),Point2D(texWidth,0),Point2D(texWidth,texWidth)),

			 tri3 = Triangle( p3, p4, p1, Vector3D(0,0,-1),Point2D(texWidth,0),Point2D(texWidth,texWidth),Point2D(0,0)),

			 tri4 = Triangle( p1, p2, p4, Vector3D(0,0,-1),Point2D(texWidth,texWidth),Point2D(0,0),Point2D(0,texWidth));
	
	master.clear();

	//stores in objects master list
	master.push_back(tri1);
	master.push_back(tri2);
	master.push_back(tri3);
	master.push_back(tri4);

	for(std::list<Triangle>::iterator it = master.begin(); it != master.end(); ++it)
	{
		it->SetTexture(tex, texWidth);
	}

	updateList(); 
}

//generates cube
void Object::generateFloor(float length, float depth)
{
	float sl = 5;	// sidelength of the square
	position = Point3D(0,depth,0);

#ifndef CLIPPING_TEST
	Triangle tri1 = Triangle(Point3D(-sl,0,-sl),Point3D(-sl,0,sl),Point3D(sl,0,sl),
							 Vector3D(0,1,0),Point2D(0,texWidth),Point2D(0,0),Point2D(texWidth,0)),
			 tri2 = Triangle(Point3D(-sl,0,-sl),Point3D(sl,0,-sl),Point3D(sl,0,sl),
							 Vector3D(0,1,0),Point2D(0,texWidth),Point2D(texWidth,texWidth),Point2D(texWidth,0));
	
	master.clear();
	tri1.Translate(-length/2,0,-length/2);	// move to back left	(x = -length,z=-length)
	tri2.Translate(-length/2,0,-length/2);	// move to back left	(x = -length,z=-length)
	for (int x = 0;x <= length/sl; x++) {
		for (int z = 0; z <= length/sl; z++) {
			master.push_back(tri1);
			master.push_back(tri2);
			tri1.Translate(0,0,sl);
			tri2.Translate(0,0,sl);
		}
		tri1.Translate(sl,0,-length - sl);
		tri2.Translate(sl,0,-length - sl);
	}
#else
	Triangle tri1 = Triangle(Point3D(-length,0,-length),Point3D(-length,0,length),Point3D(length,0,length),
							 Vector3D(0,1,0),Point2D(0,texWidth),Point2D(0,0),Point2D(texWidth,0)),
			 tri2 = Triangle(Point3D(-length,0,-length),Point3D(length,0,-length),Point3D(length,0,length),
							 Vector3D(0,1,0),Point2D(0,texWidth),Point2D(texWidth,texWidth),Point2D(texWidth,0));
	master.clear();
	master.push_back(tri1);
	master.push_back(tri2);
#endif

	for(std::list<Triangle>::iterator it = master.begin(); it != master.end(); ++it)
	{
		it->SetTexture(tex, texWidth);
	}

	updateList(); 
}

bool Object::updateTime(int time)
{
	return true;
}

bool Object::setVel(Vector3D vector)
{
	velocity = vector;
	return true;
}

bool Object::setPosition(Point3D pos)
{
	position = pos;
	return true;
}

Point3D Object::getPosition(void) {
	return position;
}

//assumes that only the temp list is being passed through
std::list<Triangle> Object::Rotate(Matrix& m)
{
	for(std::list<Triangle>::iterator it = master.begin(); it != master.end(); ++it)
	{
		it->Rotate(m);
	}

	updateList();

	return temp;
}

//assumes that only the temp list is being passed through
std::list<Triangle> Object::Translate(Vector3D& v)
{
	for(std::list<Triangle>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		it->Translate(v);
	}

	return temp;
}

//returns modified polygon list
std::list<Triangle> Object::TransformToScreen(Matrix& m)
{
	for(std::list<Triangle>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		it->TransformToScreen(m);
	}

	return temp;
}

//returns modified polygon list
std::list<Triangle> Object::TransformToScreenHomogeneous(Matrix& m)
{
	for(std::list<Triangle>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		it->TransformToScreenHomogeneous(m);
	}

	return temp;
}

//returns final render list
std::list<Triangle> Object::getRenderList()
{
	std::list<Triangle> get;
	for(std::list<Triangle>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		if( it->visible &&
#ifndef CLIPPING_TEST
			it->a.z > 0 &&
			it->b.z > 0 &&
			it->c.z > 0
#else
				(
				it->a.z > 0 ||
				it->b.z > 0 ||
				it->c.z > 0
				)
#endif
			)
		{
			get.push_back(*it);
		}
	}

	updateList(get);
	
	return temp;
}

#if 0
#include "object.h"


//Constructor
Object::Object()
{
	velocity = Vector3D(0,0,0);
	heading = Vector3D(0,0,0);
	position = Point3D(0,0,0);
}

//Alternate Constructor
Object::Object(Triangle poly, const unsigned short* texture, Vector3D vel, 
		Vector3D head, Point3D pos)
{
	velocity = vel;
	position = pos; 
	heading = head;
	poly.texture = texture;
	tex = texture;
	master.push_back(poly);
	temp.push_back(poly);
}

//Destructor
Object::~Object()
{
	if( *tex != 0)
		delete tex;
}

//generate() method switch statements??

//Updates Temp list with any changes to the master list
bool Object::updateList()
{

	if(!temp.empty() || master.empty())
		clearTemp();

	if(!master.empty())
	{
		//makes copy from master to temp
		for(std::list<Triangle>::iterator it = master.begin(); it != master.end(); ++it)
		{
			temp.push_back(*it);
		}
	
		if(!temp.empty())
			return true;
		else
			return false;
	}


	return true;
}

//Updates Temp list with any changes to the master list
bool Object::updateList(std::list<Triangle> poly)
{
	if(!temp.empty() || poly.empty())
		clearTemp();

	if(!poly.empty())
	{
		//makes copy from master to temp
		for(std::list<Triangle>::iterator it = poly.begin(); it != poly.end(); ++it)
		{
			temp.push_back(*it);
		}
	
		if(!temp.empty())
			return true;
		else
			return false;
	}

	return true;
}

//Replaces Temp list with render list
void Object::clearTemp()
{
	//empties temp list
	std::list<Triangle>::iterator start, end;
	start = temp.begin();
	end = temp.end();
	temp.erase(start, end);
}
//add polygon to lists
void Object::add(Triangle poly)
{
	master.push_back(poly);
	updateList();
}

//generates cube
void Object::generateCube(float size)
{
	Triangle tri1 = Triangle( Point3D(size, size, size),Point3D(size, -size, size), 
					Point3D(-size, size, size), Vector3D(0,0,1),Point2D(0,0),Point2D(0,texWidth),Point2D(texWidth,0)),

			tri2 = Triangle(Point3D(size, -size, size), Point3D(-size, size, size),
					Point3D(-size, -size, size), Vector3D(0,0,1),Point2D(0,texWidth),Point2D(texWidth,0),Point2D(texWidth,texWidth)),

			tri3 = Triangle( Point3D(size, size, -size), Point3D(size, -size, -size),
					Point3D(-size, size, -size), Vector3D(0,0,-1),Point2D(texWidth,0),Point2D(texWidth,texWidth),Point2D(0,0)),

			tri4 = Triangle( Point3D(size, -size, -size),Point3D(-size, size, -size),
					Point3D(-size, -size, -size), Vector3D(0,0,-1),Point2D(texWidth,texWidth),Point2D(0,0),Point2D(0,texWidth)),
			
			tri5 = Triangle( Point3D(size, size, -size), Point3D(size, size, size),
					Point3D(size, -size, size),Vector3D(1,0,0),Point2D(0,0),Point2D(texWidth,0),Point2D(texWidth,texWidth)),

			tri6 = Triangle( Point3D(size, size, -size),Point3D(size, -size, -size),
					Point3D(size, -size, size),Vector3D(1,0,0),Point2D(0,0),Point2D(0,texWidth),Point2D(texWidth,texWidth)),

			tri7 = Triangle( Point3D(-size, size, -size),Point3D(-size, size, size),
					Point3D(-size, -size, -size),Vector3D(-1,0,0),Point2D(texWidth,0),Point2D(0,0),Point2D(texWidth,texWidth)),

			tri8 = Triangle( Point3D(-size, size, size),Point3D(-size, -size, size),
					Point3D(-size, -size, -size),Vector3D(-1,0,0),Point2D(0,0),Point2D(0,texWidth),Point2D(texWidth,texWidth)),

			tri9 = Triangle( Point3D(size, size, size),Point3D(-size, size, size),
					Point3D(size, size, -size),Vector3D(0,1,0),Point2D(texWidth,0),Point2D(0,0),Point2D(texWidth,texWidth)),

			tri10 = Triangle( Point3D(-size, size, -size),Point3D(-size, size, size),
					Point3D(size, size, -size),Vector3D(0,1,0),Point2D(0,texWidth),Point2D(0,0),Point2D(texWidth,texWidth)),

			tri11 = Triangle( Point3D(size, -size, size),Point3D(size, -size, -size),
					Point3D(-size, -size, -size),Vector3D(0,-1,0),Point2D(texWidth,texWidth),Point2D(texWidth,0),Point2D(0,0)),

			tri12 = Triangle( Point3D(size, -size, size),Point3D(-size, -size, size),
					Point3D(-size, -size, -size),Vector3D(0,-1,0),Point2D(texWidth,texWidth),Point2D(0,texWidth),Point2D(0,0));
	
	//stores in objects master list
		master.push_back(tri1);
		master.push_back(tri2);
		master.push_back(tri3);
		master.push_back(tri4);
		master.push_back(tri5);
		master.push_back(tri6);
		master.push_back(tri7);
		master.push_back(tri8);
		master.push_back(tri9);
		master.push_back(tri10);
		master.push_back(tri11);
		master.push_back(tri12);

		updateList(); 
}

//generates tetrahedron
void Object::generateTetra(float size)
{
	Triangle tri1 = Triangle( Point3D(size, size, size),Point3D(size, -size, size), 
					Point3D(-size, size, size), Vector3D(0,0,1),Point2D(0,0),Point2D(0,texWidth),Point2D(texWidth,0)),

			tri2 = Triangle(Point3D(size, -size, size), Point3D(-size, size, size),
					Point3D(-size, -size, size), Vector3D(0,0,1),Point2D(0,texWidth),Point2D(texWidth,0),Point2D(texWidth,texWidth)),

			tri3 = Triangle( Point3D(size, size, -size), Point3D(size, -size, -size),
					Point3D(-size, size, -size), Vector3D(0,0,-1),Point2D(texWidth,0),Point2D(texWidth,texWidth),Point2D(0,0)),

			tri4 = Triangle( Point3D(size, -size, -size),Point3D(-size, size, -size),
					Point3D(-size, -size, -size), Vector3D(0,0,-1),Point2D(texWidth,texWidth),Point2D(0,0),Point2D(0,texWidth));
}
bool Object::Update(int time)
{
	return true;
}

bool Object::setVel(Vector3D vector)
{
	velocity = vector;
	return true;
}

bool Object::setHead(Vector3D head)
{
	heading = head;
	return true;
}

bool Object::setPosition(Point3D pos)
{
	position = pos;
	return true;
}

//assumes that only the temp list is being passed through
//make all rotations first and then update temp. 
void Object::Rotate(Matrix& m)
{
	for(std::list<Triangle>::iterator it = master.begin(); it != master.end(); ++it)
	{
		it->Rotate(m);
	}

}

//assumes that only the temp list is being passed through
/*void Object::Translate(Vector3D& v)
{
	for(std::list<Triangle>::iterator it = poly.begin(); it != poly.end(); ++it)
	{
		it->Translate(v);
	}

	updateRenderList(poly);
}*/

//Assumes only temp list is being passed
//returns modified polygon list
void Object::TransformToScreen(Matrix& m)
{
	for(std::list<Triangle>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		it->TransformToScreen(m);
	}

	//updateRenderList(poly);
}

//returns final render list
std::list<Triangle> Object::getRenderList()
{
	std::list<Triangle> get;
	for(std::list<Triangle>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		if(it->normal.z <= 0)
		{
			get.push_back(*it);
		}
	}

	updateList(get);
	
	return temp;
}
#endif