// $Id$
//
// Author: William Emfinger
// CS387 HW 1 header file
// Date Created : Feb 16, 2012
//
//	This file contains helper functions for networking
//	to keep the code cleaner in main
//

#if !defined (_CS283_HELPER_FUNCS_H)
#define _CS283_HELPER_FUNCS_H

#include <iostream>
#include <string>

// the following is used for logging of messages
#include "ace/Log_Msg.h"

#include "ace/OS_NS_string.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Stream.h"
#include "ace/Get_Opt.h"
#include "ace/OS_NS_stdio.h"
#include "ace/Thread_Manager.h"   // for the thread manager
#include "ace/Signal.h"

#include "ace/Select_Reactor.h"  // we use the Select_Reactor
#include "Dummy_Event_Handler.h"  // this is the event handler we need

using namespace std;

struct FuncArg_t
{
	int chunkID;
	int peerID;
};

int parse_args (int argc, ACE_TCHAR *argv[]);

// signature of our thread entry point
void *thread_func (void *arg);


extern Dummy_Event_Handler event_handler;

#endif /* _CS283_HELPER_FUNCS_H */