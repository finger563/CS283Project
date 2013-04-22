#include "helper_funcs.h"

string ip_addr = "127.0.0.1:9999";		// IP address from command line
string username = "TestUser";

extern Player_c player;

// lock variable for mutual exclusion
ACE_Thread_Mutex  lock;

Dummy_Event_Handler event_handler;

// command line parsing
int parse_args (int argc, ACE_TCHAR *argv[]) {
	int c;
	Player_s info;
	ACE_Get_Opt get_opt (argc, argv, "i:n:");
	while ((c = get_opt ()) != -1) {
		switch (c) {
		case 'i':
			ip_addr = get_opt.opt_arg ();
			break;
		case 'n':
			username = get_opt.opt_arg();
			break;
		case '?':
		default:
			ACE_ERROR_RETURN ((LM_ERROR,
								"usage:  %s "
								"-i <IP address>:<port number> "
								"-n <Username> "
								"\n",
								argv [0]),
								-1);
		}
	}
	info.SetName(username.c_str());
	player.Info(info);
	return 0; // Have successfully parsed command line args
}

            
// define the thread function. This is the main entry point into a
// thread so each thread can do its job independently of the others
void *thread_func (void *arg)
{
	ACE_TCHAR **myargv = (ACE_TCHAR **)arg;
	ACE_DEBUG ((LM_DEBUG,
				ACE_TEXT ("[%t] Player : Network Thread - has started\n")));

	// A singleton instance to the Reactor is obtained using the
	// ACE_Reactor::instance () method.
	//
  
	/* For this demonstration we will use the ACE Reactor available from the
		Singleton instance in ACE
	*/
	ACE_Reactor *reactor = ACE_Reactor::instance ();  // static class method (returns the same instance every time - singleton pattern)

	event_handler = Dummy_Event_Handler(reactor);
	
	cout << "Connecting to server on: " << ip_addr << endl;

	// the first step for a server is to initialize itself on the port
	// and host network interface.
	if (event_handler.open (myargv) == -1) {
		ACE_ERROR ((LM_ERROR,
					ACE_TEXT ("[%t] Player : Network Thread - "),
					ACE_TEXT ("failed to initialize client (%m)\n")));
		return 0;
	}
	
	// now simply delegate everything to the reactor and let it call us
	// back whenever there are events
	if (reactor->run_reactor_event_loop () == -1) {
		ACE_ERROR ((LM_ERROR,
					ACE_TEXT ("[%t] Player : Network Thread - "),
					ACE_TEXT ("failed in reactor's event loop (%m)\n")));
		return 0;
	}

	ACE_DEBUG ((LM_DEBUG,
				ACE_TEXT ("[%t] Player : Network Thread - done\n")));

	return 0;
}