#include "object.h"


//Constructor
Object::Object()
{
	velocity = Vector3D(0,0,0);
	position = Point3D(0,0,0);
}

//Alternate texture, veloctity, heading, position
Object::Object(const unsigned short* texture, const int texWid, const int texHgt, Vector3D vel, Point3D pos)
{
	velocity = vel;
	position = pos;
	tex = texture;
	texWidth = texWid;
	texHeight = texHgt;
}

//Alternate Constructor
Object::Object(Poly poly, const unsigned short* texture, const int texWid, const int texHgt, Vector3D vel, 
		Point3D pos)
{
	velocity = vel;
	position = pos;
	tex = texture;
	texWidth = texWid;
	texHeight = texHgt;

	//use Set Texture command instead...
	poly.SetTexture(texture, texWidth, texHeight);
	tex = texture;
	master.push_back(poly);
	temp.push_back(poly);
}

//generate() method switch statements??

//Updates Temp list with any changes to the master list
bool Object::updateList()
{
	clearTemp();

	if(!master.empty())
	{
		//makes copy from master to temp
		for(std::list<Poly>::iterator it = master.begin(); it != master.end(); ++it)
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
bool Object::updateList(std::list<Poly> poly)
{
	//if(!temp.empty() || poly.empty())
		clearTemp();

	if(!poly.empty())
	{
		//makes copy from master to temp
		for(std::list<Poly>::iterator it = poly.begin(); it != poly.end(); ++it)
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
	for(std::list<Poly>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		it->Transform(m);
	}
}

//add polygon to lists
void Object::add(Poly poly)
{
	master.push_back(poly);
	updateList();
}

//generates cube
void Object::generateCube(float size)
{
	//assume texture is set and set texture 
	Poly tri1 = Poly( Vertex(size, size, size,1,0,0),Vertex(size, -size, size,1,0,1), 
					Vertex(-size, size, size,1,1,0),Vertex(),3,Vector3D(0,0,1),TEXTURED),

			tri2 = Poly(Vertex(size, -size, size,1,0,1), Vertex(-size, size, size,1,1,0),
					Vertex(-size, -size, size,1,1,1),Vertex(),3,Vector3D(0,0,1),TEXTURED),

			tri3 = Poly( Vertex(size, size, -size,1,1,0), Vertex(size, -size, -size,1,1,1),
					Vertex(-size, size, -size,1,0,0),Vertex(),3,Vector3D(0,0,-1),TEXTURED),

			tri4 = Poly( Vertex(size, -size, -size,1,1,1),Vertex(-size, size, -size,1,0,0),
					Vertex(-size, -size, -size,1,0,1),Vertex(),3,Vector3D(0,0,-1),TEXTURED),
			
			tri5 = Poly( Vertex(size, size, -size,1,0,0), Vertex(size, size, size,1,1,0),
					Vertex(size, -size, size,1,1,1),Vertex(),3,Vector3D(1,0,0),TEXTURED),

			tri6 = Poly( Vertex(size, size, -size,1,0,0),Vertex(size, -size, -size,1,0,1),
					Vertex(size, -size, size,1,1,1),Vertex(),3,Vector3D(1,0,0),TEXTURED),

			tri7 = Poly( Vertex(-size, size, -size,1,1,0),Vertex(-size, size, size,1,0,0),
					Vertex(-size, -size, -size,1,1,1),Vertex(),3,Vector3D(-1,0,0),TEXTURED),

			tri8 = Poly( Vertex(-size, size, size,1,0,0),Vertex(-size, -size, size,1,0,1),
					Vertex(-size, -size, -size,1,1,1),Vertex(),3,Vector3D(-1,0,0),TEXTURED),

			tri9 = Poly( Vertex(size, size, size,1,1,0),Vertex(-size, size, size,1,0,0),
					Vertex(size, size, -size,1,1,1),Vertex(),3,Vector3D(0,1,0),TEXTURED),

			tri10 = Poly( Vertex(-size, size, -size,1,0,1),Vertex(-size, size, size,1,0,0),
					Vertex(size, size, -size,1,1,1),Vertex(),3,Vector3D(0,1,0),TEXTURED),

			tri11 = Poly( Vertex(size, -size, size,1,1,1),Vertex(size, -size, -size,1,1,0),
					Vertex(-size, -size, -size,1,0,0),Vertex(),3,Vector3D(0,-1,0),TEXTURED),

			tri12 = Poly( Vertex(size, -size, size,1,1,1),Vertex(-size, -size, size,1,0,1),
					Vertex(-size, -size, -size,1,0,0),Vertex(),3,Vector3D(0,-1,0),TEXTURED);
	  
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

	for(std::list<Poly>::iterator it = master.begin(); it != master.end(); ++it)
	{
		it->SetTexture(tex, texWidth, texHeight);
	}

	updateList(); 
}

//generates tetrahedron
void Object::generateTetra(float size)
{
	Vertex p1 = Vertex(size,0,-size/sqrt(2.0)),
			p2 = Vertex(-size,0,-size/sqrt(2.0)),
			p3 = Vertex(0,size,size/sqrt(2.0)),
			p4 = Vertex(0,-size,size/sqrt(2.0));

	Poly tri1 = Poly( p1, p2, p3),//, Vector3D(0,0,1),Point2D(0,0),Point2D(0,texWidth),Point2D(texWidth,0)),

			 tri2 = Poly( p2, p3, p4),//, Vector3D(0,0,1),Point2D(0,texWidth),Point2D(texWidth,0),Point2D(texWidth,texWidth)),

			 tri3 = Poly( p3, p4, p1),//, Vector3D(0,0,-1),Point2D(texWidth,0),Point2D(texWidth,texWidth),Point2D(0,0)),

			 tri4 = Poly( p1, p2, p4);//, Vector3D(0,0,-1),Point2D(texWidth,texWidth),Point2D(0,0),Point2D(0,texWidth));
	
	master.clear();

	//stores in objects master list
	master.push_back(tri1);
	master.push_back(tri2);
	master.push_back(tri3);
	master.push_back(tri4);

	for(std::list<Poly>::iterator it = master.begin(); it != master.end(); ++it)
	{
		it->SetTexture(tex, texWidth, texHeight);
	}

	updateList(); 
}

//generates cube
void Object::generateFloor(float length, float depth)
{
	float sl = 5;	// sidelength of the square
	position = Point3D(0,depth,0);

	Poly tri1 = Poly(Vertex(-length,0,-length,1,0,1),Vertex(-length,0,length,1,0,0),Vertex(length,0,length,1,1,0),
							Vertex(),3,Vector3D(0,1,0),TEXTURED),
			 tri2 = Poly(Vertex(-length,0,-length,1,0,1),Vertex(length,0,-length,1,1,1),Vertex(length,0,length,1,1,0),
						 Vertex(),3,Vector3D(0,1,0),TEXTURED);
	master.clear();
	master.push_back(tri1);
	master.push_back(tri2);

	for(std::list<Poly>::iterator it = master.begin(); it != master.end(); ++it)
	{
		it->SetTexture(tex, texWidth, texHeight);
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
void Object::Rotate(Matrix& m)
{
	for(std::list<Poly>::iterator it = master.begin(); it != master.end(); ++it)
	{
		it->Transform(m);
	}

	updateList();

	//return temp;
}

//assumes that only the temp list is being passed through
void Object::Translate(Vector3D& v)
{
	for(std::list<Poly>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		it->Translate(v.x,v.y,v.z);
	}

	//return temp;
}

//returns modified polygon list
void Object::TransformToCamera(Matrix& m)
{
	for(std::list<Poly>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		it->TransformToCamera(m);
	}

	//return temp;
}

//returns modified polygon list
void Object::TransformToPerspective(Matrix& m)
{
	for(std::list<Poly>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		it->TransformToPerspective(m);
	}

	//return temp;
}

//returns modified polygon list
void Object::TransformToPixel(Matrix& m)
{
	for(std::list<Poly>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		it->TransformToPixel(m);
	}

	//return temp;
}

//returns final render list
std::list<Poly*> Object::getRenderList()
{
	std::list<Poly*> get;
	for(std::list<Poly>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		if( it->visible &&
#ifndef CLIPPING_TEST
			it->v[0].z > 0 &&
			it->v[1].z > 0 &&
			it->v[2].z > 0 
#else
				(
				it->v[0].z > 0 ||
				it->v[1].z > 0 ||
				it->v[2].z > 0 ||
				it->v[3].z > 0
				)
#endif
			)
		{
			get.push_back(&(*it));
		}
	}

	//updateList(get);
	
	return get;
}