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

peer_s con_peers;

static ACE_CDR::Long numPlayers = 0;
static ACE_CDR::Long numObjects = 0;
const int SHOTSPEED = 15;
const float SHOTLIFE = 5.0;	// measured in seconds
const float PLAYERLIFE = 100.0;		// measured in HP
	
//ACE_Time_Value period_t (1,500000);		// update every 30,000 us = 30 ms.
ACE_Time_Value period_t (0,50000);		// update every 30,000 us = 30 ms.

// constructor (pass a pointer to the reactor). By default we assume
// a system-wide default reactor that ACE defines
Dummy_Data_Handler::Dummy_Data_Handler (ACE_Reactor *r)
// simply pass on the pointer to the reactor to the underlying event
// handler 
  : ACE_Event_Handler (r)
{
}


// destructor (does nothing)
Dummy_Data_Handler::~Dummy_Data_Handler (void)
{
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
  
	this->reactor()->schedule_timer(this,
									0,
									period_t);

	// everything went well. Return success
	return 0;
}

/* now define the event handler's callback methods  */

// handle incoming data
int Dummy_Data_Handler::handle_input (ACE_HANDLE h)
{
#if defined(DEBUG)
	// for debugging
	ACE_DEBUG ((LM_DEBUG,
				ACE_TEXT ("Dummy_Data_Handler::handle_input invoked\n")));
#endif
	// here is where we receive all data. In our case this is just a dummy
	// program where we receive some data and we send a reply back.
	string reply;

	// note that unless there is a well defined protocol between the two
	// ends we never 
	//ssize_t bytesReceived = this->peer_.recv (buffer, 1024);

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

	} else {
		// some data is received.
		// Now process message
		switch (mymessage.Type())
		{
		case REGISTER:
			myid = numPlayers++;
			myplayer = mymessage.Player();
			myplayer.SetID(myid);
			if ( server.Register(myplayer) )
			{
				mymessage.Type(ACCEPT);
				myplayer.SetHeading(0,0);
				myplayer.SetLife(PLAYERLIFE);
				myplayer.SetPos(10,0,10);
				mymessage.Player(myplayer);
				int numbytes = this->send(this->peer(),mymessage);
				#if defined(DEBUG)
				ACE_DEBUG ((LM_DEBUG,
					ACE_TEXT ("Server ACCEPTed player (%s,%d).\n"),
					myplayer.name,
					myplayer.id));
				#endif
				
				peer_s *tmp = &con_peers;
				mymessage.Type(CREATE);
				myplayer = mymessage.Player();
				myobject.SetID(myid);
				myobject.SetType(PLAYER);
				myobject.SetHeading(myplayer.theta,myplayer.phi);
				myobject.SetLife(PLAYERLIFE);
				myobject.SetPos(myplayer.x,myplayer.y,myplayer.z);
				myobject.SetContent(myplayer.name);
				server.Create(myobject);		// need to keep track of this object on the server
				mymessage.Object(myobject);
				while (tmp->next!=NULL) {		// Let other clients know of this new client
					tmp = tmp->next;
					int numbytes = this->send(*(tmp->p),mymessage);
					#if defined(DEBUG)
					ACE_DEBUG ((LM_DEBUG,
						ACE_TEXT ("Server sent CREATE player to player (%s,%d).\n"),
						server.Player(tmp->ID),
						tmp->ID));
					#endif
				}
				
				Object_s* myobjects = server.Objects();
				mymessage.Type(CREATE);
				while ( myobjects != NULL ) {	// Need to send CREATE messages to this client for all objects
					if ( myobjects->id != myid ||
						 myobjects->type != PLAYER ) {
						myobject.SetType(myobjects->type);
						myobject.SetID(myobjects->id);
						myobject.SetHeading(myobjects->theta,myobjects->phi);
						myobject.SetLife(myobjects->life);
						myobject.SetPos(myobjects->x,myobjects->y,myobjects->z);
						myobject.SetContent(myobjects->content_);
						mymessage.Object(myobject);
						int numbytes = this->send(this->peer(),mymessage);
						#if defined(DEBUG)
						ACE_DEBUG ((LM_DEBUG,
							ACE_TEXT ("Server sent CREATE to player (%s,%d).\n"),
							server.Player(tmp->ID),
							tmp->ID));
						#endif
					}
					myobjects = myobjects->next;		
				}
				peer_s *newpeer = new peer_s(&(this->peer()),myid);	// Have finished updating all clients
				con_peers.Push(newpeer);							// now add the new peer to our list
			}
			break;
		case CHAT:
			if ( true ) {//server.Player(mymessage.Player()) ) {	// Player is registered, propagate their chat
				peer_s *tmp = &con_peers;
				while (tmp->next!=NULL) {	// don't need to alter message, it is already chat!
					tmp = tmp->next;
					if ( tmp->ID != myid ) {
						int numbytes = this->send(*(tmp->p),mymessage);
#if defined(DEBUG)
						ACE_DEBUG ((LM_DEBUG,
							ACE_TEXT ("Server propagated CHAT to player (%s,%d).\n"),
							server.Player(tmp->ID),
							tmp->ID));
#endif
					}
				}
			}
			break;
		case SHOOT:
			if ( server.Player(mymessage.Player()) ) {
#if defined(DEBUG)
				ACE_DEBUG ((LM_DEBUG,
							ACE_TEXT ("%s fired a shot!\n"),
							mymessage.Player().name));
#endif
				peer_s *tmp = &con_peers;
				mymessage.Type(CREATE);
				myplayer = mymessage.Player();
				myobject.SetID(numObjects++);
				myobject.SetType(SHOT);
				myobject.SetHeading(myplayer.theta,myplayer.phi);
				myobject.SetLife(SHOTLIFE);
				myobject.SetVelocity(0,0,SHOTSPEED);		// velocity w.r.t. object's u/v/n axes
				myobject.SetPos(myplayer.x,myplayer.y,myplayer.z);
				myobject.SetContent(myplayer.name);
				server.Create(myobject);		// need to keep track of this object on the server
				mymessage.Object(myobject);
				while (tmp->next!=NULL) {
					tmp = tmp->next;
					int numbytes = this->send(*(tmp->p),mymessage);
#if defined(DEBUG)
					ACE_DEBUG ((LM_DEBUG,
						ACE_TEXT ("Server sent CREATE object to player (%s,%d).\n"),
						server.Player(tmp->ID),
						tmp->ID));
#endif
				}
			}
			break;
		case MOVE:
			if ( server.Object(mymessage.Object()) ) {
				peer_s *tmp = &con_peers;
				myobject = Object_s(mymessage.Object());
				server.Move(myobject);		// need to update server's state
				while (tmp->next!=NULL) {	// and also send move to clients
					tmp = tmp->next;
					int numbytes = this->send(*(tmp->p),mymessage);
#if defined(DEBUG)
					ACE_DEBUG ((LM_DEBUG,
						ACE_TEXT ("Server sent MOVE object to player (%s,%d).\n"),
						server.Player(tmp->ID),
						tmp->ID));
#endif
				}
			}
			break;
		case LEAVE:
			if ( server.Player(mymessage.Player()) ) {
				//server.RemovePlayer(mymessage.Player().id);	
				//server.RemoveObject(PLAYER,mymessage.Player().id);
				//server.RemovePlayer(myid);
				//server.RemoveObject(PLAYER,myid);
				//con_peers.Remove(myid);
				myobject = Object_s();
				myobject.SetID(mymessage.Player().id);
				myobject.SetType(PLAYER);
				mymessage.Type(REMOVE);		// Send remove to players
				mymessage.Object(myobject);
				peer_s *tmp = &con_peers;
				while (tmp->next!=NULL) {	// and also send remove to clients
					tmp = tmp->next;
					if ( tmp->ID != mymessage.Player().id ) {
						int numbytes = this->send(*(tmp->p),mymessage);
#if defined(DEBUG)
						ACE_DEBUG ((LM_DEBUG,
							ACE_TEXT ("Server sent REMOVE object to player (%s,%d).\n"),
							server.Player(tmp->ID),
							tmp->ID));
#endif
					}
				}
			}
			break;
		default:
			ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("Error: Not a valid message type!\n")));
			break;
		}
	}
	// success
	return 0;
}

