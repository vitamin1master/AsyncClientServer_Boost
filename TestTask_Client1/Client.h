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
	//��������� ����, ��� ��� ������ ����� �� ������
	//�� ��������� ��� ����, ����� ��� ������ ���������� �� ���� ������� ��� ������������� ������ ������
	bool _readIndicator;
	bool _firstMessage;
	boost::asio::deadline_timer* _timer;
	//_work ��������� ��� ����, ����� _io_service �� �������������� ��� �������� ���� �������, ���� �� ���� _io_service.stop()
	boost::asio::io_service::work *_work;
	
	void stop();
	//�����, ����������� ������������ �������: ���������� ��� ��������� ���������
	void userActions();
	//onSaind ����������, ���� ������������ ������ �������� - ��������� ���������
	void onSaid();

	//����������, ���������� �������� ������ ���������� �����
	void handleWait(const boost::system::error_code error);
	//���������� ����������� � �������
	void handleConnect(const boost::system::error_code error);
	//�������� ��������� �������
	void doWrite(std::string message);
	//�������� ������ �������� ���������
	void doRead();
	//�������� ����������, ���������� ��� ��������� ���������
	void readHandle(const boost::system::error_code& error, size_t bytes);
	//���������� �������� ���������
	void writeHandle(const boost::system::error_code& error, size_t bytes);
	//������������� ���������� ����������� ���������
	size_t readingComplete(const boost::system::error_code& error, size_t bytes);
public:
	Client(std::string clientName);
	void Connect(char* server, char* port);
	~Client();
};

