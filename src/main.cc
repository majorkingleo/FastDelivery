/*
 * main.cc
 *
 *  Created on: 01.07.2023
 *      Author: martin
 */
#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include "FastDelivery.h"

using namespace std::chrono_literals;
using namespace FastDelivery;


class LogNode : public PublisherNode<std::string>
{
public:
	void deliver( const std::string & msg )
	{
		std::cout << "msg: " << msg << std::endl;
	}
};

class LockedLogNode : public PublisherNode<std::string>
{
	std::list<std::string> messages;
	std::mutex m_messages;
	std::string name;

public:
	LockedLogNode( const std::string & name_ )
	: name( name_ )
	{}

	void deliver( const std::string & msg )
	{
		std::lock_guard<std::mutex> ml(m_messages);
		messages.push_back( msg );
	}

	void log()
	{
		std::lock_guard<std::mutex> ml(m_messages);
		for( const std::string & m : messages ) {
			std::cout << name << ": " << m << std::endl;
		}
		messages.clear();
	}
};

void log_thread( Publisher<std::string,LockedLogNode> & pub, const std::string & name )
{
	LockedLogNode node(name);
	pub.subscribe(&node);

	for( unsigned i = 0; i < 100; i++ ) {
		node.log();
		std::this_thread::sleep_for(200ms);
	}
}

void create_threads()
{
	for( unsigned i = 0; i < 1; i++ ) {

	}
}

int main()
{
	Publisher<std::string,LockedLogNode> pub;

	std::thread( log_thread, std::ref(pub), "t1").detach();
	std::thread( log_thread, std::ref(pub), "t2").detach();
	std::thread( log_thread, std::ref(pub), "t3").detach();


	std::this_thread::sleep_for(200ms);
	pub.distribute("hello1");
	pub.distribute("hello2");
	pub.distribute("hello3");
	pub.distribute("hello4");

	std::this_thread::sleep_for(2000ms);

}

