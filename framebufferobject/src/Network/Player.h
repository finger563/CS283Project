// $Id$
//
// Author: William Emfinger
// CS283 Project 
// Date Created : April 17,2013
//
//
// Player header file.
//

#if !defined (_CS283_PLAYER_H_)
#define _CS283_PLAYER_H_

#define DEBUG

#include "..\..\..\SBFE_Server\Message.h"
#include "..\Engine\chat.h"
#include "..\Engine\camera.h"

class Player_c {
private:
	bool registered;
	Player_s	info;
	Object_s*	objects;
	Chat_c		chat;
	Camera		eye;
public:
	Player_c() : info(), chat(), eye() {objects=NULL;registered=false;}
	Player_c(const Player_s& s): info(s), chat(), eye() {objects=NULL;registered=false;}
	Player_c(Player_c& s){*this=s;}
	~Player_c() {delete objects, this;}

	Player_c & operator=(Player_c& s) {
		if (this != &s) {
			info = s.Info();
			objects = s.Objects();
			chat = s.Chat();
			eye = s.Eye();
        }
		return *this;		// by convention, always return *this
	}

	Player_s Info() const {return info;}
	void Info(const Player_s& s) {info=s;}

	Object_s* Objects() {return objects;}

	Chat_c Chat() const { return chat; }
	void Chat(const Chat_c& c) { chat = c; }
	void AddChat(const string& str) {
		chat.addMessage(str);
	}

	Camera Eye() const { return eye; }
	void Eye(const Camera& e) { eye = e; }

	void Register() {registered=true;}
	void Leave()	{registered=false;}
	bool Registered() {return registered;}

	void Create(Object_s& a) {
		if (objects==NULL) {
			objects = new Object_s(a);
		}
		else {
			Object_s* tmp;
			for (tmp=objects;tmp->next!=NULL;tmp=tmp->next);
			Object_s* link = new Object_s(a);
			tmp->Link(link);
		}
	}

	void Move(Object_s& a) {
		Object_s* tmp = objects;
		Object_s* prev = tmp;
		while ( tmp != NULL) {
			if ( tmp->id == a.id ) {
				tmp->SetPos(a.x,a.y,a.z);
				tmp->SetHeading(a.hx,a.hy,a.hz);
				tmp->SetVelocity(a.vx,a.vy,a.vz);
				return;
			}
			prev = tmp;
			tmp = tmp->next;
		}
	}

	void Update(const float time) {
		Object_s* tmp = objects;
		while ( tmp != NULL) {
			tmp->Update(time);
			tmp = tmp->next;
		}
	}

	void Remove(const ACE_CDR::Long _id) {
		Object_s* tmp = objects;
		Object_s* prev = tmp;
		while ( tmp != NULL) {
			if ( tmp->id == _id ) {
				prev->next = tmp->next;
				if ( tmp != objects )
					delete tmp;
				else {
					delete tmp;
					objects = NULL;
				}
				return;
			}
			prev = tmp;
			tmp = tmp->next;
		}
	}
};

#endif /* _CS283_PLAYER_H_s */