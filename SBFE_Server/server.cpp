// $Id$

/*
  Date: April 16, 2013

  Author: William Emfinger

  Updated for CS283 from Andy's CS 387 Sample Code.
*/

#include <iostream>
#include <string>

// the following is used for logging of messages
#include "ace/Log_Msg.h"

#include "ace/OS_NS_unistd.h"
//#include "ace/OS_NS_string.h"
#include "ace/INET_Addr.h"
#include "ace/Get_Opt.h"		// for parsing command line arguments

#include "ace/Select_Reactor.h"  // we use the Select_Reactor
#include "..\Network\Dummy_Accept_Handler.h"  // this is the event handler we need
#include "..\Network\Server.h"

using namespace std;

extern Server_c server;

ACE_INET_Addr   server_addr;			// server address
string ip_addr = "127.0.0.1:9999";		// IP address from command line

// command line parsing
int parse_args (int argc, ACE_TCHAR *argv[]) {
	int c;
	ACE_Get_Opt get_opt (argc, argv, "i:l:");
	while ((c = get_opt ()) != -1) {
		switch (c) {
		case 'i':
			ip_addr = get_opt.opt_arg ();
			break;
		case 'l':
			server.World(atoi(get_opt.opt_arg()));
			break;
		case '?':
		default:
			ACE_ERROR_RETURN ((LM_ERROR,
								"usage:  %s "
								"-i <IP address>:<port number> "
								"-l <Level ID number> "
								"\n",
								argv [0]),
								-1);
		}
	}
	return 0; // Have successfully parsed command line args
}

int ACE_TMAIN (int argc, ACE_TCHAR *argv [])
{
	// parse the arguments
	ACE_DEBUG ((LM_DEBUG,
				"Server: parse the args\n"));
	if (parse_args (argc,argv) == -1) {
		ACE_ERROR ((LM_ERROR, ACE_TEXT ("(%P|%t) %p\n"), 
					ACE_TEXT ("parse failed")));
		return 0;
	}
	
	// Set the IP address for the server
	if (server_addr.set (ip_addr.c_str()) == -1) {
		ACE_ERROR ((LM_ERROR,
					ACE_TEXT ("[%P] server - "),
					ACE_TEXT ("failed to set ip address (%m)\n")));
		return 1;
	}

#if 0
  /**********************  Correct and useful Code ******************
  /*  This piece of code is kept here to show you how we could create
   *  our own reactor using the Reactor's Bridge pattern design. 
   ***************************************************************/

  // now instantiate a reactor object.  In this sample, we show the
  // use of single threaded select based reactor. There are many other
  // reactors that ACE provides including one that is used with thread
  // pools, one that works with Windows' WaitForMultipleObjects, one
  // that works with the "poll" system call instead of the "select"
  // system call.
  ACE_Select_Reactor  select_reactor;
  
  // tell ACE that we want to use a select reactor object.  There is a
  // default second argument to the constructor which tells the
  // reactor class whether it should take ownership of cleaning up the
  // implementation. Since we pass a static variable by reference we
  // do not want the Reactor class to take ownership. Hence we leave
  // the second arg as zero => "do not own". The reactor should be
  // passed as a pointer.  But if you allocated the implementation of the
  // reactor dynamically, then you should clean it up.
  ACE_Reactor * reactor = new ACE_Reactor (&select_reactor);

  // if you so desire, you can update the underlying Singleton instance of the
  // ACE's reactor
#endif

	/* For this demonstration we will use the ACE Reactor available from the
		Singleton instance in ACE
	*/
	ACE_Reactor *reactor = ACE_Reactor::instance ();  // static class method (returns the same instance every time - singleton pattern)

	// instantiate our server object. Note that its constructor needs a
	// pointer to a reactor. Let us pass it the reactor we just
	// instantiated above.
	Dummy_Accept_Handler  accept_handler (reactor); 

	// the first step for a server is to initialize itself on the port
	// and host network interface.
	if (accept_handler.open (server_addr) == -1) {
		ACE_ERROR ((LM_ERROR,
					ACE_TEXT ("[%P] server - "),
					ACE_TEXT ("failed to initialize server (%m)\n")));
		return 1;
	}
	char myip[32];
	server_addr.addr_to_string(myip,32);
	cout << "Server running on " << myip << endl;

	// now simply delegate everything to the reactor and let it call us
	// back whenever there are events
	if (reactor->run_reactor_event_loop () == -1) {
		ACE_ERROR ((LM_ERROR,
					ACE_TEXT ("[%P] server - "),
					ACE_TEXT ("failed in reactor's event loop (%m)\n")));
		return 1;
	}

	// unless we use the reactor's notify mechanism (not demonstrated in
	// this sample), and if there are no errors that kick us out, we
	// shall never reach this point :-) 

	return 0;
}
