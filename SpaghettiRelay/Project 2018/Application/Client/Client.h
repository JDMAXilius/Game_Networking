#pragma once
#include "../platform.h"
#include "../definitions.h"

class Client
{
	SOCKET ComSocket;
	sockaddr_in serverAddr;
	int result;
	uint8_t size = 30;
	char sendbuffer[30];

	public:

	int tcp_recv_whole(SOCKET s, char* buf, int len);
	int tcp_send_whole(SOCKET skSocket, const char* data, uint16_t length);
	int init(uint16_t port, char* address);
	int readMessage(char* buffer, int32_t size);
	int sendMessage(char* data, int32_t length);
	void stop();
};