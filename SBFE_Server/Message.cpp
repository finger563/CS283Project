// $Id$
//
// Author: William Emfinger
// Date  : Created: Feb 21, 2012
//
//
// Message implementation file.
// 

#include "Message.h"

#include <iostream>
using namespace std;

size_t Message::Length() const
{
	size_t ret = 0;
	switch (type_)
	{
	case REGISTER:
		ret += 3 * sizeof(ACE_CDR::Long);
			// 1 -> Message type
			// 1 -> Student name length
			// 1 -> Student ID
		ret += strlen(student_.name);
		break;
	case ASSIGN:
	case QUESTION:
	case REPLY:
	case SUBMIT:
		ret += 6 * sizeof(ACE_CDR::Long);
			// 1 -> Message type
			// 1 -> Student name length
			// 1 -> Student ID
			// 1 -> Assignment type
			// 1 -> Assignment ID
			// 1 -> Message Content length
		ret += strlen(student_.name);
		ret += strlen(content_);
		break;
	case DISMISS:
		ret += 1 * sizeof(ACE_CDR::Long);
			// 1 -> Message type
		break;
	}
	return ret;
}

int operator<< (ACE_OutputCDR &cdr, const Message &m)
{
	cdr << ACE_CDR::Long (m.Type());
	switch (m.Type())
	{
	case REGISTER:
		cdr << ACE_CDR::Long (strlen(m.Student().name));
		cdr.write_char_array(m.Student().name,strlen(m.Student().name));
		cdr << ACE_CDR::Long (m.Student().id);
		break;
	case ASSIGN:
	case QUESTION:
	case REPLY:
		cdr << ACE_CDR::Long (strlen(m.Student().name));
		cdr.write_char_array(m.Student().name,strlen(m.Student().name));
		cdr << ACE_CDR::Long (m.Student().id);
		cdr << ACE_CDR::Long (m.Assignment().type);
		cdr << ACE_CDR::Long (m.Assignment().id);
		cdr << ACE_CDR::Long (strlen(m.Content()));
		cdr.write_char_array(m.Content(),strlen(m.Content()));
		break;
	case SUBMIT:
		cdr << ACE_CDR::Long (strlen(m.Student().name));
		cdr.write_char_array(m.Student().name,strlen(m.Student().name));
		cdr << ACE_CDR::Long (m.Student().id);
		cdr << ACE_CDR::Long (m.Assignment().type);
		cdr << ACE_CDR::Long (m.Assignment().id);
		cdr << ACE_CDR::Long (strlen(m.Content()));
		//cdr.write_char_array(m.Content(),strlen(m.Content()));
		cdr.write_octet_array((ACE_CDR::Octet *)m.Content(),strlen(m.Content()));
		break;
	case DISMISS:
		break;
	}

	return cdr.good_bit();
}

int operator>> (ACE_InputCDR &cdr, Message &message)
{
	ACE_CDR::Long type;
	ACE_CDR::Long assign_type;
	ACE_CDR::Long assign_id;
	ACE_CDR::Long student_id;
	ACE_CDR::Long name_len;
	ACE_CDR::Long cont_len;
	char name[MAX_NAME_LEN];
	char cont[MAX_CONT_LEN];

	Student_s student;
	Assignment_s assignment;

	cdr >> type;
	message.Type((MessageType)type);

	switch (type)
	{
	case REGISTER:
		cdr >> name_len;
		cdr.read_char_array(name,name_len);
		name[name_len]='\0';
		cdr >> student_id;
		student = Student_s(name,(int)student_id);
		message.Student(student);
		break;
	case ASSIGN:
	case QUESTION:
	case REPLY:
		cdr >> name_len;
		cdr.read_char_array(name,name_len);
		name[name_len]='\0';
		cdr >> student_id;
		cdr >> assign_type;
		cdr >> assign_id;
		cdr >> cont_len;
		cdr.read_char_array(cont,cont_len);
		cont[cont_len]='\0';
		student = Student_s(name,(int)student_id);
		message.Student(student);
		assignment = Assignment_s((AssignmentType)assign_type,(int)assign_id,cont);
		message.Assignment(assignment);
		message.Content(cont);
		break;
	case SUBMIT:
		cdr >> name_len;
		cdr.read_char_array(name,name_len);
		name[name_len]='\0';
		cdr >> student_id;
		cdr >> assign_type;
		cdr >> assign_id;
		cdr >> cont_len;
		cdr.read_octet_array((ACE_CDR::Octet *)cont,cont_len);
		cont[cont_len]='\0';
		student = Student_s(name,(int)student_id);
		message.Student(student);
		assignment = Assignment_s((AssignmentType)assign_type,(int)assign_id,cont);
		message.Assignment(assignment);
		message.Content(cont);
		break;
	case DISMISS:
		break;
	}

	return cdr.good_bit();;
}

