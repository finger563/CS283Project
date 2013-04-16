// $Id$
//
// Author: William Emfinger
// CS387 HW 1 header file
// Date Created : Feb 16, 2012
//
//
// Message header file.
// 
// We define the message format for Assignment 2
//  with member and helper functions.
//

// notice how we protect double inclusion of declarations by virtue of
// having these preprocessor symbols
#if !defined (_CS387_MESSAGE_H_)
#define _CS387_MESSAGE_H_

//#define DEBUG		// used for printing out messages received

#include <stdio.h>
#include <string.h>
// the following is used for logging of messages
#include "ace/Log_Msg.h"

#include <ace/CDR_Stream.h>

const int MAX_NAME_LEN=100;
const int MAX_CONT_LEN=256;

enum MessageType {
	REGISTER,
	ASSIGN,
	QUESTION,
	REPLY,
	SUBMIT,
	DISMISS,
	DEBUG_M
};

enum AssignmentType {
	HW, PA, TQ,DEBUG_A
};

struct Student_s {
	char			name[MAX_NAME_LEN];
	ACE_CDR::Long	id;
	Student_s*		next;
	
	Student_s() {next=NULL;id=0;memset(name,0,MAX_NAME_LEN);}
	Student_s(const Student_s &s){next=NULL;id=s.id;memset(name,0,MAX_NAME_LEN);strcpy(name,s.name);}
	Student_s(char* n,ACE_CDR::Long i){next=NULL;id=i;memset(name,0,MAX_NAME_LEN);strcpy(name,n);}
	
	Student_s & operator=(const Student_s &s) 
	{
		if (this != &s) // protect against invalid self-assignment
        {
			id = s.id;
			strcpy(name,s.name);
        }
        // by convention, always return *this
        return *this;
	}
	
	void Link(Student_s* s){next = s;}
	
	bool operator==(const Student_s &b)
	{
		if ( id ==b.id && strcmp(name,b.name)==0 )
			return true;
		else
			return false;
	}
};

struct Assignment_s {
	AssignmentType	type;
	ACE_CDR::Long	id;
	char			content_[MAX_CONT_LEN];
	Assignment_s*	next;
	
	Assignment_s() {next=NULL;type=HW;id=0;memset(content_,0,MAX_CONT_LEN);}
	Assignment_s(const Assignment_s &a){next=NULL;type=a.type;id=a.id;strcpy(content_,a.content_);}
	Assignment_s(AssignmentType t, ACE_CDR::Long i,char* c){next=NULL;type=t;id=i;strcpy(content_,c);}
	
	Assignment_s & operator=(const Assignment_s &a) 
	{
		if (this != &a) // protect against invalid self-assignment
        {
			type = a.type;
			id = a.id;
			strcpy(content_,a.content_);
        }
        // by convention, always return *this
        return *this;
	}

	void Link(Assignment_s* a){next = a;}

	bool operator==(const Assignment_s &b)
	{
		if ( id ==b.id && type == b.type )
			return true;
		else
			return false;
	}
};

class Message {
private:
	MessageType type_;
	Assignment_s	assignment_;
	Student_s		student_;
	char		content_[MAX_CONT_LEN];
public:
	Message() : assignment_(),student_()
	{
		type_ = REGISTER;
		memset(content_,0,MAX_CONT_LEN);
	}
	Message(const Message &m) : assignment_(m.Assignment()),student_(m.Student())
	{
		this->type_ = m.Type();
		memset(content_,0,MAX_CONT_LEN);
		strcpy(this->content_,m.Content());
	}
	Message(MessageType t,Assignment_s &a,Student_s &s, char* c) : assignment_(a),student_(s)
	{
		this->type_ = t;
		memset(content_,0,MAX_CONT_LEN);
		strcpy(this->content_,c);
	}
	~Message(){}
	Message & operator=(const Message & m)
	{
		if (this != &m) // protect against invalid self-assignment
        {
			type_ = m.Type();
			assignment_ = m.Assignment();
			student_ = m.Student();
			strcpy(content_,m.Content());
        }
        // by convention, always return *this
        return *this;
	}

	size_t Length() const;

	void		Type(const MessageType type) {type_=type;}
	MessageType Type() const {return type_;}

	void			Assignment(const Assignment_s& a) {assignment_=a;}
	Assignment_s	Assignment() const  {return assignment_;}

	void		Student(const Student_s& s) {student_=s;}
	Student_s	Student() const  {return student_;}

	const char*		Content() const {return content_;}
	void			Content(const char* c) {memset(content_,0,MAX_CONT_LEN);strcpy(content_,c);}

	bool FormMessage(bool isTeacher);
};


int operator<< (ACE_OutputCDR &cdr, const Message &message);
int operator>> (ACE_InputCDR &cdr, Message &message);

#endif /* _CS387_MESSAGE_H_ */