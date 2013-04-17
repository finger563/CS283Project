#include "helper_funcs.h"

extern ACE_INET_Addr server_addr;	
extern string ip_addr;

// lock variable for mutual exclusion
ACE_Thread_Mutex  lock;

// command line parsing
int parse_args (int argc, ACE_TCHAR *argv[]) {
	int c;
	ACE_Get_Opt get_opt (argc, argv, "i:");
	while ((c = get_opt ()) != -1) {
		switch (c) {
		case 'i':
			ip_addr = get_opt.opt_arg ();
			break;
		case '?':
		default:
			ACE_ERROR_RETURN ((LM_ERROR,
								"usage:  %s "
								"-i <IP address>:<port number> "
								"\n",
								argv [0]),
								-1);
		}
	}
	return 0; // Have successfully parsed command line args
}

            
// define the thread function. This is the main entry point into a
// thread so each thread can do its job independently of the others
void *thread_func (void *arg)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%t) has started\n")));

  int my_id = -1;  // I will assign myself an ID.

  {
    // notice the use of the scoped locking idiom where we create a guard
    // variable parametrized by the lock. The constructor acquires the lock. On
    // leaving this scope, the destructor will get invoked and the lock will be
    // released.  Notice how I have purposefully created a dummy scope { ... }
    // to bound the critical section.
    ACE_GUARD_RETURN (ACE_Thread_Mutex, guard, lock, 0);
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("(%t) got thread id %d\n"), my_id));
  }  // end of scope where the lock will be magically released.

  // once you know what your ID is, use it and add to 10,000 to use it
  // as your port number. Use that info to initialize the reactor and
  // register the event handler for "handle_input" for this thread
  // with the reactor. The handle_input has to do 2 things:
  // handle_input may be invoked if you are participating in the
  // routing table computation algorithm. The other case is when you
  // have to do forwarding of the client's packet.
  //
  // A singleton instance to the Reactor is obtained using the
  // ACE_Reactor::instance () method.
  //

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%t) I am done\n")));

  return 0;
}