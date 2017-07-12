#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <stdio.h>
#endif

#include "AsyncServer.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
using boost::asio::ip::tcp;

//private:
void AsyncServer::handleAccept(boost::shared_ptr<Connection> connection_ptr, const boost::system::error_code &error)
{
	std::cout << "new connection"<<std::endl;
	//handleAccept вызывается при новом подключении к серверу
	//Объект класса Connection реализует Клиента, имеющего имя, порт соединения
	//В самом Connection открываются потоки на ожидание сообщения от конкретного Клиента
	connection_ptr->StartConnection();
	//Создаем объект класса Connection, ожидающий нового Клиента
	boost::shared_ptr<Connection> newConnection_ptr(new Connection(
		_io_service,
		_listConnections
		));
	//Открываем поток ожидания нового соединения
	//После успешного подключения нового клиента будет запущен метод handleAccept
	_acceptor->async_accept(*newConnection_ptr->Socket(), boost::bind(&AsyncServer::handleAccept, this, newConnection_ptr, _1));
}
void AsyncServer::stopServer()
{
	std::vector<boost::shared_ptr<Connection>>::iterator iterator;
	for (iterator = _listConnections->begin(); iterator != _listConnections->end(); iterator++)
		_listConnections->_Make_iter(iterator)->get()->StopConnection();
	_io_service.stop();
	delete _acceptor;
	delete _listConnections;
	std::cout << "Stop server";
}

//public:
AsyncServer::AsyncServer(const char* port)
{
	_acceptor = new tcp::acceptor(_io_service, tcp::endpoint(tcp::v4(), atoi(port)));
	_listConnections = new std::vector<boost::shared_ptr<Connection>>();
	_serverEnableIndicator = false;
}
//
void AsyncServer::StartServer()
{
	if (_serverEnableIndicator)
		return;
	_serverEnableIndicator = true;
	std::cout << "Start server"<<std::endl;
	//Создаем shared pointer с экземпляром Connection, ожидающий подключения Клиента.
	boost::shared_ptr<Connection> newConnection_ptr(new Connection(
		_io_service,
		_listConnections));
	//При первом подключении к серверу будет вызвана функция handleAccept.
	_acceptor->async_accept(*newConnection_ptr->Socket(), boost::bind(&AsyncServer::handleAccept, this, newConnection_ptr, _1));
	//Открываем поток ожидания.
	_io_service.run();
	_work = new boost::asio::io_service::work(_io_service);
}
void AsyncServer::StopServer()
{
	if (!_serverEnableIndicator)
		return;
	_serverEnableIndicator = false;
	stopServer();
}

AsyncServer::~AsyncServer()
{
	stopServer();
	//delete _io_service;
}
