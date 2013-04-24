
#include "Server.h"

#include <list>
#include "..\Engine\Object.h"

extern std::list<peer_s> con_peers;

// Do collision detection here
bool Server_c::DetectCollide(Object_s& s) {
	Object myobj;
	switch ( s.type ) {
	case PLAYER:
		break;
	case SHOT:
		break;
	default:
		break;
	}
	Message mymessage;
	for (std::list<Object_s>::iterator it=objects.begin(); it != objects.end(); it ++) {	// only do dynamic-dynamic object collision on shots
		if ( s.type == SHOT && it->type == PLAYER ) {	// don't do shot collision with shots
			Object playerobj;

		}
	}
		
	for (std::list<Object>::iterator it = staticlist.begin(); it != staticlist.end(); it++) {

	}
	return false;
}