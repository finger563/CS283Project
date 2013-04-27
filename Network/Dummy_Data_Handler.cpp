// $Id$
//
// Author: Aniruddha Gokhale
// Date  : Created: Feb 17, 2007
//         Updated: Feb  4, 2008
//         Updated: Feb 21, 2009
//
//
// Dummy_Data_Handler implementation file.
// 
// We demonstrate the use of the reactor framework in conjunction with
// the acceptor-connector framework
//
// Code is based on C++NPv1 and C++NPv2 books
//

#include <iostream>
#include <string>
using namespace std;

// the following is used for logging of messages
#include "ace/Log_Msg.h"

// headers necessary for this file
#include "ace/OS_NS_string.h"
#include "Dummy_Data_Handler.h"
#include "Dummy_Accept_Handler.h"

#include "Server.h"

Server_c server;

std::list<peer_s> con_peers;

static ACE_CDR::Long numPlayers = 0;
static ACE_CDR::Long numObjects = 0;
	
ACE_Time_Value period_t (0,UPDATE_TIME);	// update period (seconds,microseconds)

Dummy_Data_Handler::Dummy_Data_Handler (ACE_Reactor *r) 
  : ACE_Event_Handler (r) {
}

Dummy_Data_Handler::~Dummy_Data_Handler (void) {
}

// initialization method that registers ourselves with the reactor. 
int Dummy_Data_Handler::open (void)
{
#if defined(DEBUG)
	// for debugging
	cout << "Dummy_Data_Handler::open invoked" << endl;
#endif

	// we need to register ourselves (note that we are of the type
	// Event_Handler) with the reactor. The event that we as a data
	// handler are interested in is incoming data, which is added using
	// the READ_MASK
	//
	// Note that in the constructor we had passed a pointer to the
	// reactor to our underlying event handler. We can retrieve that
	// pointer by invoking the "reactor ()" method. Invoke the register
	// handler method on that reactor
	if (this->reactor ()
		->register_handler (this,  // register ourselves with the reactor
									// indicating it that we are interested
									// in reading data and timout handling
							ACE_Event_Handler::TIMER_MASK | ACE_Event_Handler::READ_MASK) == -1) {
		ACE_ERROR ((LM_ERROR,
					ACE_TEXT ("[%P] Dummy_Data_Handler::open - "),
					ACE_TEXT ("failed to register handler (%m)\n")));
		return -1;
	}
  
	myid = numPlayers++;

	this->reactor()->schedule_timer(this,
									0,
									period_t);

	// everything went well. Return success
	return 0;
}

// NETWORK HELPER FUNCTIONS	/////////////////////////////////////////////////////////
peer_s GetPeer( long id ) {
	for (std::list<peer_s>::iterator it = con_peers.begin(); it != con_peers.end(); it++ ) {
		if ( it->ID == id ) {
			return *it;
		}
	}
}

void Dummy_Data_Handler::SendMessage( Message& m, long id ) {
	for (std::list<peer_s>::iterator it = con_peers.begin(); it != con_peers.end(); it++ ) {
		if ( it->ID != id ) {
			int numbytes = this->send(*(it->p),m);
		}
	}
}

Player_s Dummy_Data_Handler::AcceptPlayer( Player_s& myplayer ) {
	Message m;
	m.SetType(ACCEPT);
	m.SetWorld(server.Level());	// let player know which world to load
	myplayer.SetHeading(0,0);
	myplayer.SetLife(PLAYERLIFE);
	myplayer.SetPos(10,0,10);
	m.SetPlayer(myplayer);
	int numbytes = this->send(this->peer(),m);
	return myplayer;
}

Object_s Dummy_Data_Handler::CreatePlayer( Player_s& myplayer ) {
	Message m;
	m.SetType(CREATE);
	Object_s myobject;
	myobject.SetID(myid);
	myobject.SetType(PLAYER);
	myobject.SetHeading(myplayer.theta,myplayer.phi);
	myobject.SetLife(PLAYERLIFE);
	myobject.SetPos(myplayer.x,myplayer.y,myplayer.z);
	myobject.SetContent(myplayer.name);
	m.SetObject(myobject);
	//for (std::list<peer_s>::iterator mypeer = con_peers.begin(); mypeer != con_peers.end(); mypeer++ ) {		// Let other clients know of this new client
	//	int numbytes = this->send(*(mypeer->p),m);
	//}
	SendMessage(m);
	return myobject;
}