int Dummy_Data_Handler::send(ACE_SOCK_Stream &p,const Message& message)
{
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

int Dummy_Data_Handler::recv_message(Message& message)
{
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

// handle timeout events.
// THIS IS WHERE THE STATE UPDATE & TRANSMISSION OCCURS
// When the server needs to update each player with new object positions, etc.
// this is the function timeout which will handle that.
int Dummy_Data_Handler::handle_timeout (const ACE_Time_Value & current_time, const void * act)
{	
	#if defined(DEBUG)
	// for debugging
	ACE_DEBUG ((LM_DEBUG,
				ACE_TEXT ("Dummy_Data_Handler::handle_timeout invoked\n")));
	#endif

	Message mymessage;
	if ( server.Players() != NULL )	{
		if ( server.Objects() != NULL ) {
			float time = period_t.usec()/1000000.0;
			time += period_t.sec();
			Object_s* myobjects = server.Objects();
			while ( myobjects != NULL ) {
				if ( !myobjects->Update(time) ) {	// object is no longer alive, need to remove
					peer_s *tmp = &con_peers;
					while (tmp->next != NULL) {
						tmp = tmp->next;
						mymessage.Type(REMOVE);
						mymessage.Object(*myobjects);
						int numbytes = this->send(*(tmp->p),mymessage);
#if defined(DEBUG)
						ACE_DEBUG ((LM_DEBUG,
							ACE_TEXT ("Server sent REMOVE object to player (%s,%d).\n"),
							server.Player(tmp->ID),
							tmp->ID));
#endif
					}
				}
				myobjects = myobjects->next;
			}
		}
	}

	this->reactor()->schedule_timer(this,
									0,
									period_t);

	return 0;
}

// handle clean up events. The parameters are what the reactor framework
// will pass us when it calls us.  For this code we really do not care
// about these parameters. But complex applications could do various
// things such as deregistering the handler only for that mask and when
// the mask becomes 0, then delete ourself.
int Dummy_Data_Handler::handle_close (ACE_HANDLE h, ACE_Reactor_Mask m)
{
#if defined(DEBUG)
	// for debugging
	ACE_DEBUG ((LM_DEBUG,
				ACE_TEXT ("Dummy_Data_Handler::handle_close invoked\n")));
#endif
	// Stop and cancel the periodic timer associated with this reactor
	this->reactor()->cancel_timer(this);
	server.RemovePlayer(myid);
	server.RemoveObject(PLAYER,myid);
	con_peers.Remove(myid);

	m = ACE_Event_Handler::ALL_EVENTS_MASK |
		ACE_Event_Handler::DONT_CALL;
	this->reactor()->remove_handler(this,m);

	// time for us to cleanup. First cleanup the OS socket resources
	// that are held by the SOCK_Stream data member. Then follow up with
	// deallocating the memory allocated to hold us.
	this->peer_.close ();
  
	//this->reactor()->end_reactor_event_loop();

	// notice how we clean up the allocated memory by deallocating
	// ourselves :-). 
	//
	// Deallocation is absolutely necessary since recall that we had
	// dynamically allocated ourselves. Also note that there can never be
	// a case where we are allocated on the stack because our destructor
	// was declared protected.
	delete this;

	return 0;
}

// the following method must be defined since the reactor needs to
// keep a mapping between the underlying handle and the
// corresponding handler.  Note that it is defined as a const method
// since it does not modify any state.
ACE_HANDLE Dummy_Data_Handler::get_handle (void) const
{
#if defined(DEBUG)
  // for debugging
  cout << "Dummy_Data_Handler::get_handle invoked" << endl;
#endif
  // since the ACE_SOCK_Stream class inherits from the ACE_SOCK
  // class, we simply invoke the get_handle method on the acceptor
  // data member which is the variable peer_
  return this->peer_.get_handle ();
} 

// return a reference to the underlying peer.  This is a required method
// used by the acceptor in its accept () method. Please see how it is
// used in the handle_input of the Dummy_Accept_Handler.
ACE_SOCK_Stream & Dummy_Data_Handler::peer (void)
{
#if defined(DEBUG)
  // for debugging
  cout << "Dummy_Data_Handler::peer invoked" << endl;
#endif
  return this->peer_;
}



