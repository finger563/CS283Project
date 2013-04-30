// $Id$
//
// Author: William Emfinger
// Date  : Created: April 15, 2013
//
//
// Message implementation file.
// 

#include "Message.h"

#include <iostream>
using namespace std;

size_t Message::Length() const {
	size_t ret = 0;
	switch (type_)
	{
	case REGISTER:	// Player registers themselves (name) with server
		ret += 2 * sizeof(ACE_CDR::Long);
			// 1 -> Message type
			// 1 -> Player name length
		ret += strlen(player_.name);
		break;
	case ACCEPT:	// Server returns player ID number and starting position/heading
		ret += 2 * sizeof(ACE_CDR::Long);
			// 1 -> Message type
			// 1 -> World ID
			// 1 -> Player ID
		ret += 6 * sizeof(ACE_CDR::Float);
			// 1 -> x position
			// 1 -> y position
			// 1 -> z position
			// 1 -> theta
			// 1 -> phi
			// 1 -> life
		break;
	case CHAT:		// Player sends to server & server propagates to other Players
		ret += 2 * sizeof(ACE_CDR::Long);
			// 1 -> Message type
			// 1 -> Chat content length
		ret += strlen(content_);
		break;
	case SHOOT:		// Player sends position,heading to server, which will create the shot
		ret += 2 * sizeof(ACE_CDR::Long);
			// 1 -> Message type
			// 1 -> Player ID		- USED FOR KNOWING WHO KILLED WHO
			// 1 -> Player name length
		ret += strlen(player_.name);
		ret += 6 * sizeof(ACE_CDR::Float);
			// 1 -> x position
			// 1 -> y position
			// 1 -> z position
			// 1 -> theta
			// 1 -> phi
			// 1 -> life
		break;
	case CREATE:	// Server sends create message to players to create a dynamic object
	case MOVE:		// create and move send same data
		ret += 4 * sizeof(ACE_CDR::Long);
			// 1 -> Message type
			// 1 -> Object Type
			// 1 -> Object ID/ Player ID
			// 1 -> Object content length
		ret += 9 * sizeof(ACE_CDR::Float);
			// 1 -> x position
			// 1 -> y position
			// 1 -> z position
			// 1 -> theta
			// 1 -> phi
			// 1 -> life
			// 1 -> x velocity
			// 1 -> y velocity
			// 1 -> z velocity
		ret += strlen(object_.content_);
		break;
	case UPDATE:	// Server sends update time differential to all clients
		ret += sizeof(ACE_CDR::Long);
			// 1 -> Message type
		ret += sizeof(ACE_CDR::Float);
			// 1 -> time
		break;
	case LEAVE:		// Player sends request to leave server
		ret += 2 * sizeof(ACE_CDR::Long);
			// 1 -> Message type
			// 1 -> Player ID
		break;
	case REMOVE:	// Server notifies other Players to delete dynamic object
		ret += 3 * sizeof(ACE_CDR::Long);
			// 1 -> Message type
			// 1 -> Object Type
			// 1 -> Object ID/ Player ID
		break;
	}
	return ret;
}

int operator<< (ACE_OutputCDR &cdr, const Message &m) {
	cdr << ACE_CDR::Long (m.GetType());
	switch (m.GetType())
	{
	case REGISTER:
		cdr << ACE_CDR::Long (strlen(m.GetPlayer().name));
		cdr.write_char_array(m.GetPlayer().name,strlen(m.GetPlayer().name));
		break;
	case ACCEPT:
		cdr << ACE_CDR::Long ( m.GetWorld() );
		cdr << ACE_CDR::Long ( m.GetPlayer().id );
		cdr << ACE_CDR::Float ( m.GetPlayer().x );
		cdr << ACE_CDR::Float ( m.GetPlayer().y );
		cdr << ACE_CDR::Float ( m.GetPlayer().z );
		cdr << ACE_CDR::Float ( m.GetPlayer().theta );
		cdr << ACE_CDR::Float ( m.GetPlayer().phi );
		cdr << ACE_CDR::Float ( m.GetPlayer().life );
		break;
	case CHAT:
		cdr << ACE_CDR::Long ( strlen(m.GetContent()) );
		cdr.write_char_array(m.GetContent(),strlen(m.GetContent()));
		break;
	case SHOOT:
		cdr << ACE_CDR::Long (m.GetPlayer().id);
		cdr << ACE_CDR::Long (strlen(m.GetPlayer().name));
		cdr.write_char_array(m.GetPlayer().name,strlen(m.GetPlayer().name));
		cdr << ACE_CDR::Float ( m.GetPlayer().x );
		cdr << ACE_CDR::Float ( m.GetPlayer().y );
		cdr << ACE_CDR::Float ( m.GetPlayer().z );
		cdr << ACE_CDR::Float ( m.GetPlayer().theta );
		cdr << ACE_CDR::Float ( m.GetPlayer().phi );
		cdr << ACE_CDR::Float ( m.GetPlayer().life );
		break;
	case CREATE:
	case MOVE:
		cdr << ACE_CDR::Long (m.GetObject().type);
		cdr << ACE_CDR::Long (m.GetObject().id);
		cdr << ACE_CDR::Long (strlen(m.GetObject().content_));
		cdr << ACE_CDR::Float ( m.GetObject().x );
		cdr << ACE_CDR::Float ( m.GetObject().y );
		cdr << ACE_CDR::Float ( m.GetObject().z );
		cdr << ACE_CDR::Float ( m.GetObject().theta );
		cdr << ACE_CDR::Float ( m.GetObject().phi );
		cdr << ACE_CDR::Float ( m.GetObject().life );
		cdr << ACE_CDR::Float ( m.GetObject().vx );
		cdr << ACE_CDR::Float ( m.GetObject().vy );
		cdr << ACE_CDR::Float ( m.GetObject().vz );
		cdr.write_char_array(m.GetObject().content_,strlen(m.GetObject().content_));
		break;
	case UPDATE:
		cdr << ACE_CDR::Float (m.GetTime());
		break;
	case LEAVE:
		cdr << ACE_CDR::Long ( m.GetPlayer().id );
		break;
	case REMOVE:
		cdr << ACE_CDR::Long (m.GetObject().type);
		cdr << ACE_CDR::Long (m.GetObject().id);
		break;
	}

	return cdr.good_bit();
}

