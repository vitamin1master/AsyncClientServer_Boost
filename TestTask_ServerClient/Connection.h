#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <boost/enable_shared_from_this.hpp>
using boost::asio::ip::tcp;
//������������ ����� ���������.
class Connection;

class Connection: public boost::enable_shared_from_this<Connection>
{
private:
	//�������� ������ �������� ������ ���������.
	void doRead();
	//�������� ��������� �������
	void doWrite(std::string message);
	//������������� ���������� ����������� ���������
	size_t readingComplete(const boost::system::error_code& error, size_t bytes);
	//�������� ����������, ���������� ��� ��������� ���������
	void readHandle(const boost::system::error_code& error, size_t bytes);
	//���������� �������� ���������
	void writeHandle(const boost::system::error_code& error, size_t bytes);
	//��������� ����������
	void stop();
	//����������� ������ �������
	void onLogin(std::string message);
	//���������� ��������� ����� ���������� �������
	//������� �� ������������, �� �������� ���������
	void onEchoClientSaid(std::string message);
	//������ ���������
	void onClientSaid(std::string message);

	//��������� �� ������ ����������
	std::vector<boost::shared_ptr<Connection>>* _listConnections;
	boost::asio::io_service* _io_service;
	tcp::socket* _socket;
	std::string _clientName;
	//������ ������ � ������
	char* _readBuffer;
	char* _writeBuffer;
	//��������� ����, ��� ��� ������ ����� �� ������
	//�� ��������� ��� ����, ����� ��� ������ ���������� �� ���� ������� ��� ������������� ������ ������
	bool _readIndicator;
	//��������� ������ ����������
	bool _startedIndicator;
public:
	tcp::socket* Socket();
	std::string GetClientName();
	void StartConnection();
	Connection(
		boost::asio::io_service& io_service,
		std::vector<boost::shared_ptr<Connection>>* listConnections
			);
	void StopConnection();
	~Connection();
};