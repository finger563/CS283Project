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
	Object_s	*objects;
	Player_s	*players;
	long		 worldID;	// which world is the server running?
	World		level;		// actual level which is loaded (for collision detection)
public:
	Server_c() {
		objects=NULL;players=NULL;worldID=0;
	}
	Server_c(Server_c& t) {*this=t;}
	~Server_c() {
		delete objects,players,this;
	}

	Server_c & operator=(Server_c& t) {
		if (this != &t) {
			objects = t.Objects();
			players = t.Players();
			worldID = t.worldID;
        }
        return *this;
	}

	Object_s * Objects() {return objects;}
	Player_s * Players() {return players;}
	long Level() { return worldID; }
	void Level(const long w) { worldID = w; level = World(w); }

	bool Player(Player_s& s) {
		Player_s* tmp = players;
		if (tmp != NULL) {
			while (tmp!=NULL) {
				if ( s.id == tmp->id ) 
					return true;
				tmp=tmp->next;
			}
		}
		ACE_ERROR ((LM_ERROR,
			ACE_TEXT ("Error, player has not registered!\n")));
		return false;
	}

	char* Player(ACE_CDR::Long id) {
		Player_s* tmp = players;
		while ( tmp != NULL) {
			if ( tmp->id == id ) {
				return tmp->name;
			}
			tmp = tmp->next;
		}

		ACE_ERROR ((LM_ERROR,
			ACE_TEXT ("Error, player has not registered!\n")));
		return NULL;
	}

	void RemovePlayer(ACE_CDR::Long id) {
		Player_s* tmp = players;
		Player_s* prev = tmp;
		while ( tmp != NULL) {
			if ( tmp->id == id ) {
				if ( tmp == players ) {
					players = tmp->next;
				}
				else {
					prev->next = tmp->next;
				}
				ACE_DEBUG ((LM_DEBUG,
						ACE_TEXT ("%s has been removed from players.\n"),
						tmp->name));
				delete tmp;
				return;
			}
			prev = tmp;
			tmp = tmp->next;
		}
	}
	
	bool Register(Player_s& s) {
		Player_s* tmp = players;
		Player_s* prev = tmp;
		while ( tmp != NULL) {
			if ( *tmp == s ) {
				ACE_ERROR ((LM_ERROR,
					ACE_TEXT ("Error, %s has already registered!\n"),
					s.name));
				return false;
			}
			prev = tmp;
			tmp = tmp->next;
		}
		if ( prev != NULL )
			prev->Link(new Player_s(s));
		else 
			players = new Player_s(s);
		ACE_DEBUG ((LM_DEBUG,
				ACE_TEXT ("%s is now registered.\n"),
				s.name));
		return true;
	}

	void Shutdown() {
		delete players,objects;
		players=NULL;
		objects=NULL;
	}

	bool Create(Object_s& a) {
		if ( objects==NULL) {
			objects=new Object_s(a);
			return true;
		}
		else {
			if ( a == *objects ) {
				ACE_ERROR ((LM_ERROR,
					ACE_TEXT ("Error, object has already been created!\n")));
				return false;
			}
			Object_s* tmp;
			for (tmp=objects;tmp->next!=NULL;tmp=tmp->next) {
				if ( a == *tmp ) {
					ACE_ERROR ((LM_ERROR,
						ACE_TEXT ("Error, object has already been created!\n")));
					return false;
				}
			}
			Object_s* link = new Object_s(a);
			tmp->Link(link);
			return true;
		}
	}
	
	void Move(Object_s& a) {
		Object_s* tmp = objects;
		while ( tmp != NULL) {
			if ( tmp->id == a.id ) {
				tmp->SetPos(a.x,a.y,a.z);
				tmp->SetHeading(a.theta,a.phi);
				// DO NOT UPDATE LIFE HERE, SERVER CONTROLS LIFE
				tmp->SetVelocity(a.vx,a.vy,a.vz);
				return;
			}
			tmp = tmp->next;
		}
	}
	
	bool ObjectExists(Object_s& s) {
		if (players != NULL) {
			if ( s == *objects )
				return true;
			Object_s* tmp=Objects();
			while (tmp!=NULL) {
				if ( s == *tmp ) 
					return true;
				tmp=tmp->next;
			}
		}
		ACE_ERROR ((LM_ERROR,
			ACE_TEXT ("Error, object has not been created!\n")));
		return false;
	}

	bool DetectCollide(Object_s& s);	// collision detection code
	
	void RemoveObject( ObjectType t, ACE_CDR::Long id ) {
		Object_s* tmp = objects;
		Object_s* prev = tmp;
		while ( tmp != NULL) {
			if ( tmp->id == id && tmp->type == t ) {
				if ( tmp == objects ) {
					objects = tmp->next;
				}
				else {
					prev->next = tmp->next;
				}
				ACE_DEBUG ((LM_DEBUG,
						ACE_TEXT ("%s has been removed from objects.\n"),
						tmp->content_));
				delete tmp;
				return;
			}
			prev = tmp;
			tmp = tmp->next;
		}
	}
};

struct peer_s {
	ACE_SOCK_Stream *p;
	peer_s *next;
	ACE_CDR::Long ID;

	peer_s() {p=NULL;next=NULL;ID=-1;}
	peer_s(ACE_SOCK_Stream* newpeer,ACE_CDR::Long id) {next=NULL;p=newpeer;ID=id;}

	void Push(peer_s *newpeer) {
		peer_s *tmp = this;
		while (tmp->next!=NULL) {
			tmp=tmp->next;
		}
		tmp->next = newpeer;
	}
	
	void Remove(ACE_CDR::Long id) {
		peer_s *tmp = this;
		peer_s *prev = this;
		while (tmp->next != NULL) {
			prev = tmp;
			tmp = tmp->next;
			if ( tmp->ID == id ) {
				prev->next = tmp->next;
				delete tmp;
				return;
			}
		}
	}

	void RemoveALL() {
		delete next;
		next = NULL;
	}
};

#endif /* _CS283_SERVER_H_ */