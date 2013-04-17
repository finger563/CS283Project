// $Id$
//
// Author: William Emfinger
// Date  : Created: Feb 21, 2012
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
			// 1 -> x heading
			// 1 -> y heading
			// 1 -> z heading
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
			// 1 -> Player ID
		ret += 6 * sizeof(ACE_CDR::Float);
			// 1 -> x position
			// 1 -> y position
			// 1 -> z position
			// 1 -> x heading
			// 1 -> y heading
			// 1 -> z heading
		break;
	case CREATE:	// Server sends create message to players to create a dynamic object
	case MOVE:		// create and move send same data
		ret += 3 * sizeof(ACE_CDR::Long);
			// 1 -> Message type
			// 1 -> Object Type
			// 1 -> Object ID/ Player ID
		ret += 9 * sizeof(ACE_CDR::Float);
			// 1 -> x position
			// 1 -> y position
			// 1 -> z position
			// 1 -> x heading
			// 1 -> y heading
			// 1 -> z heading
			// 1 -> x velocity
			// 1 -> y velocity
			// 1 -> z velocity
		break;
	case LEAVE:		// Player sends request to leave server
		ret += 2 * sizeof(ACE_CDR::Long);
			// 1 -> Message type
			// 1 -> Player name length
		ret += strlen(player_.name);
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
		cdr << ACE_CDR::Long (m.Player().id);
		break;
	case ACCEPT:
	case CHAT:
	case SHOOT:
		cdr << ACE_CDR::Long (strlen(m.Player().name));
		cdr.write_char_array(m.Player().name,strlen(m.Player().name));
		cdr << ACE_CDR::Long (m.Player().id);
		cdr << ACE_CDR::Long (m.Object().type);
		cdr << ACE_CDR::Long (m.Object().id);
		cdr << ACE_CDR::Long (strlen(m.Content()));
		cdr.write_char_array(m.Content(),strlen(m.Content()));
		break;
	case CREATE:
		cdr << ACE_CDR::Long (strlen(m.Player().name));
		cdr.write_char_array(m.Player().name,strlen(m.Player().name));
		cdr << ACE_CDR::Long (m.Player().id);
		cdr << ACE_CDR::Long (m.Object().type);
		cdr << ACE_CDR::Long (m.Object().id);
		cdr << ACE_CDR::Long (strlen(m.Content()));
		//cdr.write_char_array(m.Content(),strlen(m.Content()));
		cdr.write_octet_array((ACE_CDR::Octet *)m.Content(),strlen(m.Content()));
		break;
	case MOVE:
		break;
	case LEAVE:
		break;
	case REMOVE:
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
	char name[MAX_NAME_LEN];
	char cont[MAX_CONT_LEN];

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
		cdr >> player_id;
		player = Player_s(name,(int)player_id);
		message.Player(player);
		break;
	case ACCEPT:
	case CHAT:
	case SHOOT:
		cdr >> name_len;
		cdr.read_char_array(name,name_len);
		name[name_len]='\0';
		cdr >> player_id;
		cdr >> object_type;
		cdr >> object_id;
		cdr >> cont_len;
		cdr.read_char_array(cont,cont_len);
		cont[cont_len]='\0';
		player = Player_s(name,(int)player_id);
		message.Player(player);
		object = Object_s((ObjectType)object_type,(int)object_id,cont);
		message.Object(object);
		message.Content(cont);
		break;
	case CREATE:
		cdr >> name_len;
		cdr.read_char_array(name,name_len);
		name[name_len]='\0';
		cdr >> player_id;
		cdr >> object_type;
		cdr >> object_id;
		cdr >> cont_len;
		cdr.read_octet_array((ACE_CDR::Octet *)cont,cont_len);
		cont[cont_len]='\0';
		player = Player_s(name,(int)player_id);
		message.Player(player);
		object = Object_s((ObjectType)object_type,(int)object_id,cont);
		message.Object(object);
		message.Content(cont);
		break;
	case MOVE:
		break;
	case LEAVE:
		break;
	case REMOVE:
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