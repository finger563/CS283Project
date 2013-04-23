// $Id$
//
// Author: Aniruddha Gokhale
// CS283 Network Class Sample Code; Now used for CS387
// Date Created : Feb 17, 2007
// Date Modified : Feb 9, 2012
//
//
// Dummy_Data_Handler header file.
// 
// We demonstrate the use of the reactor framework in conjunction with
// the acceptor-connector framework
//
// Code is based on C++NPv1 and C++NPv2 books
//

// notice how we protect double inclusion of declarations by virtue of
// having these preprocessor symbols
#if !defined (_CS387_DUMMY_DATA_HANDLER_H_)
#define _CS387_DUMMY_DATA_HANDLER_H_

#include <ace/INET_Addr.h>        // for addressing
#include <ace/Event_Handler.h>    // for event handling
#include <ace/Reactor.h>          // for reactor functionality
#include <ace/SOCK_Stream.h>      // for data comm functionality

#include "Message.h"


// our data communication handler inherits from ACE_Event_Handler so
// that it can be registered with the Reactor.  We also maintain a
// SOCK_Stream object (Bridge design pattern).
class Dummy_Data_Handler : public ACE_Event_Handler
{
public:
  // constructor (pass a pointer to the reactor). By default we assume
  // a system-wide default reactor that ACE defines
  Dummy_Data_Handler (ACE_Reactor *r = ACE_Reactor::instance ());

  // define an initialization method 
  virtual int open (void);

  /* now define the event handler's callback methods.  These are the
     methods that we will get dispatched on the events */

  // handle incoming events. The event we are interested in is the
  // incoming data.
  virtual int handle_input (ACE_HANDLE h = ACE_INVALID_HANDLE);

  int send(ACE_SOCK_Stream &p, const Message &message);
  int recv_message ( Message& message);

  int handle_timeout (const ACE_Time_Value & current_time, const void * act);

  // handle clean up events. The parameters are what the reactor
  // framework will pass us when it calls us 
  virtual int handle_close (ACE_HANDLE h = ACE_INVALID_HANDLE,
                            ACE_Reactor_Mask = 0);

  // the following method must be defined since the reactor needs to
  // keep a mapping between the underlying handle and the
  // corresponding handler.  Note that it is defined as a const method
  // since it does not modify any state.
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

  // maintain a handle to the peer.  We adopt a strategy to use an
  // underscore after the variable for private data members of a
  // class. 
  ACE_SOCK_Stream peer_;

  ACE_CDR::Long myid;

};

#endif /* _CS387_DUMMY_DATA_HANDLER_H_ */



