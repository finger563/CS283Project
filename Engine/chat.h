#ifndef Chat_H
#define Chat_H

#include <list>
#include <string>
#include <iterator>

class Chat_c{

private:
	std::list<std::string> conversation;
	std::list<std::string>::reverse_iterator place;

	int numChatsDisplayed;
 
public:

	Chat_c();
	Chat_c(const int chats);

	void NumChatsDisplayed( const int chats ) { numChatsDisplayed = chats; }
	int NumChatsDisplayed( ) const { return numChatsDisplayed; }

	void Conversation(const std::list<std::string> c) { conversation = c; }
	std::list<std::string> Conversation() const { return conversation; }

	void Place(const std::list<std::string>::reverse_iterator p) { place = p; }
	std::list<std::string>::reverse_iterator Place() const { return place; }

	void addMessage(std::string msg);

	std::string getMsg();

	void Up();

	void Down();

	Chat_c& operator= (const Chat_c& rhs);

};
#endif