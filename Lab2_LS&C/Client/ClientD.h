#pragma once
#include <string>
#include <thread>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

class ClientD;

typedef void(*MessageReceivedHandler)(std::string msg);

class ClientD
{
public:

	ClientD();
	~ClientD();
	void connectSock();
	void sendMsg(std::string txt);
	void threadRecv();
	std::thread recvThread;
	std::string userName;
	bool joinedChat = true;


private:
	SOCKET createSocket();
	std::string ip = "127.0.0.1";
	uint16_t Port = 31337;
	sockaddr_in addSock;
	SOCKET sock;		//This is the socket we will connect to. 
	bool recvTrun;


};