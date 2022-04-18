#include "Server.h"
//#include <iostream>

// loop recv func
int Server::tcp_recv_whole(SOCKET s, char* buf, int len)
{
	int total = 0;

	do
	{
		int ret = recv(s, buf + total, len - total, 0);
		if (ret < 1)
			return ret;
		else
			total += ret;

	} while (total < len);

	return total;
}

// loop send func
int Server::tcp_send_whole(SOCKET skSocket, const char* data, uint16_t length)
{
	int result;
	int bytesSent = 0;

	while (bytesSent < length)
	{
		result = send(skSocket, (const char*)data + bytesSent, length - bytesSent, 0);

		if (result <= 0)
			return result;

		bytesSent += result;
	}

	return bytesSent;
}

int Server::init(uint16_t port)
{
	////Return Values On a successful connection, returns SUCCESS.
	////Socket
	//listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//
	////If error was encountered when binding the socket, returns BIND_ERROR.
	////Bind
	//serverAddr.sin_family = AF_INET;
	//serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	//serverAddr.sin_port = htons(port);

	//result = bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	//if (result == SOCKET_ERROR)
	//{
	//	//printf("DEBUG// Bind function incorrect\n");
	//	return BIND_ERROR;
	//}
	//
	////Listen
	//result = listen(listenSocket, 1);
	//if (result == SOCKET_ERROR)
	//{
	//	//printf("DEBUG// Listen function incorrect\n");
	//	return SETUP_ERROR;
	//}
	//int y = sizeof(serverAddr);
	//ComSocket = accept(listenSocket, NULL, NULL);
	//if (ComSocket == SOCKET_ERROR)
	//{
	//	if (result != SHUTDOWN)
	//	{
	//		return CONNECT_ERROR;
	//	}
	//	//If error appeared during acceptand WAS caused by shutdown, returns SHUTDOWN.
	//	else 
	//	{
	//		return SHUTDOWN;
	//	}
	//}
	
	
	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	result = bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (result == SOCKET_ERROR)
	{ 
		return BIND_ERROR; 
	}
	if (listenSocket == INVALID_SOCKET) 
	{ 
		return SETUP_ERROR; 
	}
	result = listen(listenSocket, 2);
	if (result == SOCKET_ERROR) 
	{ 
		return SETUP_ERROR; 
	}
	//int y = sizeof(sockAdd);
	ComSocket = accept(listenSocket, NULL, NULL);
	if (ComSocket == SOCKET_ERROR) 
	{
		if (result != SHUTDOWN) 
		{ 
			return CONNECT_ERROR;
		}
		if (result == SHUTDOWN)
		{ 
			return SHUTDOWN; 
		}
	}
	//return SHUTDOWN;
	return SUCCESS;
}

//init(port)
//{
//	Create a socket(socket().)
//		Bind the socket to the specified port(bind().)
//		Set up a listening queue for connections(listen().)
//		Wait forand accept a single connection from a client(accept().)
//		NOTE: If there are errors in any call, return the correct error
//		code.Otherwise, return SUCCESS.
//}

