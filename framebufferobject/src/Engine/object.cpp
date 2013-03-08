#include "object.h"


//Constructor
Object::Object()
{
	velocity = Vector3D(0,0,0);
	heading = Vector3D(0,0,0);
	position = Point3D(0,0,0);
}

//Alternate Constructor
Object::Object(Triangle poly, const unsigned short* texture, Vector3D vel = Vector3D(), 
		Vector3D head = Vector3D(), Point3D pos = Point3D())
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

//Updates Temp last with any changes to the master list
bool Object::updateList()
{
	//makes copy to temp
	std::copy (master.begin(), master.end(), temp.begin());
}

//add polygon to lists
void Object::add(Triangle poly)
{
	master.push_back(poly);
	updateList();
}

//generates cube
void Object::generateCube(float size = 5)
{
	Triangle tri1 = Triangle( Point3D(size, size, size),Point3D(size, -size, size), 
					Point3D(-size, size, size), Vector3D(0,0,1),Point2D(0,0),Point2D(0,512),Point2D(512,0)),

			tri2 = Triangle(Point3D(size, -size, size), Point3D(-size, size, size),
					Point3D(-size, -size, size), Vector3D(0,0,1),Point2D(0,512),Point2D(512,0),Point2D(512,512)),

			tri3 = Triangle( Point3D(size, size, -size), Point3D(size, -size, -size),
					Point3D(-size, size, -size), Vector3D(0,0,-1),Point2D(512,0),Point2D(512,512),Point2D(0,0)),

			tri4 = Triangle( Point3D(size, -size, -size),Point3D(-size, size, -size),
					Point3D(-size, -size, -size), Vector3D(0,0,-1),Point2D(512,512),Point2D(0,0),Point2D(0,512)),
			
			tri5 = Triangle( Point3D(size, size, -size), Point3D(size, size, size),
					Point3D(size, -size, size),Vector3D(1,0,0),Point2D(0,0),Point2D(512,0),Point2D(512,512)),

			tri6 = Triangle( Point3D(size, size, -size),Point3D(size, -size, -size),
					Point3D(size, -size, size),Vector3D(1,0,0),Point2D(0,0),Point2D(0,512),Point2D(512,512)),

			tri7 = Triangle( Point3D(-size, size, -size),Point3D(-size, size, size),
					Point3D(-size, -size, -size),Vector3D(-1,0,0),Point2D(512,0),Point2D(0,0),Point2D(512,512)),

			tri8 = Triangle( Point3D(-size, size, size),Point3D(-size, -size, size),
					Point3D(-size, -size, -size),Vector3D(-1,0,0),Point2D(0,0),Point2D(0,512),Point2D(512,512)),

			tri9 = Triangle( Point3D(size, size, size),Point3D(-size, size, size),
					Point3D(size, size, -size),Vector3D(0,1,0),Point2D(512,0),Point2D(0,0),Point2D(512,512)),

			tri10 = Triangle( Point3D(-size, size, -size),Point3D(-size, size, size),
					Point3D(size, size, -size),Vector3D(0,1,0),Point2D(0,512),Point2D(0,0),Point2D(512,512)),

			tri11 = Triangle( Point3D(size, -size, size),Point3D(size, -size, -size),
					Point3D(-size, -size, -size),Vector3D(0,-1,0),Point2D(512,512),Point2D(512,0),Point2D(0,0)),

			tri12 = Triangle( Point3D(size, -size, size),Point3D(-size, -size, size),
					Point3D(-size, -size, -size),Vector3D(0,-1,0),Point2D(512,512),Point2D(0,512),Point2D(0,0));
	
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
void Object::generateTetra(float size = 5)
{

}
bool Object::Update(int time)
{}

bool Object::setVel(Vector3D vector)
{
	velocity = vector;
}

bool Object::setHead(Vector3D head)
{
	heading = head;
}

bool Object::setPosition(Point3D pos)
{
	position = pos;
}

//assumes that only the temp list is being passed through
std::list<Triangle> Object::Rotate(std::list<Triangle> poly, Matrix& m)
{
	for(std::list<Triangle>::iterator it = poly.begin(); it != poly.end(); ++it)
	{
		it->Rotate(m);
	}

	return poly;
}

//assumes that only the temp list is being passed through
std::list<Triangle> Object::Translate(std::list<Triangle> poly, Vector3D& v)
{
	for(std::list<Triangle>::iterator it = poly.begin(); it != poly.end(); ++it)
	{
		it->Translate(v);
	}

	return poly;
}

//Assumes only temp list is being passed
//returns modified polygon list
std::list<Triangle> Object::TransformToScreen(std::list<Triangle> poly, Matrix& m)
{
	for(std::list<Triangle>::iterator it = poly.begin(); it != poly.end(); ++it)
	{
		it->TransformToScreen(m);
	}

	return poly;
}