int operator>> (ACE_InputCDR &cdr, Message &message) {
	ACE_CDR::Long type;
	ACE_CDR::Long world;
	ACE_CDR::Long object_type;
	ACE_CDR::Long object_id;
	ACE_CDR::Long player_id;
	ACE_CDR::Long name_len;
	ACE_CDR::Long cont_len;
	ACE_CDR::Float x,y,z,
				   theta,phi,
				   life,
				   vx,vy,vz,
				   time;
	char name[MAX_NAME_LEN];
	char cont[MAX_CONT_LEN];
	memset(name,0,MAX_NAME_LEN);
	memset(cont,0,MAX_CONT_LEN);

	Player_s player;
	Object_s object;

	cdr >> type;
	message.SetType((MessageType)type);

	switch (type)
	{
	case REGISTER:
		cdr >> name_len;
		cdr.read_char_array(name,name_len);
		name[name_len]='\0';
		player = Player_s();
		player.SetName(name);
		message.SetPlayer(player);
		break;
	case ACCEPT:
		cdr >> world;
		cdr >> player_id;
		cdr >> x;
		cdr >> y;
		cdr >> z;
		cdr >> theta;
		cdr >> phi;
		cdr >> life;
		player = Player_s();
		player.SetID(player_id);
		player.SetPos(x,y,z);
		player.SetHeading(theta,phi);
		player.SetLife(life);
		message.SetPlayer(player);
		message.SetWorld(world);
		break;
	case CHAT:
		cdr >> cont_len;
		cdr.read_char_array(cont,cont_len);
		cont[cont_len]='\0';
		message.SetContent(cont);
		break;
	case SHOOT:
		cdr >> player_id;
		cdr >> name_len;
		cdr.read_char_array(name,name_len);
		name[name_len]='\0';
		cdr >> x;
		cdr >> y;
		cdr >> z;
		cdr >> theta;
		cdr >> phi;
		cdr >> life;
		player = Player_s();
		player.SetName(name);
		player.SetID(player_id);
		player.SetPos(x,y,z);
		player.SetHeading(theta,phi);
		player.SetLife(life);
		message.SetPlayer(player);
		break;
	case CREATE:
	case MOVE:
		cdr >> object_type;
		cdr >> object_id;
		cdr >> cont_len;
		cdr >> x;
		cdr >> y;
		cdr >> z;
		cdr >> theta;
		cdr >> phi;
		cdr >> life;
		cdr >> vx;
		cdr >> vy;
		cdr >> vz;
		cdr.read_char_array(cont,cont_len);
		cont[cont_len]='\0';
		object = Object_s((ObjectType)object_type,(int)object_id,cont);
		object.SetPos(x,y,z);
		object.SetHeading(theta,phi);
		object.SetLife(life);
		object.SetVelocity(vx,vy,vz);
		message.SetObject(object);
		message.SetContent(cont);
		break;
	case UPDATE:
		cdr >> time;
		message.SetTime(time);
		break;
	case LEAVE:
		cdr >> player_id;
		player = Player_s();
		player.SetID(player_id);
		message.SetPlayer(player);
		break;
	case REMOVE:
		cdr >> object_type;
		cdr >> object_id;
		object = Object_s();
		object.SetType((ObjectType)object_type);
		object.SetID(object_id);
		message.SetObject(object);
		break;
	}

	return cdr.good_bit();
}