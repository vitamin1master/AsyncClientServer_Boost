#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <stdio.h>
#endif

#include "Client.h"
#include <boost/bind.hpp>
#include <iostream>
#include <sstream>
#include <thread>
using boost::asio::ip::tcp;

#define binding1(x) boost::bind(&Client::x,this)
#define binding2(x,y) boost::bind(&Client::x,this,y)
#define binding3(x,y,z) boost::bind(&Client::x,this,y,z)

boost::asio::io_service ioService;
boost::asio::io_service timerIoService;
const int maxSize = 80;
//private:


void Client::userActions()
{
	std::cout << "<<<Y or N>>>: ";
	char simbol;
	fflush(stdin);
	scanf_s("%c", &simbol);
	if (simbol == 'Y')
		onSaid();
	else 
	{
		if (simbol == 'N')
		{
			//��������� ������ ��������. ���� ������ ����� delay �� ������ �� ������ ���������,
			//����� ������ ����� userActions �� handleWait
			int delay = 5000;
			_timer->expires_from_now(boost::posix_time::milliseconds(delay));
			doRead();
			_timer->async_wait(binding2(handleWait,_1));
		}
		else
		{
			std::cout << "Invalid input" << std::endl;
			userActions();
		}
	}
}
void Client::onSaid()
{
	std::string userMessage;
	std::string line;
	std::cout << "You: ";
	std::cin.ignore(std::cin.rdbuf()->in_avail());
	std::getline(std::cin, userMessage);
	doWrite(userMessage);
	userActions();
}

void Client::stop()
{
	_socket->close();
	std::cout << "Server lost connection" << std::endl;
}

void Client::handleWait(const boost::system::error_code error)
{
	if (error)
		return;
	//���� ������ �����������, ������, userActions ��� ��� ������ �� readHandle
	if (_readIndicator)
		userActions();
}
void Client::handleConnect(const boost::system::error_code error)
{
	if (error)
	{
		stop();
		return;
	}
	std::string message("connect ");
	message += _clientName;
	std::cout << "Sent connection request" << std::endl;
	doWrite(message);
	std::cout << "***Please note***" << std::endl;
	std::cout << "***If you see <<Y or N>>, enter Y if you want to send a message, or N if you want to receive a message***" << std::endl;
	std::cout << "***If you entered N and if the message does not arrive after 5 seconds, you can choose another action***" << std::endl;
}
void Client::writeHandle(const boost::system::error_code& error, size_t bytes)
{
	if (error)
	{
		stop();
		return;
	}
	//��� ������ ��������� ���� ����������, ����������� ����� ������
	//�� ������ ��� ������� ���������. ����������� ������ ������ ����������� �� ������� ������������
	if (_firstMessage)
	{
		_firstMessage = false;
		doRead();
	}
}
void Client::readHandle(const boost::system::error_code& error, size_t bytes)
{
	if (error)
	{
		stop();
		return;
	}
	//����� �������� ��������� ��������� ����� ������ �����������
	_readIndicator = false;
	std::string message(_readBuffer, bytes);
	message.erase(message.find("/n"), 2);
	std::cout << message << std::endl;
	//doRead();
	userActions();
}

size_t Client::readingComplete(const boost::system::error_code& error, size_t bytes)
{
	if (error)
		return 0;
	if (bytes)
	{
		//��������� ���������, ���� � ������� ������ �� �������� ��������� "/n"
		int i;
		for (i = 0; i < (int)bytes - 1 && (_readBuffer[i] != '/' || _readBuffer[i + 1] != 'n'); i++);
		//���� ������� ��������� "/n", ���������� 0
		if (i >= (int)bytes - 1)
			return 1;
		return 0;
	}
	return 1;
}


void Client::doWrite(std::string message)
{
	message += "/n";
	_writeBuffer = &message[0];
	//�������� ��������� _writeBuffer ������������� �������
	//��� ������� �������� ��������� ����� ������ ����� writeHandle
	_socket->async_write_some(boost::asio::buffer(_writeBuffer, message.size()), binding3(writeHandle, _1, _2));
}
void Client::doRead()
{
	if (_readIndicator)
		return;
	_readIndicator = true;
	_readBuffer = new char[maxSize];
	//��� ��������� ��������� �������� ����� ����������� �������� ��������� ������� readComplete
	//���� readComplete ������ 0, ��������� ��������� �����������, � ����� ������ ����� readHandle
	boost::asio::async_read(*_socket, boost::asio::buffer(_readBuffer,maxSize),
		binding3(readingComplete, _1, _2), binding3(readHandle, _1, _2));
}
//public:
Client::Client(std::string clientName)
{
	_socket = new tcp::socket(ioService);
	_clientName = clientName;
	_readIndicator = false;
	_firstMessage = true;
	_timer = new boost::asio::deadline_timer(ioService);
}
void Client::Connect(char* server, char* port)
{
	//������������ �� ��������� ������ ������� � �����
	_firstMessage = true;
	_endPoint = new tcp::endpoint(boost::asio::ip::address::from_string(server), atoi(port));
	_socket->async_connect(*_endPoint, binding2(handleConnect, _1));
	ioService.run();
	_work = new boost::asio::io_service::work(ioService);
}


Client::~Client()
{
}
