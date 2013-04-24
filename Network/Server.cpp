
#include "Server.h"

#include <list>
#include "..\Engine\Object.h"

extern peer_s con_peers;

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
	Object_s* myobjects=Objects();
	while ( myobjects != NULL ) {	// only do dynamic-dynamic object collision on shots
		if ( s.type == SHOT && myobjects->type == PLAYER ) {	// don't do shot collision with shots
			Object playerobj;

			peer_s *tmp = &con_peers;
		}
		myobjects = myobjects->next;
	}
		
	for (std::list<Object>::iterator it = staticlist.begin(); it != staticlist.end(); it++) {

	}
	return false;
}