// $Id$
//
// Author: William Emfinger
// Date Created : April 17, 2013
//
//
// Server class header file.
//

#if !defined (_CS283_SERVER_H_)
#define _CS283_SERVER_H_

#include "Message.h"
#include <string>

#include <iostream>
#include <list>
using namespace std;

#include <ace/SOCK_Stream.h>      // for data comm functionality

#include "..\Engine\Object.h"
#include "..\Engine\world.h"

//#define DEBUG

class Server_c {
private:
	std::list<Object_s> objects;
	std::list<Player_s> players;
	long		 worldID;				// which world is the server running?
	World		level;					// actual level which is loaded (for collision detection)
	std::list<Object> staticlist;		// list of static objects in the world (collision detection)
public:
	Server_c() {
		worldID=0;level=World(worldID);staticlist=level.getRenderList();
	}
	Server_c(Server_c& t) {*this=t;}
	~Server_c() { }

	Server_c & operator=(Server_c& t) {
		if (this != &t) {
			objects = t.Objects();
			players = t.Players();
			worldID = t.worldID;
			level = World(t.Level());
			staticlist = level.getRenderList();
        }
        return *this;
	}

	std::list<Object_s> Objects() {return objects;}
	std::list<Player_s> Players() {return players;}
	long Level() { return worldID; }
	void Level(const long w) { worldID = w; level = World(w); staticlist = level.getRenderList(); }

	bool Player(Player_s& s) {
		for (std::list<Player_s>::iterator it = players.begin(); it != players.end(); it++) {
			if ( it->id == s.id )
				return true;
		}
		return false;
	}

	char* Player(ACE_CDR::Long id) {
		for (std::list<Player_s>::iterator it = players.begin(); it != players.end(); it++) {
			if ( it->id == id )
				return it->name;
		}
		return NULL;
	}

	void RemovePlayer(ACE_CDR::Long id) {
		for (std::list<Player_s>::iterator it = players.begin(); it!= players.end();) {
			if ( it->id == id ) {
				ACE_DEBUG ((LM_DEBUG,
						ACE_TEXT ("%s has been removed from players.\n"),
						it->name));
				it = players.erase(it);
				return;
			}
			else
				it++;
		}
	}
	
	bool Register(Player_s& s) {
		for (std::list<Player_s>::iterator it = players.begin(); it != players.end(); it++) {
			if ( it->id == s.id )
				return false;
		}
		players.push_back(s);
		ACE_DEBUG ((LM_DEBUG,
				ACE_TEXT ("%s is now registered.\n"),
				s.name));
		return true;
	}

	bool Create(Object_s& a) {
		for (std::list<Object_s>::iterator it = objects.begin(); it != objects.end(); it++) {
			if ( it->id == a.id && it->type == a.type )
				return false;
		}
		objects.push_back(a);
		return true;
	}
	
	void Move(Object_s& a) {
		for (std::list<Object_s>::iterator it = objects.begin(); it != objects.end(); it++) {
			if ( it->id == a.id && it->type == a.type ) {
				it->SetPos(a.x,a.y,a.z);
				it->SetHeading(a.theta,a.phi);
				it->SetVelocity(a.vx,a.vy,a.vz);
				return;
			}
		}
	}
	
	bool ObjectExists(Object_s& a) {
		for (std::list<Object_s>::iterator it = objects.begin(); it != objects.end(); it++) {
			if ( it->id == a.id && it->type == a.type )
				return true;
		}
		return false;
	}

	void UpdateObjects(float time) {
		for (std::list<Object_s>::iterator it = objects.begin(); it != objects.end(); it++) {
			if ( it->type != PLAYER )
				it->Update(time);
		}
	}

	float getObjectLife(ObjectType t, ACE_CDR::Long id){
		for (std::list<Object_s>::iterator it = objects.begin(); it != objects.end(); it++) {
			if ( it->id == id && it->type == t )
				return it->life;
		}
		return -1;
	}

	void setObjectLife(ObjectType t, ACE_CDR::Long id, float _l) {
		for (std::list<Object_s>::iterator it = objects.begin(); it != objects.end(); it++) {
			if ( it->id == id && it->type == t )
				it->SetLife(_l);
		}
	}

	bool DetectCollide(Object_s& s);	// collision detection code
	
	void RemoveObject( ObjectType t, ACE_CDR::Long id ) {
		for (std::list<Object_s>::iterator it = objects.begin(); it!= objects.end();) {
			if ( it->id == id && it->type == t ) {
#if defined(DEBUG)
				ACE_DEBUG ((LM_DEBUG,
						ACE_TEXT ("(%d,%d,%s) has been removed from objects.\n"),
						it->type,it->id,it->content_));
#endif
				it = objects.erase(it);
				return;
			}
			else
				it++;
		}
	}
};

struct peer_s {
	ACE_SOCK_Stream *p;
	ACE_CDR::Long ID;

	peer_s() {p=NULL;ID=-1;}
	peer_s(ACE_SOCK_Stream* newpeer,ACE_CDR::Long id) {p=newpeer;ID=id;}
};

#endif /* _CS283_SERVER_H_ */