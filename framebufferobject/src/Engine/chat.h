#ifndef Chat_H
#define Chat_H

#include <list>
#include <string>
#include <iterator>

class Chat{

private:
	std::list<std::string> conversation;
	std::list<std::string>::reverse_iterator place;

 
public:

	//bleh constructor
	Chat();

	void addMessage(std::string msg);

	std::list<std::string> getList();

	std::list<std::string>::reverse_iterator getPlace();

	std::string getMsg();

	void Up();

	void Down();

};
#endif