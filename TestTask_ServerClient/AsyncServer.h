#pragma once
#include <boost/asio.hpp>
#include <iostream>
#include "Connection.h"

class AsyncServer
{
private:
	bool _serverEnableIndicator;
	//Указательна список соединений
	std::vector<boost::shared_ptr<Connection>>* _listConnections;
	boost::asio::ip::tcp::acceptor *_acceptor;
	boost::asio::io_service _io_service;
	//_work создается для того, чтобы _io_service не останавливался при закрытии всех потоков, пока не было _io_service.stop()
	boost::asio::io_service::work *_work;
	

	void stopServer();
	//Обработчик подключения нового клиента
	void handleAccept(boost::shared_ptr<Connection> connectionPtr, const boost::system::error_code &error);

public:

	AsyncServer(const char* port);
	void StartServer();
	void StopServer();
	~AsyncServer();
};
