// $Id$
//
// Author: William Emfinger
// CS283 Project header file
// Date Created : April 16, 2013
//
//
// Dummy_Event_Handler header file.
// 

#if !defined (_CS283_DUMMY_EVENT_HANDLER_H_)
#define _CS283_DUMMY_EVENT_HANDLER_H_

#include <string>
using namespace std;

#include <ace/INET_Addr.h>        // for addressing
#include <ace/Event_Handler.h>    // for event handling
#include <ace/Reactor.h>          // for reactor functionality
#include <ace/SOCK_Stream.h>      // for data comm functionality
#include <ace/SOCK_Acceptor.h>    // for acceptor functionality
#include <ace/SOCK_Connector.h>

#include "..\..\..\SBFE_Server\Message.h"

#include "Player.h"

extern Player_c player;

// our data communication handler inherits from ACE_Event_Handler so
// that it can be registered with the Reactor.  We also maintain a
// SOCK_Stream object (Bridge design pattern).
class Dummy_Event_Handler : public ACE_Event_Handler
{
public:
  // constructor (pass a pointer to the reactor). By default we assume
  // a system-wide default reactor that ACE defines
  Dummy_Event_Handler (ACE_Reactor *r = ACE_Reactor::instance ());

  // define an initialization method 
  virtual int open (ACE_TCHAR *argv []);

  /* now define the event handler's callback methods.  These are the
     methods that we will get dispatched on the events */


  int recv_message( Message& message);
  int send(const Message& message);


  // handle incoming events. The event we are interested in is the
  // incoming data.
  virtual int handle_input (ACE_HANDLE h = ACE_INVALID_HANDLE);

  // handle clean up events. The parameters are what the reactor
  // framework will pass us when it calls us 
  virtual int handle_close (ACE_HANDLE h = ACE_INVALID_HANDLE,
                            ACE_Reactor_Mask = 0);
  
  // handle timeout events. The parameters are what the reactor
  // framework will pass us when it calls us 
  virtual int handle_timeout (const ACE_Time_Value & current_time,
							  const void * = 0);
  
  // the following method must be defined since the reactor needs to
  // keep a mapping between the underlying handle and the
  // corresponding handler.  Note that it is defined as a const method
  // since it does not modify any state.
  virtual ACE_HANDLE get_handle (void) const;

  // need access to the underlying sock stream so it can be updated by
  // the accept command.  Note, we get a reference to it.
  ACE_SOCK_Stream &peer (void);
  
  // No longer protected because there is only one instance, so no need
  // to dynamically allocate it.
  ~Dummy_Event_Handler (void);

private:
  // 
  ACE_INET_Addr   server_addr_;     // server address

  // 
  ACE_SOCK_Connector connector_;    // the connector role

  // maintain a handle to the peer.  We adopt a strategy to use an
  // underscore after the variable for private data members of a
  // class. 
  ACE_SOCK_Stream peer_;
};

#endif /* _CS283_DUMMY_EVENT_HANDLER_H_ */