void Dummy_Data_Handler::SendObjects( ) {	// Need to send CREATE messages to this client for active dynamic objects
	Message m;
	Object_s myobject;
	m.SetType(CREATE);
	std::list<Object_s> objlist = server.Objects();
	for (std::list<Object_s>::iterator it=objlist.begin(); it != objlist.end(); it++) {
		if ( it->id != myid ||
				it->type != PLAYER ) {
			myobject.SetType(it->type);
			myobject.SetID(it->id);
			myobject.SetHeading(it->theta,it->phi);
			myobject.SetLife(it->life);
			myobject.SetPos(it->x,it->y,it->z);
			myobject.SetContent(it->content_);
			m.SetObject(myobject);
			int numbytes = this->send(this->peer(),m);
		}		
	}
}

Object_s Dummy_Data_Handler::CreateShot( Player_s& myplayer ) {
	Message m;
	m.SetType(CREATE);
	Object_s myobject;
	myobject.SetID(numObjects++);
	myobject.SetType(SHOT);
	myobject.SetHeading(myplayer.theta,myplayer.phi);
	float r = cos(myplayer.phi),
		x = r*sin(myplayer.theta),
		y = sin(myplayer.phi),
		z = r*cos(myplayer.theta);
	myobject.SetLife(SHOTLIFE);
	myobject.SetVelocity(0,0,SHOTSPEED);		// velocity w.r.t. object's u/v/n axes
	myobject.SetPos(myplayer.x + SHOTOFFSET*x,myplayer.y+SHOTOFFSET*y,myplayer.z+SHOTOFFSET*z);
	myobject.SetContent(myplayer.name);
	m.SetObject(myobject);
	SendMessage(m);
	return myobject;
}

// REACTOR CALLBACK FUNCTIONS	/////////////////////////////////////////////////////
int Dummy_Data_Handler::handle_input (ACE_HANDLE h) {
	#if defined(DEBUG)
	ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("Dummy_Data_Handler::handle_input invoked\n")));
	#endif
	Message mymessage;
	Player_s myplayer;
	Object_s myobject;
	ssize_t bytesReceived = this->recv_message(mymessage);
	  
	if (bytesReceived == 0) {
		// we need to handle this case since it would mean that the connection is
		// closed from the other side.  So it is time for this handler to go too.
		ACE_DEBUG ((LM_DEBUG,
					ACE_TEXT ("Dummy_Data_Handler::handle_input - 0 bytes received")
					ACE_TEXT (" => peer closed connection\n")));

		// we let the reactor trigger the handle close by returning a -1 to
		// the reactor
		return -1;

	} else if (bytesReceived == -1) {
		// this is an error condition
		ACE_ERROR ((LM_ERROR,
					ACE_TEXT ("%s force quit their client application!\n"),
					server.Player(myid)));
		// we let the reactor trigger the handle close
		return -1;

	} else {		// Now process message
		switch (mymessage.GetType())
		{
		case REGISTER:
			myplayer = mymessage.GetPlayer();
			myplayer.SetID(myid);
			if ( server.Register(myplayer) )
			{
				myplayer = AcceptPlayer( myplayer );
				#if defined(DEBUG)
				ACE_DEBUG ((LM_DEBUG,
					ACE_TEXT ("Server ACCEPTed player (%s,%d).\n"),
					myplayer.name, myplayer.id));
				#endif
				
				myobject = CreatePlayer( myplayer );	// Send CREATE message to connected players
				#if defined(DEBUG)
				ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("Server sent CREATE player to other players.\n"));
				#endif
				server.Create(myobject);		// need to keep track of this object on the server
				SendObjects();					// send all current objects to the new player
				peer_s newpeer = peer_s(&(this->peer()),myid);	// Have finished updating all clients
				con_peers.push_back(newpeer);					// now add the new peer to our list
			}
			break;
		case CHAT:	// don't mind if players who are unregistered chat (we'll pretend they're spectators)
			SendMessage(mymessage,myid);	// don't need to alter message, it is chat. Do not send to the player who sent it
			#if defined(DEBUG)
			ACE_DEBUG ((LM_DEBUG,
				ACE_TEXT ("Server propagated CHAT from player (%s,%d).\n"),
				server.Player(myid), myid));
			#endif
			break;
		case SHOOT:
			if ( server.Player(mymessage.GetPlayer()) ) {
				myplayer = mymessage.GetPlayer();
				#if defined(DEBUG)
				ACE_DEBUG ((LM_DEBUG,ACE_TEXT ("%s fired a shot!\n"), myplayer.name));
				#endif
				myobject = CreateShot( myplayer );
				server.Create(myobject);		// need to keep track of this object on the server
			}
			break;
		case MOVE:
			if ( server.ObjectExists(mymessage.GetObject()) ) {
				myobject = Object_s(mymessage.GetObject());
				myobject = server.Move(myobject);		// need to update server's state
				mymessage.SetObject(myobject);
				SendMessage(mymessage);					// update all clients (sender will receive life update)
				#if defined(DEBUG)
				ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("Server sent MOVE object to players.\n"));
				#endif
			}
			break;
		case LEAVE:
			if ( server.Player(mymessage.GetPlayer()) ) {
				myobject = Object_s();
				myobject.SetID(mymessage.GetPlayer().id);
				myobject.SetType(PLAYER);
				mymessage.SetType(REMOVE);
				mymessage.SetObject(myobject);
				SendMessage(mymessage,mymessage.GetPlayer().id);	// Send remove to other clients
			}
			break;
		default:
			ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("Error: Not a valid message type!\n")));
			break;
		}
	}
	return 0;
}

