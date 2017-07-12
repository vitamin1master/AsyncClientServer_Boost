#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <stdio.h>
#include "Client.h"
#endif

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>

void main()
{
	std::string name;
	std::cout << "Please enter your name" << std::endl;
	std::cin.ignore(std::cin.rdbuf()->in_avail());
	std::getline(std::cin, name);
	Client client1(name);
	char* port = new char[10]();
	char* server = new char[10]();
	port = "8001";
	server = "127.0.0.1";
	client1.Connect(server, port);
	getchar();
}
