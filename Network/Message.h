// $Id$
//
// Author: William Emfinger
// CS283 Project: Super Block Fighter Extreme
// Date Created : April 16, 2013
//
//
// Message header file.
// 
// We define the message format for the SBFE application
//

#if !defined (_SBFE_MESSAGE_H_)
#define _SBFE_MESSAGE_H_

//#define DEBUG		// used for printing out messages received

#include <stdio.h>
#include <string.h>
#include <math.h>
// the following is used for logging of messages
#include "ace/Log_Msg.h"

#include <ace/CDR_Stream.h>

const int MAX_NAME_LEN=100;		// Max length of username
const int MAX_CONT_LEN=256;		// Max length of single chat message

enum MessageType {
	REGISTER,	// Players REGISTER with the server initially
	ACCEPT,		//	Then the server responds using ACCEPT with a player ID, and the initial position/heading
	CHAT,		// Players send CHAT messages with username and text to server, it relays the message to other players
	SHOOT,		// Players send SHOOT command to Server, it replies/relays CREATE message to *all* players
	CREATE,		// Server sends CREATE message when a new object is created by a player (shot, player, etc.)
	MOVE,		// Server sends MOVE message when a created object needs to be moved (i.e. shot update, player update, etc.)
				//	Players send MOVE messages to the server when the player needs to move (i.e. keyboard/mouse input)
	UPDATE,		// Server sends UPDATE periodically with a time differential so clients can synchronously update their object states
	LEAVE,		// Player sends LEAVE message when they disconnect from the server
	REMOVE		//	Then the server removes the player from it's list and sends REMOVE to remove the object from other players' games
};

enum ObjectType {	// These are the types of dynamic objects which need to be tracked by the server
	PLAYER, SHOT
};

struct Player_s {
	char			name[MAX_NAME_LEN];
	ACE_CDR::Long	id;
	float x,y,z,		// position vector
		  theta, phi,	// heading vector
		  life,			// time to live
		  vx,vy,vz;		// velocity vector
	Player_s*		next;
	
	Player_s() {next=NULL;id=0;memset(name,0,MAX_NAME_LEN);}
	Player_s(const Player_s &s) { 
		id = s.id;
		strcpy(name,s.name);
		x = s.x;
		y = s.y;
		z = s.z;
		theta = s.theta;
		phi = s.phi;
		life = s.life;
		vx = s.vx;
		vy = s.vy;
		vz = s.vz;
		next=NULL;
	}
	Player_s(char* n,ACE_CDR::Long i){next=NULL;id=i;memset(name,0,MAX_NAME_LEN);strcpy(name,n);}
	
	Player_s & operator=(const Player_s &s) {
		if (this != &s) {
			id = s.id;
			strcpy(name,s.name);
			x = s.x;
			y = s.y;
			z = s.z;
			theta = s.theta;
			phi = s.phi;
			life = s.life;
			vx = s.vx;
			vy = s.vy;
			vz = s.vz;
			next=NULL;
        }
        // by convention, always return *this
        return *this;
	}

	void SetName(const char* n) {memset(name,0,MAX_NAME_LEN);strcpy(name,n);}
	void SetID(ACE_CDR::Long i) {id = i;}
	void SetPos(const float _x, const float _y, const float _z) { x=_x;y=_y;z=_z;}
	void SetHeading(const float _t, const float _p) { theta = _t; phi = _p; }
	void SetLife(const float _l) { life = _l; }
	void SetVelocity(const float _x, const float _y, const float _z) { vx=_x;vy=_y;vz=_z;}
	
	void Link(Player_s* s){next = s;}
	
	bool operator==(const Player_s &b) {
		if ( id ==b.id && strcmp(name,b.name)==0 )
			return true;
		else
			return false;
	}
};

struct Object_s {
	ObjectType		type;
	ACE_CDR::Long	id;
	float x,y,z,		// position vector
		  theta,phi,	// heading vector
		  life,			// time to live
		  vx,vy,vz;		// velocity vector
	char			content_[MAX_CONT_LEN];
	Object_s*	next;
	
