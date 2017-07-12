#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <stdio.h>
#endif

#include <boost/asio.hpp>
#include <iostream>
#include "AsyncServer.h"

boost::asio::io_service io_service;

void main()
{
	char* port = new char[10]();
	port = "8001";
	AsyncServer server(port);
	server.StartServer();
	getchar();
}