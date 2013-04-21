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
using namespace std;

#include <ace/SOCK_Stream.h>      // for data comm functionality

#define DEBUG

class Server_c {
private:
	Object_s	*objects;
	Player_s	*players;
public:
	Server_c() {
		objects=NULL;players=NULL;
		//chats=NULL;
	}
	Server_c(Server_c& t) {*this=t;}
	~Server_c() {
		delete objects,players,this;
		//delete chats;
	}

	Server_c & operator=(Server_c& t)
	{
		if (this != &t) // protect against invalid self-assignment
        {
			objects = t.Objects();
			players = t.Players();
        }
        // by convention, always return *this
        return *this;
	}

	Object_s * Objects() {return objects;}
	Player_s * Players() {return players;}

	bool Player(Player_s& s) {
		if (players != NULL) {
			if ( s == *players )
				return true;
			Player_s* tmp=Players();
			while (tmp!=NULL) {
				if ( s == *tmp ) 
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
				prev->next = tmp->next;
				ACE_DEBUG ((LM_DEBUG,
						ACE_TEXT ("%s has been removed.\n"),
						tmp->name));
				if ( tmp != players )
					delete tmp;
				else {
					delete tmp;
					players = NULL;
				}
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
	
	bool Object(Object_s& s) {
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

	void UpdateObjects(const float t) {
		if (players != NULL) {
			Object_s* tmp=Objects();
			while (tmp!=NULL) {
				tmp->x += tmp->hx * t;
				tmp->y += tmp->hy * t;
				tmp->z += tmp->hz * t;
				tmp=tmp->next;
			}
		}
	}
};

struct peer_s {
	ACE_SOCK_Stream *p;
	peer_s *next, *prev;
	ACE_CDR::Long ID;

	peer_s() {p=NULL;next=prev=NULL;ID=-1;}
	peer_s(ACE_SOCK_Stream* newpeer,ACE_CDR::Long id) {next=prev=NULL;p=newpeer;ID=id;}

	void Push(peer_s *newpeer) {
		peer_s *tmp=this;
		while (tmp->next!=NULL)
			tmp=tmp->next;
		tmp->next = newpeer;
		newpeer->prev=tmp;
	}
	
	void Remove(ACE_CDR::Long id) {
		peer_s *tmp = this;
		peer_s* prev = this;
		while (tmp != NULL) {
			if ( tmp->ID == id ) {
				prev->next = tmp->next;
				if ( tmp->next != NULL )
					tmp->next->prev = prev;
				delete tmp;
				return;
			}
			tmp=tmp->next;
		}
	}

	void RemoveALL() {
		delete next;
		next = NULL;
	}
};

#endif /* _CS283_SERVER_H_ */