#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <stdio.h>
#endif

#include "Connection.h"
#include "AsyncServer.h"
#include <chrono>
#include <thread>

#define binding1(x) boost::bind(&Connection::x,shared_from_this())
#define binding2(x,y) boost::bind(&Connection::x,shared_from_this(),y)
#define binding3(x,y,z) boost::bind(&Connection::x,shared_from_this(),y,z)

const int maxSize=80;
//private:
void Connection::stop()
{
	_socket->close();
	//Удаляем shared_from_this (объект shared_ptr<Connection>, в котором находимся в данный момент)
	//из списка всех соединений.
	std::vector<boost::shared_ptr<Connection>>::iterator iterator = 
		std::find(_listConnections->begin(), _listConnections->end(), shared_from_this());
	if (iterator!=_listConnections->end())
		_listConnections->erase(iterator);
	_startedIndicator = false;
	std::cout << _clientName << " " << "lost connection" << std::endl;
}

void Connection::doWrite(std::string message)
{
	if (!_startedIndicator)
		return;
	message += "/n";
	_writeBuffer = &message[0];
	//Посылаем сообщение _writeBuffer подключенному клиенту
	//При удачной отправке сообщения будет вызван метод writeHandle
	_socket->async_write_some(boost::asio::buffer(_writeBuffer, message.size()), binding3(writeHandle, _1, _2));
}


void Connection::doRead()
{
	if (!_startedIndicator)
		return;
	//Выходим, если поток чтения уже был запущен
	if (_readIndicator)
		return;
	_readIndicator = true;
	if (_clientName == "")
		std::cout << "Server waiting for new messages by Anonymous" << std::endl;
	else
		std::cout << "Server waiting for new messages by " << _clientName << std::endl;
	_readBuffer = new char[maxSize];
	//При получении сообщения сервером будет произведена проверка сообщения методом readComplete
	//Если readComplete вернет 0, сообщение считается законченным, и будет вызван метод readHandle
	boost::asio::async_read(*_socket, boost::asio::buffer(_readBuffer,maxSize), 
		binding3(readingComplete, _1, _2), 
			binding3(readHandle, _1, _2));
}

void Connection::writeHandle(const boost::system::error_code& error, size_t bytes)
{
	//Как только сообщение было отправлено, открывается поток чтения
	doRead();
}


void Connection::readHandle(const boost::system::error_code& error, size_t bytes)
{
	if (!_startedIndicator)
		return;
	if (error)
		stop();
	//После удачного получения сообщения поток чтения закрывается
	_readIndicator = false;
	if (_clientName == "")
		std::cout << "Server received a new message by Anonymous" << std::endl;
	else
		std::cout << "Server received a new message by " << _clientName << std::endl;
	std::string message(_readBuffer, bytes);
	message.erase(message.find("/n"), 2);
	if (message.find("connect")==0)
		onLogin(message);
	else onClientSaid(message);
}

size_t Connection::readingComplete(const boost::system::error_code& error, size_t bytes)
{
	if (!_startedIndicator)
		return 1;
	if (error)
		stop();
	if (bytes)
	{
		//Принимаем сообщения, пока в буффере чтения не появится подстрока "/n"
		int i;
		for (i = 0; i < (int)bytes-1 && (_readBuffer[i] != '/'||_readBuffer[i+1]!='n'); i++);
		//Если нашлась подстрока "/n", возвращаем 0
		if (i>=(int)bytes-1)
			return 1;
		return 0;
	}
	return 1;
}

void Connection::onLogin(std::string message)
{
	if (_clientName != "")
		return;
	std::string connectWord = "connect";
	int k = connectWord.length();
	std::string restMessage = message.substr(k + 1, message.length() - k - 1);
	//Проверка на наличие в имени пробельных символов.
	int i;
	for (i = 0; i < restMessage.length(); i++)
	{
		if (restMessage[i] == ' ')
			break;
	}
	if (i < restMessage.length())
		return;
	_clientName = restMessage;
	std::cout << _clientName << " connected" << std::endl;
	doWrite(_clientName + " connected");
}

void Connection::onEchoClientSaid(std::string message)
{
	if (_clientName == "")
		doWrite("You may have entered an incorrect name. Send a message: <<connect &&& >>, where &&& is your new name");
	if (message == "")
		return;
	doWrite(_clientName + " said: " + message);
}
void Connection::onClientSaid(std::string message)
{
	if (_clientName == "")
		doWrite("You may have entered an incorrect name. Send a message: <<connect &&& >>, where &&& is your new name");
	if (message == "")
		return;
	std::cout << "Message by " << _clientName << " was sent to:";
	if (_listConnections->size() <= 1)
	{
		std::cout << _clientName << std::endl;
		doWrite("Only you are connected to the server. You said: " + message);
	}
	else
	{
		//Если соединений больше одного, отправляем всем клиентам полученное сообщение, кроме отправителя
		std::vector<boost::shared_ptr<Connection>>::iterator findIterator =
			std::find(_listConnections->begin(), _listConnections->end(), shared_from_this());
		std::vector<boost::shared_ptr<Connection>>::iterator iterator;
		for (iterator = _listConnections->begin(); iterator != _listConnections->end(); iterator++)
		{
			if (iterator != findIterator)
			{
				_listConnections->_Make_iter(iterator)->get()->doWrite(_clientName + ":" + message);
				std::cout << " " << _listConnections->_Make_iter(iterator)->get()->GetClientName();
			}
		}
		std::cout << std::endl;
	}
}

//public:
boost::asio::ip::tcp::socket* Connection::Socket()
{
	return _socket;
}
std::string Connection::GetClientName()
{
	return _clientName;
}


void Connection::StartConnection()
{
	//Т.к. this - экземпляр нового соединения, добавляем его в список соединений
	_listConnections->push_back(shared_from_this());
	_startedIndicator = true;
	//Открываем поток чтения
	doRead();
	//_io_service->run();
}

void Connection::StopConnection()
{
	if (_startedIndicator)
		stop();
}

Connection::Connection(
	boost::asio::io_service& io_service, 
		std::vector<boost::shared_ptr<Connection>>* listConnections
		)
{
	_socket = new tcp::socket(io_service);
	_listConnections = listConnections;
	_clientName = "";
	_readIndicator = false;
	_startedIndicator = false;
}


Connection::~Connection()
{
	stop();
	delete _io_service;
}
