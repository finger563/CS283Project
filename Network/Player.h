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

//#define DEBUG
#include <list>
#include "Message.h"
#include "..\Engine\chat.h"
#include "..\Engine\camera.h"
#include "..\Engine\world.h"

extern Camera tempeye;					// for update from server and input handling
extern std::list<Object> objectlist;	// for update from server

class Player_c {
private:
	bool registered;
	Player_s	info;
	std::list<Object_s> objects;
	Chat_c		chat;
	Camera		eye;
	World		level;
public:
	Player_c() : info(), chat(), eye() {registered=false;}
	Player_c(const Player_s& s): info(s), chat(), eye() {registered=false;}
	Player_c(Player_c& s){*this=s;}
	~Player_c() {}

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

	std::list<Object_s> Objects() {return objects;}

	Chat_c Chat() const { return chat; }
	void Chat(const Chat_c& c) { chat = c; }
	void AddChat(const string& str) {
		chat.addMessage(str);
	}

	Camera Eye() const { return eye; }
	void Eye(const Camera& e) { eye = e; }

	World Level() const { return level; }
	void Level(const World& l) { level = l; objectlist = level.GetRenderList(); }
	void Level(const long id) { level = World(id); objectlist = level.GetRenderList(); }

	void Register() {registered=true;}
	void Leave()	{registered=false;}
	bool Registered() {return registered;}

	void Create(Object_s& a) {
		objects.push_back(a);
	}

	void Move(Object_s& a) {
		
		if ( a.id == info.id && a.type == PLAYER ) {
			eye.SetPosition(a.x,a.y,a.z);
			eye.SetAngles(a.theta,a.phi);
			tempeye = eye;
		}
		else {
			for (std::list<Object_s>::iterator it = objects.begin(); it!= objects.end(); it++) {
				if ( it->id == a.id  && it->type == a.type ) {
					it->SetPos(a.x,a.y,a.z);
					it->SetHeading(a.theta,a.phi);
					it->SetLife(a.life);
					it->SetVelocity(a.vx,a.vy,a.vz);
					return;
				}
			}
		}
	}

	void Update(const float time) {
		for (std::list<Object_s>::iterator it = objects.begin(); it!= objects.end(); it++) {
			if ( it->type != PLAYER ) {
				it->Update(time);
			}
		}
	}

	void Remove(const ObjectType t, const ACE_CDR::Long _id) {
		for (std::list<Object_s>::iterator it = objects.begin(); it!= objects.end();) {
			if ( it->type == t && it->id == _id ) {
				it = objects.erase(it);
			}
			else
				it++;
		}
	}
};

#endif /* _CS283_PLAYER_H_s */