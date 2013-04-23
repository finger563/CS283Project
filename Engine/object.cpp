#include "object.h"


//Constructor
Object::Object()
{
	velocity = Vector3D(0,0,0);
	position = Point3D(0,0,0);
	kill = false;
	counter = 0;
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
bool Object::updateList() {
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
bool Object::updateList(std::list<Poly> poly) {
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
void Object::clearTemp() {
	//empties temp list
	temp.clear();
}

//Temp List Operations
void Object::RotateTemp(const Matrix& m) {
	for(std::list<Poly>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		it->Transform(m);
	}
}

void Object::TranslateTemp(const Vector3D& v) {
	for(std::list<Poly>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		it->Translate(v);
	}
}

//add polygon to lists
void Object::add(Poly poly) {
	master.push_back(poly);
	updateList();
}

//generates cube
void Object::generateCube(float size) {	  
	master.clear();

	//stores in objects master list
	master.push_back(Poly( Vertex(size, size, size,1,0,0),Vertex(-size, size, size,1,1,0),Vertex(size, -size, size,1,0,1),
						   Vertex(),3,Vector3D(0,0,1),TEXTURED));

	master.push_back(Poly( Vertex(size, -size, size,1,0,1), Vertex(-size, size, size,1,1,0),Vertex(-size, -size, size,1,1,1),
						   Vertex(),3,Vector3D(0,0,1),TEXTURED));

	master.push_back(Poly( Vertex(size, size, -size,1,1,0), Vertex(size, -size, -size,1,1,1),Vertex(-size, size, -size,1,0,0),
						   Vertex(),3,Vector3D(0,0,-1),TEXTURED));

	master.push_back(Poly( Vertex(size, -size, -size,1,1,1),Vertex(-size, -size, -size,1,0,1),Vertex(-size, size, -size,1,0,0),
						   Vertex(),3,Vector3D(0,0,-1),TEXTURED));

	master.push_back(Poly( Vertex(size, size, -size,1,0,0), Vertex(size, size, size,1,1,0),Vertex(size, -size, size,1,1,1),
						   Vertex(),3,Vector3D(1,0,0),TEXTURED));

	master.push_back(Poly( Vertex(size, size, -size,1,0,0),Vertex(size, -size, size,1,1,1),Vertex(size, -size, -size,1,0,1),
						   Vertex(),3,Vector3D(1,0,0),TEXTURED));

	master.push_back(Poly( Vertex(-size, size, -size,1,1,0),Vertex(-size, -size, -size,1,1,1),Vertex(-size, size, size,1,0,0),
						   Vertex(),3,Vector3D(-1,0,0),TEXTURED));

	master.push_back(Poly( Vertex(-size, size, size,1,0,0),Vertex(-size, -size, -size,1,1,1),Vertex(-size, -size, size,1,0,1),
						   Vertex(),3,Vector3D(-1,0,0),TEXTURED));

	master.push_back(Poly( Vertex(size, size, size,1,1,0),Vertex(size, size, -size,1,1,1),Vertex(-size, size, size,1,0,0),
						   Vertex(),3,Vector3D(0,1,0),TEXTURED));

	master.push_back(Poly( Vertex(-size, size, -size,1,0,1),Vertex(-size, size, size,1,0,0),Vertex(size, size, -size,1,1,1),
						   Vertex(),3,Vector3D(0,1,0),TEXTURED));

	master.push_back(Poly( Vertex(size, -size, size,1,1,1),Vertex(-size, -size, -size,1,0,0),Vertex(size, -size, -size,1,1,0),
						   Vertex(),3,Vector3D(0,-1,0),TEXTURED));

	master.push_back(Poly( Vertex(size, -size, size,1,1,1),Vertex(-size, -size, size,1,0,1),Vertex(-size, -size, -size,1,0,0),
						   Vertex(),3,Vector3D(0,-1,0),TEXTURED));

	for(std::list<Poly>::iterator it = master.begin(); it != master.end(); ++it)
	{
		it->SetTexture(tex, texWidth, texHeight);
		it->SetColor(rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,rand()/(float)RAND_MAX);
#if 0
		it->SetVertexColors(rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,
			rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,
			rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,rand()/(float)RAND_MAX);
#else
		it->SetVertexColors(0.9,0,0,
			0,0.9,0,
			0,0,0.9);
#endif
	}

	updateList(); 
}

//generates tetrahedron
void Object::generateTetra(float size) {
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
void Object::generateFloor(float length, float depth) {
	position = Point3D(0,depth,0);

	master.clear();
	master.push_back( Poly( Vertex(-length,0,-length,1,0,1),Vertex(-length,0,length,1,0,0),Vertex(length,0,length,1,1,0),
							Vertex(),3,Vector3D(0,1,0),TEXTURED));
	master.push_back( Poly( Vertex(-length,0,-length,1,0,1),Vertex(length,0,length,1,1,0),Vertex(length,0,-length,1,1,1),
							Vertex(),3,Vector3D(0,1,0),TEXTURED));

	for(std::list<Poly>::iterator it = master.begin(); it != master.end(); ++it)
	{
		it->SetTexture(tex, texWidth, texHeight);
	}

	updateList(); 
}

bool Object::updateTime(int time) {
	return true;
}

bool Object::setVel(Vector3D vector) {
	velocity = vector;
	return true;
}

bool Object::setPosition(Point3D pos) {
	position = pos;
	return true;
}

bool Object::SetRenderType( RenderType rt ) {
	for(std::list<Poly>::iterator it = master.begin(); it != master.end(); ++it)
	{
		it->SetRenderType(rt);
	}
	return true;
}

Point3D Object::getPosition(void) {
	return position;
}

//assumes that only the temp list is being passed through
void Object::Rotate(Matrix& m) {
	for(std::list<Poly>::iterator it = master.begin(); it != master.end(); ++it)
	{
		it->Transform(m);
	}
}

//assumes that only the temp list is being passed through
void Object::Translate(Vector3D& v) {
	for(std::list<Poly>::iterator it = master.begin(); it != master.end(); ++it)
	{
		it->Translate(v.x,v.y,v.z);
	}
}

//returns modified polygon list
void Object::TransformToCamera(Matrix& m) {
	for(std::list<Poly>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		it->TransformToCamera(m);
	}
}

//returns modified polygon list
void Object::TransformToPerspective(Matrix& m) {
	for(std::list<Poly>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		it->TransformToPerspective(m);
	}
}

//returns modified polygon list
void Object::TransformToPixel(Matrix& m) {
	for(std::list<Poly>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		it->TransformToPixel(m);
	}
}

//returns final render list
std::list<Poly> Object::getRenderList() {
	std::list<Poly> get;
	for(std::list<Poly>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		if( it->visible &&
				(
				it->v[0].z > 0 ||
				it->v[1].z > 0 ||
				it->v[2].z > 0 ||
				it->v[3].z > 0
				)
			)
		{
			get.push_back(*it);
		}
	}
	
	return get;
}

////////////////////////////////////////
/////////////////Projectile functons////
///////////////////////////////////////
void Object::projectileInit(Vector3D head, Vector3D pos) {
	//may have to create overload this to take a vector based
	//on where the mouse clicks
	heading = head;
	position = pos;
	generateCube(1);
	counter=1;
}

void Object::projectileMove() {
	position = position + heading;
	counter++;

	if(counter >= 25)
		kill = true;
}

bool Object::getKill() {
	return kill;
}

size_t Object::getCount() {
	return counter;
}

void Object::upCount() {
	counter++;
}