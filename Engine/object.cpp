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

	updateList(); 
}

//generates tetrahedron
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

//generates floor
void Object::GenerateFloor(float length, float depth) {
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

	radius = 0;

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
	if ( distance <= (radius + b.GetRadius()) )
		return true;
	updateList();
	//RotateTempToHeading();
	TranslateTemp(position);
	std::list<Poly> blist = b.GetRenderList();
	bool front = false,
			back = false;
	for(std::list<Poly>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		Vector3D itvector = Vector3D( it->normal.x,
									  it->normal.y,
									  it->normal.z,
									  - (it->normal.x*it->v[0].x + it->normal.y*it->v[0].y + it->normal.z*it->v[0].z) );
		for (std::list<Poly>::iterator bpoly = blist.begin(); bpoly != blist.end(); ++bpoly) {
			for ( int i=0;i<bpoly->numVertices;i++) {
				Vector3D test = Vector3D(bpoly->v[i].x,bpoly->v[i].y,bpoly->v[i].z);
				float tmp = test*itvector;
				if ( tmp < 0 )
					back = true;
				else if ( tmp > 0 )
					front = true;
			}
		}
	}
	if ( !front && back )
		return true;
	return false;
}