int Dummy_Data_Handler::send(ACE_SOCK_Stream &p,const Message& message) {
	size_t maxpayloadsize = message.Length();
	maxpayloadsize += ACE_CDR::MAX_ALIGNMENT;

	ACE_OutputCDR payload (maxpayloadsize);
	payload << message;
	ACE_CDR::ULong length = payload.total_length();

	ACE_OutputCDR header (8);
	header << ACE_OutputCDR::from_boolean (ACE_CDR_BYTE_ORDER);
	header << ACE_CDR::ULong (length);

	iovec iov[2];

	iov[0].iov_base = header.begin()->rd_ptr();
	iov[0].iov_len = 8;
	iov[1].iov_base = payload.begin()->rd_ptr();
	iov[1].iov_len = length;
	return p.sendv_n (iov, 2);
}

int Dummy_Data_Handler::recv_message(Message& message) {
#define MAXHOSTNAMELEN	100
	ACE_INET_Addr peer_addr;
	peer_.get_remote_addr(peer_addr);
	ACE_Message_Block* mblk = new ACE_Message_Block (MAXHOSTNAMELEN +1);
	peer_addr.get_host_name (mblk->wr_ptr(),MAXHOSTNAMELEN);
	mblk->wr_ptr(strlen (mblk->wr_ptr())+1);
	
	ACE_Message_Block *payload = new ACE_Message_Block (ACE_DEFAULT_CDR_BUFSIZE);
	ACE_CDR::mb_align (payload);

	if (this->peer_.recv_n (payload->wr_ptr(),8)==8) 
	{
		payload->wr_ptr(8);
		ACE_InputCDR cdr (payload);

		ACE_CDR::Boolean byte_order;
		cdr >> ACE_InputCDR::to_boolean(byte_order);
		cdr.reset_byte_order(byte_order);

		ACE_CDR::ULong length;
		cdr >> length;

		payload->size(8 + ACE_CDR::MAX_ALIGNMENT + length);
		
		if (peer_.recv_n (payload->wr_ptr(), length) > 0) {
			payload->wr_ptr (length);   // Reflect additional bytes.
			payload->rd_ptr (8);
			mblk->cont(payload);
			cdr = ACE_InputCDR(payload);
			cdr >> message;
			mblk->release();
			payload = mblk = 0;
			return length; // Return length of the message.
		}
		else 
			ACE_DEBUG ((LM_ERROR,
						ACE_TEXT ("can't convert length!\n")));
	}
	payload->release();
	mblk->release();
	payload = mblk = 0;
	return -1;
}