int Server::readMessage(char* buffer, int32_t size)
{
	int mesLength = 0;

	//result = recv(ComSocket, (char*)&mesLen, sizeof(char), 0);
	result = tcp_recv_whole(ComSocket, (char*)&mesLength, 1);
	if (mesLength < 1) {
		if (result != SHUTDOWN) 
		{ 
			return DISCONNECT; 
		}
		if (result == SHUTDOWN) 
		{ 
			return SHUTDOWN; 
		}
	}
	if (mesLength > size) 
	{ 
		return PARAMETER_ERROR;
	}

	//// decode
	//int encryptData = 13;
	//char message = 'a';
	//for (size_t i = 0; i < size; i++)
	//{
	//	if (isupper(buffer[i]))
	//	{
	//		//buffer[i] = (char)((int)(buffer[i] - encryptData + 65) % 26 + 65);
	//		buffer[i] = (char) ((int)(buffer[i] - encryptData + 65) % 26 + 65);
	//		//buffer[i] = message;
	//	}
	//	else
	//	{
	//		//buffer[i] = (char)((int)(buffer[i] - encryptData + 97) % 26 + 97);
	//		buffer[i] = (char)((int)(buffer[i] - encryptData + 97) % 26 + 97);
	//		//buffer[i] = message;
	//	}
	//}

	//// decode
	//int encryptNum = 13;
	//for (size_t i = 0; i < size; i++)
	//{
	//	if (isupper(buffer[i]))
	//	{
	//		buffer[i] = (char)((int)(buffer[i] - encryptNum + 65) % 26 + 65);
	//	}
	//	else
	//	{
	//		buffer[i] = (char)((int)(buffer[i] - encryptNum + 97) % 26 + 97);
	//	}
	//}

	// encode
	int encryptData = 13;
	char cC;
	int shift = 5;
	int i = 0;
	while (i < size)
	{
		cC = buffer[i];
		//if (isupper(data[i]))
		if (cC >= 'A' && cC <= 'Z')
		{
			buffer[i] = (char)((int)(buffer[i] - encryptData + 65) % 26 + 65);
		}
		if (cC >= 'a' && cC <= 'z')
		{
			buffer[i] = (char)((int)(buffer[i] - encryptData + 97) % 26 + 97);
		}
		i++;
	}

	//char cC;
	//int shift = 5;

	//for (size_t i = 0; i < size; i++)
	//{
	//	cC = buffer[i];
	//	//lower case
	//	if (cC >= 'a' && cC <= 'z')
	//	{
	//		cC = cC + shift;
	//		if (cC > 'z')
	//		{
	//			cC = cC + 'z' - 'a' + 1;
	//		}
	//		buffer[i] = cC;
	//	}
	//	/*if (isupper(cC))
	//	{
	//		cC = cC + shift;
	//		if (cC > 'Z')
	//		{
	//			cC = cC + 'Z' - 'A' + 1;
	//		}
	//		buffer[i] = cC;
	//	}*/
	//	//Upper case
	//	if (cC >= 'A' && cC <= 'Z')
	//	{
	//		cC = cC + shift;
	//		if (cC > 'Z')
	//		{
	//			cC = cC + 'Z' - 'A' + 1;
	//		}
	//		buffer[i] = cC;
	//	}
	//}


	for (int32_t i = 0; i < size; i++) 
	{
		buffer[i] = '\0';
	}

	//qtyError = recv(ComSocket, buffer + recieved, mesLen - recieved, 0);
	result = tcp_recv_whole(ComSocket, buffer, mesLength);
	if (result < 1)
	{
		if (result != SHUTDOWN)
		{
			return DISCONNECT;
		}
		if (result == SHUTDOWN)
		{
			return SHUTDOWN;
		}
	}
	
	if (mesLength > size) 
	{ 
		return PARAMETER_ERROR; 
	}
	return SUCCESS;
}
int Server::sendMessage(char* data, int32_t length)
{
	char* x = new char[length + 1];
	x[0] = length;

	//// decode
	//int encryptData = 13;
	//char message = 'a';
	//for (size_t i = 0; i < size; i++)
	//{
	//	if (isupper(data[i]))
	//	{
	//		//buffer[i] = (char)((int)(buffer[i] - encryptData + 65) % 26 + 65);
	//		data[i] = message * ((int)(data[i] + encryptData - 65) % 26 + 65);
	//		//data[i] = message;
	//	}
	//	else
	//	{
	//		//buffer[i] = (char)((int)(buffer[i] - encryptData + 97) % 26 + 97);
	//		data[i] = message * ((int)(data[i] + encryptData - 97) % 26 + 97);
	//		//data[i] = message;
	//	}
	//}

	// encode
	int encryptData = 13;
	char cC;
	int shift = 5;
	int i = 0;
	while (i < length)
	{
		cC = data[i];
		if (cC >= 'A' && cC <= 'Z')
		{
			data[i] = (char)((int)(data[i] + encryptData - 65) % 26 + 65);
		}
		if (cC >= 'a' && cC <= 'z')
		{
			data[i] = (char)((int)(data[i] + encryptData - 97) % 26 + 97);
		}
		i++;
	}

	//char cC;
	//int shift = 5;

	//for (size_t i = 0; i < length; i++)
	//{
	//	cC = data[i];
	//	//lower case
	//	if (cC >= 'a' && cC <= 'z')
	//	{
	//		cC = cC + shift;
	//		if (cC > 'z')
	//		{
	//			cC = cC - 'z' + 'a' - 1;
	//		}
	//		x[i] = cC;
	//	}
	//	//Upper case
	//	if (cC >= 'A' && cC <= 'Z')
	//	{
	//		cC = cC + shift;
	//		if (cC > 'Z')
	//		{
	//			cC = cC - 'Z' + 'A' - 1;
	//		}
	//		x[i] = cC;
	//	}
	//}

	for (int32_t i = 1; i < length; i++) { x[i] = data[i - 1]; }
	if (length > 0) { x[length] = '\0'; }
	result = send(ComSocket, x, length + 1, 0);
	//result = tcp_send_whole(ComSocket, data, length);

	if (result < 1) {
		if (result != SHUTDOWN)
		{
			return DISCONNECT;
		}
		if (result == SHUTDOWN)
		{
			return SHUTDOWN;
		}
	}
	if (length < 0 || length > 255) 
	{ 
		return PARAMETER_ERROR; 
	}

	delete[] x;

	return SUCCESS;
}
void Server::stop()
{
	// close both sockets
	shutdown(listenSocket, SD_BOTH);
	closesocket(listenSocket);

	shutdown(ComSocket, SD_BOTH);
	closesocket(ComSocket);
}