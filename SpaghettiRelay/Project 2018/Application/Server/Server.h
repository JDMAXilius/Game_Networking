#pragma once
#include "../platform.h"
#include "../definitions.h"

class Server
{
	SOCKET listenSocket;
	sockaddr_in serverAddr;
	SOCKADDR sockAdd;
	int result;
	SOCKET ComSocket; 
	uint8_t size = 0;
	char* buffer = new char[size];

	public:

	int tcp_recv_whole(SOCKET s, char* buf, int len);
	int tcp_send_whole(SOCKET skSocket, const char* data, uint16_t length);
	int init(uint16_t port);
	int readMessage(char* buffer, int32_t size);
	int sendMessage(char* data, int32_t length);
	void stop();

};