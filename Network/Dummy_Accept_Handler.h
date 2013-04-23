// $Id$
//
// Author: Aniruddha Gokhale
// CS283 Class Sample Code, now used for CS 387.
// Date Created : Feb 17, 2007
// Date Updated : Feb 21, 2009
// Date Updated : Feb 9, 2012
//
//
// Dummy_Accept_Handler header file.
// 
// We demonstrate the use of the reactor framework in conjunction with
// the acceptor-connector framework
//
// Code is based on C++NPv1 and C++NPv2 books
//

// notice how we protect double inclusion of declarations by virtue of
// having these preprocessor symbols
#if !defined (_CS387_DUMMY_ACCEPT_HANDLER_H_)
#define _CS387_DUMMY_ACCEPT_HANDLER_H_

#include <ace/INET_Addr.h>        // for addressing
#include <ace/Event_Handler.h>    // for event handling
#include <ace/Reactor.h>          // for reactor functionality
#include <ace/SOCK_Acceptor.h>    // for acceptor functionality
#include <ace/SOCK_Stream.h>      // for data comm functionality

// we make our application-specific acceptor be an event handler that
// we register with the reactor. We maintain an underlying
// ACE_Acceptor. The purpose of having an event handler for the acceptor
// is because we want even the connection establishment messages to show
// up as an event that the reactor can recognize and make an upcall onto
// us.
class Dummy_Accept_Handler : public ACE_Event_Handler
{
public:
  // constructor (pass a pointer to the reactor). By default we assume
  // a system-wide default reactor that ACE defines
  Dummy_Accept_Handler (ACE_Reactor *r = ACE_Reactor::instance ());

  // the destructor
  ~Dummy_Accept_Handler (void);

  // define an initialization method that sets the interface and port
  // on which we are going to make our service available to clients.
  // Since this does not change, we are passing it as a const reference.
  //
  virtual int open (const ACE_INET_Addr &local_addr);

  /* now define the event handler's callback methods which are what will
     get dispatched by the underlying reactor */

  // handle incoming events.  Note that this is the method that will get
  // dispatched when a new connection establishment message shows up.
  // What gets passed to us as the parameter is the handle (socket) on
  // which the event occurred.
  virtual int handle_input (ACE_HANDLE h = ACE_INVALID_HANDLE);

  // handle clean up events. The parameters are what the reactor
  // framework will pass us when it calls us 
  virtual int handle_close (ACE_HANDLE h = ACE_INVALID_HANDLE,
                            ACE_Reactor_Mask = 0);

  // the following method must be defined since the reactor needs to
  // keep a mapping between the underlying handle and the
  // corresponding handler.  Note that it is defined as a const method
  // since it does not modify any state.
  virtual ACE_HANDLE get_handle (void) const;

private:

  // maintain a handle to the acceptor.  We adopt a strategy to use an
  // underscore after the variable for private data members of a
  // class.  The "get_handle" method shown above will retrieve the
  // handle from the acceptor_ data member.
  ACE_SOCK_Acceptor acceptor_;

};

#endif /* _CS387_DUMMY_ACCEPT_HANDLER_H_ */



