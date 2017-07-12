#pragma once
#include <boost/asio.hpp>
using boost::asio::ip::tcp;

const int maxLength = 80;
class Client
{
private:
	tcp::socket* _socket;
	char* _readBuffer;
	char* _writeBuffer;
	std::string _clientName;
	tcp::endpoint* _endPoint;
	//Индикатор того, что был открыт поток на чтение
	//Он необходим для того, чтобы для одного соединения не было открыто два конфликтующих потока чтения
	bool _readIndicator;
	bool _firstMessage;
	boost::asio::deadline_timer* _timer;
	//_work создается для того, чтобы _io_service не останавливался при закрытии всех потоков, пока не было _io_service.stop()
	boost::asio::io_service::work *_work;
	
	void stop();
	//Метод, позволяющий пользователю выбрать: отправлять или принимать сообщение
	void userActions();
	//onSaind вызывается, если пользователь выбрал действие - отправить сообщение
	void onSaid();

	//Обработчик, вызываемый таймером спустя отведенное время
	void handleWait(const boost::system::error_code error);
	//Обработчик подключения к серверу
	void handleConnect(const boost::system::error_code error);
	//Отправка сообщения серверу
	void doWrite(std::string message);
	//Открытие потока ожидания сообщения
	void doRead();
	//Конечный обработчик, вызываемый при получении сообщения
	void readHandle(const boost::system::error_code& error, size_t bytes);
	//Обработчик отправки сообщения
	void writeHandle(const boost::system::error_code& error, size_t bytes);
	//Промежуточный обработчик прнимаемого сообщения
	size_t readingComplete(const boost::system::error_code& error, size_t bytes);
public:
	Client(std::string clientName);
	void Connect(char* server, char* port);
	~Client();
};

