// $Id$
//
// Author: Aniruddha Gokhale
// Date  : Created: Feb 17, 2007
//         Updated: Feb  6, 2008
//         Updated: Feb  21, 2009
//         Updated: Feb  9, 2012
//
//
// Dummy_Accept_Handler implementation file.
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

// include the headers. We need the Data handler class here since our acceptor
// will create the Data Handler as part of the factory production process.
#include "Dummy_Data_Handler.h"

// The following is the header for our acceptor event handler
#include "Dummy_Accept_Handler.h"

// constructor (pass a pointer to the reactor). By default we assume
// a system-wide default reactor that ACE defines
Dummy_Accept_Handler::Dummy_Accept_Handler (ACE_Reactor *r)
// simply pass on the pointer to the reactor to the underlying event
// handler. 
  : ACE_Event_Handler (r)
{
}


// destructor (does nothing)
Dummy_Accept_Handler::~Dummy_Accept_Handler (void)
{
}

// define an initialization method that sets the interface and port
// on which we are going to make our service available to clients.
int Dummy_Accept_Handler::open (const ACE_INET_Addr &local_addr)
{
  // for debugging
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("Ready to accept players!\n")));

  // first initialize the underlying acceptor as we did before in the
  // previous sample code by calling the open method and passing the
  // INET_Addr object
  if (this->acceptor_.open (local_addr) == -1) {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("[%P] Dummy_Accept_Handler::open - "),
                ACE_TEXT ("failed to initialize acceptor (%m)\n")));
    return -1;
  }

  // now we need to register ourselves with the reactor. Note that we
  // are of the type Event_Handler. The event that we as an acceptor
  // are interested in is a connection establishment event and nothing
  // else.  Hence we must mention it in the "mask".  See how we do it.
  //
  // Note that in the constructor we had passed a pointer to the
  // reactor to our underlying event handler. We can retrieve that
  // pointer by invoking the "reactor ()" method. Invoke the register
  // handler method on that reactor because we want to register with
  // it. 
  if (this->reactor ()
      ->register_handler (this,  // register ourselves indicating
                                 // that we are interested only
                                 // in accepting connections
                          ACE_Event_Handler::ACCEPT_MASK) == -1) {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("[%P] Dummy_Accept_Handler::open - "),
                ACE_TEXT ("failed to register handler (%m)\n")));
    return -1;
  }

  // everything went well. Return success
  return 0;
}

/* now define the event handler's callback methods */

// handle incoming events
int Dummy_Accept_Handler::handle_input (ACE_HANDLE h)
{
  // since the only type of input event we will get is a connection
  // establishment event (recall the ACCEPT_MASK we used when
  // registering ourselves with the reactor), it is here that we
  // allocate a SOCK_Stream object

  // for debugging
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("New player is connecting!\n")));	//Dummy_Accept_Handler::handle_input invoked\n

  // Remember to dynamically allocate this object since otherwise it
  // will go out of scope at the end of the "handle_input" method. This is a
  // big mistake one can possibly make.  However, this mistake can be prevented
  // (and is indeed done so here) using a powerful feature of C++ wherein the
  // destructor is declared protected, which compels us to dynamically allocate
  // the object.
  Dummy_Data_Handler *data_handler = 0;
  
  // we use an ACE defined macro to dynamically allocate a data
  // handler object. Note that it too will get registered with the
  // same reactor as the acceptor. To ensure that we use the same
  // reactor, we simply retrieve it from the Accept_Handler object
  ACE_NEW_RETURN (data_handler,
                  Dummy_Data_Handler (this->reactor ()),
                  -1);

  // now do the "accept" action. Notice how we pass the underlying
  // SOCK_Stream variable by reference to the accept method
  if (this->acceptor_.accept (data_handler->peer ()) == -1) {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("[%P] Dummy_Accept_Handler::handle_input - "),
                ACE_TEXT ("accept failed (%m)\n")));
      return -1;
  }
  
  // the "peer" that was returned to us is already initialized with the
  // communication socket.  We ensure blocking semantics
  data_handler->peer ().disable (ACE_NONBLOCK);  // ensure blocking receives

  // now initialize the data handler object
  if (data_handler->open () == -1) {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("[%P] Dummy_Accept_Handler::handle_input - "),
                ACE_TEXT ("failed to initialize data handler (%m)\n")));
    // cleanup the peer which we had allocated
    (void) data_handler->handle_close ();
    return -1;
  }

  // go back to waiting for newer incoming request and let the data
  // handler do all the data communication
  return 0;
}

// handle clean up events. The parameters are what the reactor
// framework will pass us when it calls us 
int Dummy_Accept_Handler::handle_close (ACE_HANDLE h, ACE_Reactor_Mask m)
{
  // for debugging
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("Dummy_Accept_Handler::handle_close invoked\n")));

  // time for us to cleanup. Cleanup the OS socket resources
  // that are held by the acceptor data member. 
  this->acceptor_.close ();
  return 0;
}

// the following method must be defined since the reactor needs to
// keep a mapping between the underlying handle and the
// corresponding handler.  Note that it is defined as a const method
// since it does not modify any state.
ACE_HANDLE Dummy_Accept_Handler::get_handle (void) const
{
  // since the ACE_SOCK_Acceptor class inherits from the ACE_SOCK
  // class, we simply invoke the get_handle method on the acceptor
  // data member
  return this->acceptor_.get_handle ();
}




