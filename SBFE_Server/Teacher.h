// $Id$
//
// Author: William Emfinger
// CS387 HW 1 header file
// Date Created : Feb 16, 2012
//
//
// Teacher header file.
//

// notice how we protect double inclusion of declarations by virtue of
// having these preprocessor symbols
#if !defined (_CS387_TEACHER_H_)
#define _CS387_TEACHER_H_

#include "Message.h"

#include <iostream>
using namespace std;

#include <ace/SOCK_Stream.h>      // for data comm functionality

class Teacher_c {
private:
	Assignment_s	*assignments;
	Student_s		*students;
	Assignment_s	*questions;
public:
	Teacher_c() {assignments=NULL;students=NULL;questions=NULL;}
	Teacher_c(Teacher_c& t) {*this=t;}
	~Teacher_c() {delete assignments,students,this;}

	Teacher_c & operator=(Teacher_c& t)
	{
		if (this != &t) // protect against invalid self-assignment
        {
			assignments = t.Assignments();
			students = t.Students();
			questions = t.Questions();
        }
        // by convention, always return *this
        return *this;
	}

	Assignment_s * Assignments() {return assignments;}
	Student_s *	Students() {return students;}
	Assignment_s * Questions() {return questions;}

	void Questions(Assignment_s& q)
	{
		char str[3];
		switch (q.type)
		{
		case HW:
			sprintf(str,"HW");
			break;
		case TQ:
			sprintf(str,"TQ");
			break;
		case PA:
			sprintf(str,"PA");
			break;
		default:
			break;
		}
		ACE_DEBUG ((LM_DEBUG,
				ACE_TEXT ("Question about Assignment: (%s,%d) is: %s\n"),
				str,
				q.id,
				q.content_));
		Assignment_s* tmp;
		if (questions==NULL)
		{
			questions = new Assignment_s(q);
		}
		else
		{
			for (tmp=questions;tmp->next!=NULL;tmp=tmp->next);
			Assignment_s* link = new Assignment_s(q);
			tmp->Link(link);
		}
	}

	bool Student(Student_s& s)
	{
		if (students != NULL)
		{
			if ( s == *students )
				return true;
			Student_s* tmp=Students();
			while (tmp!=NULL)
			{
				if ( s == *tmp ) 
					return true;
				tmp=tmp->next;
			}
		}
		ACE_ERROR ((LM_ERROR,
			ACE_TEXT ("Error, student has not registered!\n")));
		return false;
	}

	char* Student(ACE_CDR::Long id)
	{
		if (students != NULL)
		{
			if ( id == students->id )
				return students->name;
			Student_s* tmp=Students();
			while (tmp!=NULL)
			{
				if ( id == tmp->id ) 
					return tmp->name;
				tmp=tmp->next;
			}
		}
		ACE_ERROR ((LM_ERROR,
			ACE_TEXT ("Error, student has not registered!\n")));
		return NULL;
	}

	void RemoveStudent(ACE_CDR::Long id)
	{
		if ( students!=NULL)
		{
			Student_s* tmp = students;
			if ( tmp->id == id ) 
			{
				students = tmp->next;
				delete tmp;
				return;
			}
			else if (students->next != NULL)
			{
				tmp = tmp->next;
				if ( tmp->id == id  ) 
				{
					students->next = tmp->next;
					delete tmp;
					return;
				}
				Student_s* prev = students;
				for (;tmp->next!=NULL;tmp=tmp->next)
				{
					if ( tmp->id == id  ) 
					{
						prev->next = tmp->next;
						delete tmp;
						return;
					}
					prev = prev->next;
				}
			}
		}
	}
	
	bool Register(Student_s& s)
	{
		if ( students==NULL)
			students=new Student_s(s);
		else
		{
			if ( s == *students ) 
			{
				ACE_ERROR ((LM_ERROR,
					ACE_TEXT ("Error, %s has already registered!\n"),
					s.name));
				return false;
			}
			Student_s* tmp;
			for (tmp=students;tmp->next!=NULL;tmp=tmp->next)
			{
				if ( s == *tmp ) 
				{
					ACE_ERROR ((LM_ERROR,
						ACE_TEXT ("Error, %s has already registered!\n"),
						s.name));
					return false;
				}
			}
			Student_s* link = new Student_s(s);
			tmp->Link(link);
		}
		ACE_DEBUG ((LM_DEBUG,
				ACE_TEXT ("%s is now registered.\n"),
				s.name));
		return true;
	}

	void Dismiss() {delete students,assignments,questions;students=NULL;assignments=NULL;questions=NULL;}

	bool Assign(Assignment_s& a)
	{
		if ( assignments==NULL)
		{
			assignments=new Assignment_s(a);
			return true;
		}
		else
		{
			if ( a == *assignments ) 
			{
				ACE_ERROR ((LM_ERROR,
					ACE_TEXT ("Error, you have already assigned this!\n")));
				return false;
			}
			Assignment_s* tmp;
			for (tmp=assignments;tmp->next!=NULL;tmp=tmp->next)
			{
				if ( a == *tmp ) 
				{
					ACE_ERROR ((LM_ERROR,
						ACE_TEXT ("Error, you have already assigned this!\n")));
					return false;
				}
			}
			Assignment_s* link = new Assignment_s(a);
			tmp->Link(link);
			return true;
		}
	}

	bool Reply(Assignment_s& a)
	{
		if (questions==NULL)
		{
			ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("Error, no questions have been asked!\n")));
			return false;
		}
		else
		{
			Assignment_s* tmp;
			if ( a == *questions) return true;
			for (tmp=questions;tmp->next!=NULL;tmp=tmp->next)
			{
				if ( a == *tmp ) return true;
			}
			ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("Error, a question has not been asked about this!\n")));
			return false;
		}
	}
};

struct peer_s {
	ACE_SOCK_Stream *p;
	peer_s *next, *prev;
	ACE_CDR::Long ID;

	peer_s() {p=NULL;next=prev=NULL;ID=0;}
	peer_s(ACE_SOCK_Stream* newpeer,ACE_CDR::Long id) {next=prev=NULL;p=newpeer;ID=id;}

	void Push(peer_s *newpeer)
	{
		peer_s *tmp=this;
		while (tmp->next!=NULL)
			tmp=tmp->next;
		tmp->next = newpeer;
		newpeer->prev=tmp;
	}
	
	void Remove(ACE_CDR::Long id)
	{
		peer_s *tmp=this;

		while (tmp!=NULL)
		{
			if (  tmp->ID==id )
				break;
			tmp=tmp->next;
		}
		if (tmp==NULL)
			return;	// that ID doesn't exist in here
		tmp->prev->next = tmp->next;
		if ( tmp->next != NULL ) tmp->next->prev = tmp->prev;
		delete tmp;
	}

	void RemoveALL()
	{
		delete next;
		next = NULL;
	}
};

#endif /* _CS387_TEACHER_H_ */