int Dummy_Data_Handler::handle_timeout (const ACE_Time_Value& current_time, const void* act) {	
	#if defined(DEBUG)
	ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("Dummy_Data_Handler::handle_timeout invoked\n")));
	#endif

	Message mymessage;
	Player_s myplayer;
	Object_s myobject;
	if ( con_peers.begin()->ID == myid ) {	// only want one handle_timeout running on the server
		if ( !server.Objects().empty() ) {	// do we have something to update?
			float time = period_t.usec()/1000000.0;
			time += period_t.sec();
			server.UpdateObjects(time);			// upate position of all shots
			std::list<Object_s> objlist = server.Objects();
			for (std::list<Object_s>::iterator it=objlist.begin(); it != objlist.end(); it++ ) {
				if ( it->life <= 0.0 ||
						server.DetectCollide(*it) ) {	// object is no longer alive, need to remove
					if ( it->type != PLAYER ) {
						mymessage.SetType(REMOVE);
						mymessage.SetObject(*it);
						SendMessage(mymessage);
						server.RemoveObject(it->type,it->id);
					}
					else if ( it->type == PLAYER ) {		// Player was killed by shots, need to respawn
						mymessage.SetType(CHAT);
						string killedstring = string(it->content_) + " was killed by " + string(it->killedby);
						mymessage.SetContent(killedstring.c_str());
						SendMessage(mymessage);
						it->life = PLAYERLIFE;
						mymessage.SetType(ACCEPT);
						mymessage.SetWorld(server.Level());
						myplayer.SetHeading(0,0);
						myplayer.SetID(it->id);
						myplayer.SetLife(PLAYERLIFE);
						myplayer.SetPos(10,0,10);
						myplayer.SetName(it->content_);
						mymessage.SetPlayer(myplayer);
						peer_s mypeer = GetPeer(myplayer.id);
						int numbytes = this->send(*(mypeer.p),mymessage);
						#if defined(DEBUG)
						ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("Server respawned player (%s,%d).\n"), myplayer.name, myplayer.id));
						#endif
				
						mymessage.SetType(MOVE);
						myobject.SetID(myplayer.id);
						myobject.SetType(PLAYER);
						myobject.SetHeading(myplayer.theta,myplayer.phi);
						myobject.SetLife(PLAYERLIFE);
						myobject.SetPos(myplayer.x,myplayer.y,myplayer.z);
						myobject.SetContent(myplayer.name);
						mymessage.SetObject(myobject);
						server.Move(myobject);
						server.SetObjectLife(it->type, it->id, PLAYERLIFE);
						SendMessage(mymessage,myplayer.id);
					}
				}
			}
		}
	}

	this->reactor()->schedule_timer(this,
									0,
									period_t);

	return 0;
}

int Dummy_Data_Handler::handle_close (ACE_HANDLE h, ACE_Reactor_Mask m) {
	#if defined(DEBUG)
	ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("Dummy_Data_Handler::handle_close invoked\n")));
	#endif
	// Stop and cancel the periodic timer associated with this reactor
	this->reactor()->cancel_timer(this);
	server.RemovePlayer(myid);
	server.RemoveObject(PLAYER,myid);
	for (std::list<peer_s>::iterator mypeer = con_peers.begin(); mypeer != con_peers.end(); ) {
		if ( mypeer->ID == myid ) {
			mypeer = con_peers.erase(mypeer);
		}
		else
			mypeer++;
	}

	m = ACE_Event_Handler::ALL_EVENTS_MASK |
		ACE_Event_Handler::DONT_CALL;
	this->reactor()->remove_handler(this,m);

	// time for us to cleanup. First cleanup the OS socket resources
	// that are held by the SOCK_Stream data member. Then follow up with
	// deallocating the memory allocated to hold us.
	this->peer_.close ();

	// Deallocation is absolutely necessary since recall that we had
	// dynamically allocated ourselves. Also note that there can never be
	// a case where we are allocated on the stack because our destructor
	// was declared protected.
	delete this;

	return 0;
}

ACE_HANDLE Dummy_Data_Handler::get_handle (void) const {
  return this->peer_.get_handle ();
} 

ACE_SOCK_Stream & Dummy_Data_Handler::peer (void) {
	return this->peer_;
}
