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
	cdr << ACE_CDR::Long (m.Type());
	switch (m.Type())
	{
	case REGISTER:
		cdr << ACE_CDR::Long (strlen(m.Player().name));
		cdr.write_char_array(m.Player().name,strlen(m.Player().name));
		break;
	case ACCEPT:
		cdr << ACE_CDR::Long ( m.Player().id );
		cdr << ACE_CDR::Float ( m.Player().x );
		cdr << ACE_CDR::Float ( m.Player().y );
		cdr << ACE_CDR::Float ( m.Player().z );
		cdr << ACE_CDR::Float ( m.Player().theta );
		cdr << ACE_CDR::Float ( m.Player().phi );
		cdr << ACE_CDR::Float ( m.Player().life );
		break;
	case CHAT:
		cdr << ACE_CDR::Long ( strlen(m.Content()) );
		cdr.write_char_array(m.Content(),strlen(m.Content()));
		break;
	case SHOOT:
		cdr << ACE_CDR::Long (m.Player().id);
		cdr << ACE_CDR::Long (strlen(m.Player().name));
		cdr.write_char_array(m.Player().name,strlen(m.Player().name));
		cdr << ACE_CDR::Float ( m.Player().x );
		cdr << ACE_CDR::Float ( m.Player().y );
		cdr << ACE_CDR::Float ( m.Player().z );
		cdr << ACE_CDR::Float ( m.Player().theta );
		cdr << ACE_CDR::Float ( m.Player().phi );
		cdr << ACE_CDR::Float ( m.Player().life );
		break;
	case CREATE:
	case MOVE:
		cdr << ACE_CDR::Long (m.Object().type);
		cdr << ACE_CDR::Long (m.Object().id);
		cdr << ACE_CDR::Long (strlen(m.Object().content_));
		cdr << ACE_CDR::Float ( m.Object().x );
		cdr << ACE_CDR::Float ( m.Object().y );
		cdr << ACE_CDR::Float ( m.Object().z );
		cdr << ACE_CDR::Float ( m.Object().theta );
		cdr << ACE_CDR::Float ( m.Object().phi );
		cdr << ACE_CDR::Float ( m.Object().life );
		cdr << ACE_CDR::Float ( m.Object().vx );
		cdr << ACE_CDR::Float ( m.Object().vy );
		cdr << ACE_CDR::Float ( m.Object().vz );
		cdr.write_char_array(m.Object().content_,strlen(m.Object().content_));
		break;
	case UPDATE:
		cdr << ACE_CDR::Float (m.Time());
		break;
	case LEAVE:
		cdr << ACE_CDR::Long ( m.Player().id );
		break;
	case REMOVE:
		cdr << ACE_CDR::Long (m.Object().type);
		cdr << ACE_CDR::Long (m.Object().id);
		break;
	}

	return cdr.good_bit();
}

int operator>> (ACE_InputCDR &cdr, Message &message) {
	ACE_CDR::Long type;
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
	message.Type((MessageType)type);

	switch (type)
	{
	case REGISTER:
		cdr >> name_len;
		cdr.read_char_array(name,name_len);
		name[name_len]='\0';
		player = Player_s();
		player.SetName(name);
		message.Player(player);
		break;
	case ACCEPT:
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
		message.Player(player);
		break;
	case CHAT:
		cdr >> cont_len;
		cdr.read_char_array(cont,cont_len);
		cont[cont_len]='\0';
		message.Content(cont);
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
		message.Player(player);
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
		message.Object(object);
		message.Content(cont);
		break;
	case UPDATE:
		cdr >> time;
		message.Time(time);
		break;
	case LEAVE:
		cdr >> player_id;
		player = Player_s();
		player.SetID(player_id);
		message.Player(player);
		break;
	case REMOVE:
		cdr >> object_type;
		cdr >> object_id;
		object = Object_s();
		object.SetType((ObjectType)object_type);
		object.SetID(object_id);
		message.Object(object);
		break;
	}

	return cdr.good_bit();
}

bool Message::FormMessage(bool isServer) {
	if (isServer) {
	}
	else {
	}
	return false;
}