// $Id$
//
// Original Author: Aniruddha Gokhale
// Author: William Emfinger
// Date Created : Feb 17, 2007
// Date Modified : April 17, 2013
// 
// We demonstrate the use of the reactor framework in conjunction with
// the acceptor-connector framework
//
// Code is based on C++NPv1 and C++NPv2 books
//

#if !defined (_CS387_DUMMY_DATA_HANDLER_H_)
#define _CS387_DUMMY_DATA_HANDLER_H_

#include <ace/INET_Addr.h>        // for addressing
#include <ace/Event_Handler.h>    // for event handling
#include <ace/Reactor.h>          // for reactor functionality
#include <ace/SOCK_Stream.h>      // for data comm functionality

#include "Message.h"

class Dummy_Data_Handler : public ACE_Event_Handler
{
public:
  Dummy_Data_Handler (ACE_Reactor *r = ACE_Reactor::instance ());

  virtual int open (void);

  // Network Helper functions
  void		SendMessage( Message& m, long id = -1 );
  Player_s	AcceptPlayer( Player_s& myplayer );
  Object_s	CreatePlayer( Player_s& myplayer );
  void		SendObjects( );
  Object_s	CreateShot( Player_s& myplayer );

  // Reactor Callback Functions
  virtual int	handle_input (ACE_HANDLE h = ACE_INVALID_HANDLE);
  int			handle_timeout (const ACE_Time_Value & current_time, const void * act);
  virtual int	handle_close (ACE_HANDLE h = ACE_INVALID_HANDLE,
                              ACE_Reactor_Mask = 0);
  
  int send(ACE_SOCK_Stream &p, const Message &message);
  int recv_message ( Message& message);

  // the following method must be defined since the reactor needs to
  // keep a mapping between the underlying handle and the
  // corresponding handler.
  virtual ACE_HANDLE get_handle (void) const;

  // need access to the underlying sock stream so it can be updated by
  // the accept command.  Note, we get a reference to it.
  ACE_SOCK_Stream &peer (void);

protected:

  // the destructor is declared in protected section which compels us
  // to dynamically allocate an instance of this class. This way we will
  // not make a mistake in the handle_input method of the Accept_Handler
  // class. See comments in that method.
  ~Dummy_Data_Handler (void);

private:
  ACE_SOCK_Stream peer_;	// maintain a handle to the peer
  ACE_CDR::Long myid;		// id of player connected to us
};

#endif /* _CS387_DUMMY_DATA_HANDLER_H_ */