bool Message::FormMessage(bool isTeacher)
{
	if (isTeacher)
	{
		cout << "Enter message type (ASSIGN,REPLY,DISMISS) or EXIT to receive: ";
		char str[MAX_CONT_LEN];
		cin.getline(str,MAX_CONT_LEN);
		if (stricmp(str,"ASSIGN")==0)
		{
			Type(ASSIGN);
		}
		else if (stricmp(str,"REPLY")==0)
		{
			Type(REPLY);
		}
		else if (stricmp(str,"DISMISS")==0)
		{
			Type(DISMISS);
		}
		else if (stricmp(str,"EXIT")==0)
		{
			return false;
		}
		else
		{
			cout << "I'm sorry, that is not a valid message\n";
			return false;
		}
		switch (type_)
		{
		case ASSIGN:
			cout << "Enter Assignment type (HW,PA,TQ): ";
			cin.getline(str,MAX_CONT_LEN);
			if (stricmp(str,"HW")==0)
			{
				assignment_.type=HW;
			}
			else if (stricmp(str,"TQ")==0)
			{
				assignment_.type=TQ;
			}
			else if (stricmp(str,"PA")==0)
			{
				assignment_.type=PA;
			}
			else
			{
				cout << "I'm sorry, that is not a valid type\n";
				return 0;
			}
			cout << "Enter Assignment number: ";
			cin.getline(str,MAX_CONT_LEN);
			assignment_.id = atoi(str);
			cout << "Enter Assignment content: ";
			cin.getline(str,MAX_CONT_LEN);
			strcpy(content_,str);
			break;
		case REPLY:
			cout << "Enter Reply to Assignment type (HW,PA,TQ): ";
			cin.getline(str,MAX_CONT_LEN);
			if (stricmp(str,"HW")==0)
			{
				assignment_.type=HW;
			}
			else if (stricmp(str,"TQ")==0)
			{
				assignment_.type=TQ;
			}
			else if (stricmp(str,"PA")==0)
			{
				assignment_.type=PA;
			}
			else
			{
				cout << "I'm sorry, that is not a valid type\n";
				return 0;
			}
			cout << "Enter Reply to Assignment number: ";
			cin.getline(str,MAX_CONT_LEN);
			assignment_.id = atoi(str);
			cout << "Enter Reply content: ";
			cin.getline(str,MAX_CONT_LEN);
			strcpy(content_,str);
			break;
		case DISMISS:
			break;
		default:
			return false;
			break;
		}
	}
	else
	{
		cout << "Enter message type (REGISTER,QUESTION,SUBMIT) or EXIT to receive: ";
		char str[MAX_CONT_LEN];
		cin.getline(str,MAX_CONT_LEN);
		if (stricmp(str,"REGISTER")==0)
		{
			Type(REGISTER);
		}
		else if (stricmp(str,"QUESTION")==0)
		{
			Type(QUESTION);
		}
		else if (stricmp(str,"SUBMIT")==0)
		{
			Type(SUBMIT);
		}
		else if (stricmp(str,"EXIT")==0)
		{
			return false;
		}
		else
		{
			cout << "I'm sorry, that is not a valid message\n";
			return false;
		}
		switch (type_)
		{
		case REGISTER:
			cout << "Enter your Full Name: ";
			cin.getline(str,MAX_CONT_LEN);
			strcpy(student_.name,str);
			cout << "Enter ID number: ";
			cin.getline(str,MAX_CONT_LEN);
			student_.id = atoi(str);
			break;
		case QUESTION:
			cout << "Enter Question to Assignment type (HW,PA,TQ): ";
			cin.getline(str,MAX_CONT_LEN);
			if (stricmp(str,"HW")==0)
			{
				assignment_.type=HW;
			}
			else if (stricmp(str,"TQ")==0)
			{
				assignment_.type=TQ;
			}
			else if (stricmp(str,"PA")==0)
			{
				assignment_.type=PA;
			}
			else
			{
				cout << "I'm sorry, that is not a valid type\n";
				return 0;
			}
			cout << "Enter Question to Assignment number: ";
			cin.getline(str,MAX_CONT_LEN);
			assignment_.id = atoi(str);
			cout << "Enter Question content: ";
			cin.getline(str,MAX_CONT_LEN);
			strcpy(content_,str);
			break;
		case SUBMIT:
			cout << "Enter Submission to Assignment type (HW,PA,TQ): ";
			cin.getline(str,MAX_CONT_LEN);
			if (stricmp(str,"HW")==0)
			{
				assignment_.type=HW;
			}
			else if (stricmp(str,"TQ")==0)
			{
				assignment_.type=TQ;
			}
			else if (stricmp(str,"PA")==0)
			{
				assignment_.type=PA;
			}
			else
			{
				cout << "I'm sorry, that is not a valid type\n";
				return 0;
			}
			cout << "Enter Submission to Assignment number: ";
			cin.getline(str,MAX_CONT_LEN);
			assignment_.id = atoi(str);
			cout << "Enter Submission content: ";
			cin.getline(str,MAX_CONT_LEN);
			strcpy(content_,str);
			break;
		default:
			return false;
			break;
		}
	}
	return true;
}