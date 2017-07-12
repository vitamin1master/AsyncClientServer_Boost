#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <boost/enable_shared_from_this.hpp>
using boost::asio::ip::tcp;
//Максимальная длина сообщения.
class Connection;

class Connection: public boost::enable_shared_from_this<Connection>
{
private:
	//Открытие потока ожидания нового сообщения.
	void doRead();
	//Отправка сообщения клиенту
	void doWrite(std::string message);
	//Промежуточный обработчик прнимаемого сообщения
	size_t readingComplete(const boost::system::error_code& error, size_t bytes);
	//Конечный обработчик, вызываемый при получении сообщения
	void readHandle(const boost::system::error_code& error, size_t bytes);
	//Обработчик отправки сообщения
	void writeHandle(const boost::system::error_code& error, size_t bytes);
	//Остановка соединения
	void stop();
	//Регистрация нового Клиента
	void onLogin(std::string message);
	//Полученное сообщение будет отправлено обратно
	//Функция не используется, но работает правильно
	void onEchoClientSaid(std::string message);
	//Другое сообщение
	void onClientSaid(std::string message);

	//Указатель на список соединений
	std::vector<boost::shared_ptr<Connection>>* _listConnections;
	boost::asio::io_service* _io_service;
	tcp::socket* _socket;
	std::string _clientName;
	//Буферы чтения и записи
	char* _readBuffer;
	char* _writeBuffer;
	//Индикатор того, что был открыт поток на чтение
	//Он необходим для того, чтобы для одного соединения не было открыто два конфликтующих потока чтения
	bool _readIndicator;
	//Индикатор работы соединения
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