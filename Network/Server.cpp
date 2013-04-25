
#include "Server.h"

#include <list>
#include "..\Engine\Object.h"

extern std::list<peer_s> con_peers;

// Do collision detection here
bool Server_c::DetectCollide(const Object_s& s) {
	Object myobj;
	switch ( s.type ) {
	case PLAYER:
		myobj.GeneratePlayer(Point3D(s.x,s.y,s.z),s.theta,s.phi);
		break;
	case SHOT:
		myobj.GenerateShot(Point3D(s.x,s.y,s.z),s.theta,s.phi);
		break;
	default:
		break;
	}
	myobj.updateList();
	myobj.TranslateTemp(myobj.GetPosition());
	Message mymessage;
	Object playerobj;
	if ( s.type == SHOT ) {
		for (std::list<Object_s>::iterator it=objects.begin(); it != objects.end(); it ++) {	// only do dynamic-dynamic object collision on shots
			if ( it->type == PLAYER ) {	// don't do shot collision with shots
				playerobj.GeneratePlayer(Point3D(it->x,it->y,it->z),it->theta,it->phi);
				if ( playerobj.CollidesWith(myobj) ) {
					it->life = it->life - 20;
					return true;
				}
			}
		}
		
		for (std::list<Object>::iterator it = staticlist.begin(); it != staticlist.end(); it++) {
				if ( it->CollidesWith(myobj) )
					return true;
		}
	}
	return false;
}