#include "chat.h"

//bleh constructor
Chat::Chat(): place(conversation.rbegin()) 
{
}

void Chat::addMessage(std::string msg)
{
	conversation.push_back(msg);
	place = conversation.rbegin();
}

std::list<std::string> Chat::getList()
{
	return conversation;
}

std::list<std::string>::reverse_iterator Chat::getPlace()
{
	return place;
}

void Chat::Up()
{
	++place;
}

void Chat::Down()
{
	--place;
}

std::string Chat::getMsg()
{
	return *conversation.rbegin();
}