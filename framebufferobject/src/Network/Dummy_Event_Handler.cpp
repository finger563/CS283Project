// $Id$
//
// Author: William Emfinger
// Date  : Created: Feb 21, 2012
//
//
// Dummy_Event_Handler implementation file.
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
#include "Dummy_Event_Handler.h"

Player_c player;

ACE_Time_Value period_t (1);


// constructor (pass a pointer to the reactor). By default we assume
// a system-wide default reactor that ACE defines
Dummy_Event_Handler::Dummy_Event_Handler (ACE_Reactor *r)
// simply pass on the pointer to the reactor to the underlying event
// handler 
  : ACE_Event_Handler (r)
{
}


// destructor (does nothing)
Dummy_Event_Handler::~Dummy_Event_Handler (void)
{
}

// initialization method that registers ourselves with the reactor. 
int Dummy_Event_Handler::open (string server_ip)
{
#if defined(DEBUG)
  // for debugging
  cout << "Dummy_Event_Handler::open invoked" << endl;
#endif
  
	// initialize the address data structure. 
	if (server_addr_.set (server_ip.c_str()) == -1) {
		ACE_ERROR ((LM_ERROR,
					ACE_TEXT ("[%t] Player : Network Thread - "),
					ACE_TEXT ("failed to set IP address and port (%m)\n")));
		return 1;
	}
  
	// now let the client connect to the server.  Note how the connect
	// factory method creates a SOCK_Stream for us. Note that since the
	// third parameter (which is a timeout) is zero, we do a blocking
	// connect. 
	if (this->connector_.connect (this->peer_, server_addr_) == -1) {
		ACE_ERROR ((LM_ERROR,
					ACE_TEXT ("[%t] Player : Network Thread - "),
					ACE_TEXT ("cannot connect to server (%m)\n")));
		return 1;
	}

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
										// in reading data
								ACE_Event_Handler::TIMER_MASK | ACE_Event_Handler::READ_MASK) == -1) {
		ACE_ERROR ((LM_ERROR,
					ACE_TEXT ("[%P] Dummy_Event_Handler::open - "),
					ACE_TEXT ("failed to register handler (%m)\n")));
		return -1;
	}

	// disable non blocking I/O
	this->peer_.disable (ACE_NONBLOCK);

	this->reactor()->schedule_timer(this,
									0,
									period_t);

	// REGISTER THE PLAYER WITH THE SERVER
	Message mymessage;
	mymessage.Type(REGISTER);
	mymessage.Player(player.Info());
	player.Register();
	this->send(mymessage);

	// everything went well. Return success
	return 0;
}

/* now define the event handler's callback methods  */

