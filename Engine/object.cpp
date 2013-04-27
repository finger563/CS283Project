#include "object.h"


//Constructor
Object::Object()
{
	velocity = Vector3D(0,0,0);
	position = Point3D(0,0,0);
	kill = false;
	counter = 0;
	radius = 0;
}

//Alternate texture, veloctity, heading, position
Object::Object(const unsigned short* texture, const int texWid, const int texHgt, Vector3D vel, Point3D pos, float r)
{
	velocity = vel;
	position = pos;
	tex = texture;
	texWidth = texWid;
	texHeight = texHgt;
	radius = r;
}

//Alternate Constructor
Object::Object(Poly poly, const unsigned short* texture, const int texWid, const int texHgt, Vector3D vel, 
		Point3D pos, float r)
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
	radius = r;
}

//generate() method switch statements??

//Updates Temp list with any changes to the master list
bool Object::updateList() {
	temp.clear();

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

void Object::Rotate(Matrix& m) {
	for(std::list<Poly>::iterator it = master.begin(); it != master.end(); ++it)
	{
		it->Transform(m);
	}
}

void Object::Translate(Vector3D& v) {
	for(std::list<Poly>::iterator it = master.begin(); it != master.end(); ++it)
	{
		it->Translate(v.x,v.y,v.z);
	}
}

void Object::RotateToHeading() {
	float r = cos(phi);
	float x = r*sin(theta),
			y = sin(phi),
			z = r*cos(theta);
	Vector3D forward = normalize(Vector3D(x,y,z));
	Vector3D up = normalize(Vector3D(0,1,0));
	Vector3D right = normalize(Cross(up,forward));
	up = normalize(Cross(forward,right));
	Matrix m = Matrix();
	m.data[0][0] = right.x;
	m.data[0][1] = right.y;
	m.data[0][2] = right.z;
	m.data[1][0] = up.x;
	m.data[1][1] = up.y;
	m.data[1][2] = up.z;
	m.data[2][0] = forward.x;
	m.data[2][1] = forward.y;
	m.data[2][2] = forward.z;
	Rotate(m);
}

void Object::RotateToHeading(Vector3D changeUp) {
	float r = cos(phi);
	float x = r*sin(theta),
			y = sin(phi),
			z = r*cos(theta);
	Vector3D forward = normalize(Vector3D(x,y,z));
	Vector3D up = normalize(changeUp);
	Vector3D right = normalize(Cross(up,forward));
	up = normalize(Cross(forward,right));
	Matrix m = Matrix();
	m.data[0][0] = right.x;
	m.data[0][1] = right.y;
	m.data[0][2] = right.z;
	m.data[1][0] = up.x;
	m.data[1][1] = up.y;
	m.data[1][2] = up.z;
	m.data[2][0] = forward.x;
	m.data[2][1] = forward.y;
	m.data[2][2] = forward.z;
	Rotate(m);
}

void Object::clearTemp() {
	//empties temp list
	temp.clear();
}

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

void Object::RotateTempToHeading() {
	float r = cos(phi);
	float x = r*sin(theta),
			y = sin(phi),
			z = r*cos(theta);
	Vector3D forward = normalize(Vector3D(x,y,z));
	Vector3D up = normalize(Vector3D(0,1,0));
	Vector3D right = normalize(Cross(up,forward));
	up = normalize(Cross(forward,right));
	Matrix m = Matrix();
	m.data[0][0] = right.x;
	m.data[0][1] = right.y;
	m.data[0][2] = right.z;
	m.data[1][0] = up.x;
	m.data[1][1] = up.y;
	m.data[1][2] = up.z;
	m.data[2][0] = forward.x;
	m.data[2][1] = forward.y;
	m.data[2][2] = forward.z;
	RotateTemp(m);
}

void Object::add(Poly poly) {
	master.push_back(poly);
	updateList();
}

void Object::GenerateCube(float size) {	  
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

	radius = size;
	theta = 3.141592;
	phi = 0;

	updateList(); 
}

void Object::GenerateTetra(float size) {
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

void Object::GenerateFloor(float length, float depth) {

	master.clear();
	master.push_back( Poly( Vertex(-length,0,-length,1,0,1),Vertex(-length,0,length,1,0,0),Vertex(length,0,length,1,1,0),
							Vertex(),3,Vector3D(0,1,0),TEXTURED));
	master.push_back( Poly( Vertex(-length,0,-length,1,0,1),Vertex(length,0,length,1,1,0),Vertex(length,0,-length,1,1,1),
							Vertex(),3,Vector3D(0,1,0),TEXTURED));

	for(std::list<Poly>::iterator it = master.begin(); it != master.end(); ++it)
	{
		it->SetTexture(tex, texWidth, texHeight);
	}

	radius = 0;
	theta = 0;
	phi = 3.141592/2.0;
	position = Point3D(0,depth,0);
	updateList();
}

//Incomplete...
void Object::GenerateCeiling(float length, float depth) {
	position = Point3D(0,depth,0);

	theta = 0;
	phi= 0;

	master.clear();
	master.push_back( Poly( Vertex(-length,0,-length,1,0,1),Vertex(-length,0,length,1,0,0),Vertex(length,0,length,1,1,0),
							Vertex(),3,Vector3D(0,1,0),TEXTURED));
	master.push_back( Poly( Vertex(-length,0,-length,1,0,1),Vertex(length,0,length,1,1,0),Vertex(length,0,-length,1,1,1),
							Vertex(),3,Vector3D(0,1,0),TEXTURED));

	for(std::list<Poly>::iterator it = master.begin(); it != master.end(); ++it)
	{
		it->SetTexture(tex, texWidth, texHeight);
	}

	radius = 0;

	RotateToHeading(Vector3D(0,-1,0));
	updateList(); 
}

//Generates wall
//Wall types: 0 = front: faces the player init
///////////// 1 = left: the left of player init
///////////// 2 = right: to the right of player init
///////////// 3 = back: behind player init
void Object::GenerateWall(size_t type, float length, float depth) {
	
	switch (type)
	{
		case 0: //front
			theta = 0;
			phi= 3.14/2;
			position = Point3D(0, depth,length);
			break;
		case 1: //left
			theta = -3.14/2;
			phi= 3.14/2;
			position = Point3D(-length, depth, 0);
			break;
		case 2: //right
			theta = 3.14/2;
			phi= 3.14/2;
			position = Point3D(length, depth,0);
			break;
		case 3: //behind
			theta = 3.14;
			phi= 3.14/2;
			position = Point3D(0, depth,-length);
			break;

	}

	

	master.clear();
	master.push_back( Poly( Vertex(-length,0,-length,1,0,1),Vertex(-length,0,length,1,0,0),Vertex(length,0,length,1,1,0),
							Vertex(),3,Vector3D(0,1,0),TEXTURED));
	master.push_back( Poly( Vertex(-length,0,-length,1,0,1),Vertex(length,0,length,1,1,0),Vertex(length,0,-length,1,1,1),
							Vertex(),3,Vector3D(0,1,0),TEXTURED));

	for(std::list<Poly>::iterator it = master.begin(); it != master.end(); ++it)
	{
		it->SetTexture(tex, texWidth, texHeight);
	}

	radius = 0;

	RotateToHeading();
	updateList(); 
}

void Object::GenerateShot(Vector3D pos, float theta_, float phi_) {
	GenerateCube(1.0);
	theta = theta_;
	phi = phi_;
	position = pos;
	SetRenderType(FLAT);
	RotateToHeading();
}
	
void Object::GeneratePlayer(Vector3D pos, float theta_, float phi_,const unsigned short* texture, const int texWid, const int texHgt) {
	tex = texture;
	texWidth = texWid;
	texHeight = texHgt;
	GenerateCube();
	theta = theta_;
	phi = phi_;
	position = pos;
	SetRenderType(TEXTURED);
	RotateToHeading();
	updateList();
}

bool Object::UpdateTime(int time) {
	return true;
}

bool Object::SetVelocity(const Vector3D& vector) {
	velocity = vector;
	return true;
}

bool Object::SetPosition(const Point3D& pos) {
	position = pos;
	return true;
}

Point3D Object::GetPosition(void) const {
	return position;
}

bool Object::SetRadius(float r) {
	radius = r;
	return true;
}

float Object::GetRadius(void) const {
	return radius;
}

bool Object::SetRenderType( RenderType rt ) {
	for(std::list<Poly>::iterator it = master.begin(); it != master.end(); ++it)
	{
		it->SetRenderType(rt);
	}
	return true;
}

void Object::TransformToCamera(Matrix& m) {
	for(std::list<Poly>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		it->TransformToCamera(m);
	}
}

void Object::TransformToPerspective(Matrix& m) {
	for(std::list<Poly>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		it->TransformToPerspective(m);
	}
}

void Object::TransformToPixel(Matrix& m) {
	for(std::list<Poly>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		it->TransformToPixel(m);
	}
}

//returns final render list
std::list<Poly> Object::GetRenderList() const {
	std::list<Poly> get;
	std::list<Poly> local = temp;
	for(std::list<Poly>::iterator it = local.begin(); it != local.end(); ++it)
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

std::list<Poly> Object::GetTemp() const {
	return temp;
}

////////////////////////////////////////
/////////////////Projectile functons////
///////////////////////////////////////
void Object::projectileInit(Vector3D head, Vector3D pos) {
	//may have to create overload this to take a vector based
	//on where the mouse clicks
	heading = head;
	position = pos;
	GenerateCube(1);
	counter=1;
}


bool Object::CollidesWith(const Object& b) {
	float distance = magnitude(b.GetPosition() - position);
	//if ( distance >= (radius + b.GetRadius()) )
	//	return false;
	if ( !updateList() )
		return false;
	TranslateTemp(position);
	std::list<Poly> blist = b.GetTemp();
	for (std::list<Poly>::iterator bpoly = blist.begin(); bpoly != blist.end(); ++bpoly) {	// for each poly of b
		Vector3D p0 = Vector3D(bpoly->v[0].x,bpoly->v[0].y,bpoly->v[0].z),
				 p1 = Vector3D(bpoly->v[1].x,bpoly->v[1].y,bpoly->v[1].z),
				 p2 = Vector3D(bpoly->v[2].x,bpoly->v[2].y,bpoly->v[2].z);
		for(std::list<Poly>::iterator it = temp.begin(); it != temp.end(); ++it) {	// for each poly in object
			Vector3D A = Vector3D(it->v[0].x,it->v[0].y,it->v[0].z),
					 B = Vector3D(it->v[1].x,it->v[1].y,it->v[1].z),
					 C = Vector3D(it->v[2].x,it->v[2].y,it->v[2].z);
			Vector3D p,n1,n2,n3;
			float t = (-it->normal * (p0 - A)) / (it->normal * (p1-p0));
			if ( t > 0 && t < 1 ) {
				p = p0 + (p1-p0)*t;
				n1 = normalize(Cross(A-B,p-B));
				n2 = normalize(Cross(B-C,p-C));
				n3 = normalize(Cross(C-A,p-A));
				if ( n1*n2 > 0 &&
					 n2*n3 > 0 )
					 return true;
			}
			else {
				t = (-it->normal * (p0 - A)) / (it->normal * (p2-p0));
				if ( t > 0 && t < 1 ) {
					p = p0 + (p2-p0)*t;
					n1 = normalize(Cross(A-B,p-B));
					n2 = normalize(Cross(B-C,p-C));
					n3 = normalize(Cross(C-A,p-A));
					if ( n1*n2 > 0 &&
						 n2*n3 > 0 )
						 return true;
				}
			}
		}
	}
	return false;
}