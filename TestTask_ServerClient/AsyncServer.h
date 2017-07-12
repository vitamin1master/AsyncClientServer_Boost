#pragma once
#include <boost/asio.hpp>
#include <iostream>
#include "Connection.h"

class AsyncServer
{
private:
	bool _serverEnableIndicator;
	//����������� ������ ����������
	std::vector<boost::shared_ptr<Connection>>* _listConnections;
	boost::asio::ip::tcp::acceptor *_acceptor;
	boost::asio::io_service _io_service;
	//_work ��������� ��� ����, ����� _io_service �� �������������� ��� �������� ���� �������, ���� �� ���� _io_service.stop()
	boost::asio::io_service::work *_work;
	

	void stopServer();
	//���������� ����������� ������ �������
	void handleAccept(boost::shared_ptr<Connection> connectionPtr, const boost::system::error_code &error);

public:

	AsyncServer(const char* port);
	void StartServer();
	void StopServer();
	~AsyncServer();
};