// handle incoming data
int Dummy_Event_Handler::handle_input (ACE_HANDLE h)
{
#if defined(DEBUG)
  // for debugging
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("Dummy_Event_Handler::handle_input invoked\n")));
#endif
  
	Message mymessage;
  ssize_t bytesReceived = this->recv_message(mymessage);

  // note that unless there is a well defined protocol between the two
  // ends we never 

  if (bytesReceived == -1) { 
    // this is an error condition
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("[%P] Dummy_Event_Handler::handle_input - "),
                ACE_TEXT ("failed on recv (%m)\n")));
    // we let the reactor trigger the handle close
	return -1;
  } 
  else 
  {
#if defined(DEBUG)
	  ACE_DEBUG ((LM_DEBUG,
				  ACE_TEXT ("Client received %d bytes.\n"),
				  bytesReceived));
    // some data is received.
		cout << "Message type: " << mymessage.Type() << endl
			<< "Object type: " << mymessage.Object().type << endl
			<< "Object ID: " << mymessage.Object().id << endl
			<< "Player Name: " << mymessage.Player().name << endl
			<< "Player ID: " << mymessage.Player().id << endl
			<< "Message Content: " << mymessage.Content() << endl;
#endif
		// Now process message
		switch (mymessage.Type())
		{
		case ACCEPT:		// the server has accepted us
			break;
		case CREATE:		// server has sent a create command for an object
			char str[3];
			switch (mymessage.Object().type)
			{
			case PLAYER:
				sprintf(str,"Player");
				break;
			case SHOT:
				sprintf(str,"Shot");
				break;
			default:
				break;
			}
			ACE_DEBUG ((LM_DEBUG,
					ACE_TEXT ("Server created Object: (%s,%d) is: %s\n"),
					str,
					mymessage.Object().id,
					mymessage.Object().content_));
			break;
		case CHAT:
			break;
		case MOVE:
			break;
		case REMOVE:
			player.Remove(mymessage.Object().id);
			ACE_DEBUG ((LM_DEBUG,
					ACE_TEXT ("Class has been dismissed!\n")));
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

int Dummy_Event_Handler::send(const Message& message)
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
	
#if defined(DEBUG)
	ACE_DEBUG ((LM_DEBUG,
		ACE_TEXT ("Client sent %d bytes.\n"),
		length));	
	ACE_DEBUG ((LM_DEBUG,
		ACE_TEXT ("Client sent %d bytes of header: %s\n"),
		length, header.begin()->rd_ptr()));

	ACE_DEBUG ((LM_DEBUG,
		ACE_TEXT ("Client sent %d bytes of message: %s\n"),
		length, payload.begin()->rd_ptr()));
#endif
	return peer_.sendv_n (iov, 2);
}


int Dummy_Event_Handler::recv_message(Message& message)
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

#if defined(DEBUG)
		ACE_DEBUG ((LM_DEBUG,
					ACE_TEXT ("Recieved message of length: %d\n"),
					length));
#endif
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
			ACE_DEBUG ((LM_DEBUG,
						ACE_TEXT ("can't convert length!\n")));
	}
	payload->release();
	mblk->release();
	payload = mblk = 0;
	return -1;
}

// handle timeout events.
int Dummy_Event_Handler::handle_timeout (const ACE_Time_Value & current_time, const void * act)
{	
#if defined(DEBUG)
	// for debugging
	ACE_DEBUG ((LM_DEBUG,
				ACE_TEXT ("Dummy_Event_Handler::handle_timeout invoked\n")));
#endif
	
	Message mymessage;
	mymessage.Player(player.Info());	// need to make sure message contains info

	if (mymessage.FormMessage(false))		// false because I am a player
	{
		
		switch(mymessage.Type())
		{
		case REGISTER:
			if ( player.Registered() ) {
				ACE_DEBUG ((LM_DEBUG,
							ACE_TEXT ("Already Registered!\n")));
				break;
			}
			player.Register();
			this->send(mymessage);
			break;
		case SHOOT:
		case CHAT:
			if ( ! player.Registered() ) {
				ACE_DEBUG ((LM_DEBUG,
							ACE_TEXT ("Need to be registered first!\n")));
				break;
			}
			if ( ! player.Question(mymessage.Object()) )
				break;	// exit out, invalid assignment 
			this->send(mymessage);
			break;
		default:
			break;
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
int Dummy_Event_Handler::handle_close (ACE_HANDLE, ACE_Reactor_Mask mask)
{
  // for debugging
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("Dummy_Event_Handler::handle_close invoked\n")));

  mask = ACE_Event_Handler::ALL_EVENTS_MASK |
	  ACE_Event_Handler::DONT_CALL;
  this->reactor()->remove_handler(this,mask);
  
  // time for us to cleanup. First cleanup the OS socket resources
  // that are held by the SOCK_Stream data member. Then follow up with
  // deallocating the memory allocated to hold us.
  this->peer_.close ();

  this->reactor()->end_reactor_event_loop();

  return 0;
}

// the following method must be defined since the reactor needs to
// keep a mapping between the underlying handle and the
// corresponding handler.  Note that it is defined as a const method
// since it does not modify any state.
ACE_HANDLE Dummy_Event_Handler::get_handle (void) const
{
  // since the ACE_SOCK_Stream class inherits from the ACE_SOCK
  // class, we simply invoke the get_handle method on the acceptor
  // data member which is the variable peer_
  return this->peer_.get_handle ();
} 

// return a reference to the underlying peer.  This is a required method
// used by the acceptor in its accept () method. Please see how it is
// used in the handle_input of the Dummy_Accept_Handler.
ACE_SOCK_Stream & Dummy_Event_Handler::peer (void)
{
  return this->peer_;
}