	Object_s() {next=NULL;type=SHOT;id=0;memset(content_,0,MAX_CONT_LEN);}
	Object_s(const Object_s &a) {
		type = a.type;
		id = a.id;
		strcpy(content_,a.content_);
		x = a.x;
		y = a.y;
		z = a.z;
		theta = a.theta;
		phi = a.phi;
		life = a.life;
		vx = a.vx;
		vy = a.vy;
		vz = a.vz;
		next=NULL;
	}
	Object_s(ObjectType t, ACE_CDR::Long i,char* c){next=NULL;type=t;id=i;strcpy(content_,c);}
	
	Object_s & operator=(const Object_s &a) {
		if (this != &a) {
			type = a.type;
			id = a.id;
			strcpy(content_,a.content_);
			x = a.x;
			y = a.y;
			z = a.z;
			theta = a.theta;
			phi = a.phi;
			life = a.life;
			vx = a.vx;
			vy = a.vy;
			vz = a.vz;
			next=NULL;
        }
        // by convention, always return *this
        return *this;
	}

	void SetID(ACE_CDR::Long i) {id = i;}
	void SetType(ObjectType t) { type = t; }
	void SetContent(const char* n) {memset(content_,0,MAX_NAME_LEN);strcpy(content_,n);}
	void SetPos(const float _x, const float _y, const float _z) { x=_x;y=_y;z=_z;}
	void SetHeading(const float _t, const float _p) { theta = _t; phi = _p; }
	void SetLife(const float _l) { life = _l; }
	void SetVelocity(const float _x, const float _y, const float _z) { vx=_x;vy=_y;vz=_z;}

	void Link(Object_s* a){next = a;}
	
	bool Update(const float time) {
		float tr = cos(phi);
		float tx = tr*sin(theta),
			  ty = sin(phi),
			  tz = tr*cos(theta);
		float mag = sqrt(tx*tx + ty*ty + tz*tz);
		tx = tx/mag;
		ty = ty/mag;
		tz = tz/mag;
		x += tx * vz * time;
		y += ty * vz * time;
		z += tz * vz * time;
		if ( type != PLAYER )
			life = life - time;
		return (life > 0);
	}

	bool operator==(const Object_s &b) {
		if ( id ==b.id && type == b.type )
			return true;
		else
			return false;
	}
};

class Message {
private:
	MessageType type_;
	float		t_;		// time differential for update
	Object_s	object_;
	Player_s	player_;
	char		content_[MAX_CONT_LEN];
public:
	Message() : object_(),player_() {
		type_ = REGISTER;
		t_ = 0;
		memset(content_,0,MAX_CONT_LEN);
	}
	Message(const Message &m) : object_(m.Object()),player_(m.Player()), t_(m.Time()) {
		this->type_ = m.Type();
		memset(content_,0,MAX_CONT_LEN);
		strcpy(this->content_,m.Content());
	}
	Message(MessageType t,Object_s &o,Player_s &p, char* c) : object_(o),player_(p) {
		this->type_ = t;
		memset(content_,0,MAX_CONT_LEN);
		strcpy(this->content_,c);
	}
	~Message(){}
	Message & operator=(const Message & m) {
		if (this != &m)  {
			type_ = m.Type();
			t_ = m.Time();
			object_ = m.Object();
			player_ = m.Player();
			strcpy(content_,m.Content());
        } 
        return *this;
	}

	size_t Length() const;

	void		Type(const MessageType type) {type_=type;}
	MessageType Type() const {return type_;}

	void	Time(const float time) { t_ = time; }
	float	Time() const { return t_; }

	void		Object(const Object_s& o) {object_=o;}
	Object_s	Object() const {return object_;}

	void		Player(const Player_s& p) {player_=p;}
	Player_s	Player() const {return player_;}

	const char*	Content() const {return content_;}
	void		Content(const char* c) {memset(content_,0,MAX_CONT_LEN);strcpy(content_,c);}

	bool FormMessage(bool isServer);
};


int operator<< (ACE_OutputCDR &cdr, const Message &message);
int operator>> (ACE_InputCDR &cdr, Message &message);

#endif /* _SBFE_MESSAGE_H_ */