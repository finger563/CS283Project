// $Id$
//
// Author: William Emfinger
// CS387 HW 1 header file
// Date Created : Feb 16, 2012
//
//
// Player header file.
//

// notice how we protect double inclusion of declarations by virtue of
// having these preprocessor symbols
#if !defined (_CS283_PLAYER_H_)
#define _CS283_PLAYER_H_

#include "..\..\..\SBFE_Server\Message.h"

class Player_c {
private:
	bool registered;
	Player_s	info;
	Object_s*	objects;
public:
	Player_c() : info() {objects=NULL;registered=false;}
	Player_c(const Player_s& s): info(s) {objects=NULL;registered=false;}
	Player_c(Player_c& s){*this=s;}
	~Player_c() {delete objects, this;}

	Player_c & operator=(Player_c& s)
	{
		if (this != &s) // protect against invalid self-assignment
        {
			info = s.Info();
			objects = s.Objects();
        }
        // by convention, always return *this
        return *this;
	}

	Player_s Info() const {return info;}
	void Info(const Player_s& s) {info=s;}

	Object_s* Objects() {return objects;}

	void Register() {registered=true;}
	void Leave()	{registered=false;}

	bool Registered() {return registered;}

	void Create(Object_s& a) {
		char str[3];
		switch (a.type) {
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
				ACE_TEXT ("Receiving Object: (%s,%d) with content: %s\n"),
				str,
				a.id,
				a.content_));
		if (objects==NULL) {
			objects = new Object_s(a);
		}
		else {
			Object_s* tmp;
			for (tmp=objects;tmp->next!=NULL;tmp=tmp->next);
			Object_s* link = new Object_s(a);
			tmp->Link(link);
		}
	}

	void Remove(const ACE_CDR::Long _id) {
	}
	
	bool Question(Object_s& a) {
		if ( objects!=NULL) {
			if ( a == *objects )
				return true;
			Object_s* tmp;
			for (tmp=objects;tmp->next!=NULL;tmp=tmp->next) {
				if ( a == *tmp )
					return true;
			}
		}
		ACE_ERROR ((LM_ERROR,
			ACE_TEXT ("Error, this has not been assigned!\n")));
		return false;
	}
};

#endif /* _CS283_PLAYER_H_s */