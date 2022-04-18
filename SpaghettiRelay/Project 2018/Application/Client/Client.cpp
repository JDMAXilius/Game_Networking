#include "Client.h"

// loop recv func
int Client::tcp_recv_whole(SOCKET s, char* buf, int len)
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
int Client::tcp_send_whole(SOCKET skSocket, const char* data, uint16_t length)
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

int Client::init(uint16_t port, char* address)
{
	////On a successful connection, returns SUCCESS.
	////Socket
	////If address is not in dotted - quadrant format, returns ADDRESS_ERROR.
	//if (serverAddr.sin_addr.S_un.S_addr == SOCKET_ERROR)
	//{
	//	//printf("DEBUG// Connect function incorrect\n");
	//	return ADDRESS_ERROR;
	//}
	////If error was encountered when creating a socket, returns SETUP_ERROR.
	//if (ComSocket == INVALID_SOCKET)
	//{
	//	//printf("DEBUG// Socket function incorrect\n");
	//	return SETUP_ERROR;
	//}
	////Connect
	//serverAddr.sin_family = AF_INET;
	//serverAddr.sin_addr.S_un.S_addr = inet_addr(address);
	//serverAddr.sin_port = htons(port);

	//ComSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//
	//result = connect(ComSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	//
	////If error appeared during connectand was not caused by shutdown, returns CONNECT_ERROR.
	//if (result != SHUTDOWN)
	//{
	//	return CONNECT_ERROR;
	//}
	////If error appeared during connectand WAS caused by shutdown, returns SHUTDOWN.
	//if(result == SHUTDOWN)
	//{
	//	return SHUTDOWN;
	//}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.S_un.S_addr = inet_addr(address);
	//If no address, address error
	if (serverAddr.sin_addr.s_addr == INADDR_NONE) 
	{ 
		return ADDRESS_ERROR; 
	}
	
	ComSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ComSocket == INVALID_SOCKET) 
	{ 
		return SETUP_ERROR; 
	}
	//If error, disconnect or shutdown
	result = connect(ComSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (result == SOCKET_ERROR) 
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
	return SUCCESS;
}

//init(address, port)
//{
//	Create a socket(socket().)
//		Convert the addressand port into a sock_addr structure.
//		Connect the socket to the sock_addr(connect().)
//		NOTE: If there are errors in any call, return the correct error
//		code.Otherwise, return SUCCESS.
//}

int Client::readMessage(char* buffer, int32_t size)
{
	int mesLength = 0;
	////On a successful receiveand write to buffer, returns SUCCESS.
	////Communication
	//int y;
	//result = tcp_recv_whole(ComSocket, (char*)&y, 1);
	////if ((result == SOCKET_ERROR) || (result == 0))
	////{
	////	int error = WSAGetLastError();
	////	//printf("DEBUG// send is incorrect\n");
	////	return SETUP_ERROR;
	////}
	//
	////If error appeared during sendand was not caused by shutdown, returns DISCONNECT.
	//if (result != SHUTDOWN)
	//{
	//	return DISCONNECT;
	//}
	////If error appeared during sendand WAS caused by shutdown, returns SHUTDOWN.
	//if (result == SHUTDOWN)
	//{
	//	return SHUTDOWN;
	//}
	//
	////If the message is longer than size, returns PARAMETER_ERROR
	//if (y > size)
	//{
	//	return PARAMETER_ERROR;
	//}
	//result = tcp_recv_whole(ComSocket, buffer, y);
	////if ((result == SOCKET_ERROR) || (result == 0))
	////{
	////	int error = WSAGetLastError();
	////	//printf("DEBUG// send is incorrect\n");
	////	return SETUP_ERROR;
	////}
	//
	////If error appeared during sendand was not caused by shutdown, returns DISCONNECT.
	//if (result != SHUTDOWN)
	//{
	//	return DISCONNECT;
	//}
	////If error appeared during sendand WAS caused by shutdown, returns SHUTDOWN.
	//if (result == SHUTDOWN)
	//{
	//	return SHUTDOWN;
	//}
	////If the message is longer than size, returns PARAMETER_ERROR
	//if (y > size)
	//{
	//	return PARAMETER_ERROR;
	//}
	
	//resived
	//result = recv(ComSocket, (char*)&mesLen, sizeof(char), 0);
	//result = tcp_recv_whole(ComSocket, (char*)&mesLen, sizeof(char), 0);
	result = tcp_recv_whole(ComSocket, (char*)&mesLength, 1);
	if (mesLength < 1) 
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
	
	//// decode
	//int encryptData = 13;
	//char message = 'a';
	//for (size_t i = 0; i < size; i++)
	//{
	//	if (isupper(buffer[i]))
	//	{
	//		//buffer[i] = (char)((int)(buffer[i] - encryptData + 65) % 26 + 65);
	//		buffer[i] = message * ((int)(buffer[i] - encryptData + 65) % 26 + 65);
	//		//buffer[i] = message;
	//	}
	//	else
	//	{
	//		//buffer[i] = (char)((int)(buffer[i] - encryptData + 97) % 26 + 97);
	//		buffer[i] = message * ((int)(buffer[i] - encryptData + 97) % 26 + 97);
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

	//read
	for (int32_t i = 0; i < size; i++) { buffer[i] = '\0'; }
	
	
	result = tcp_recv_whole(ComSocket, buffer, mesLength);
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

	if (mesLength > size) 
	{ 
		return PARAMETER_ERROR;
	}

	return SUCCESS;
}
int Client::sendMessage(char* data, int32_t length)
{
	////On a successful write to stream, returns SUCCESS.
	//int y;
	////result = tcp_send_whole(ComSocket, (char*)&y, 1);
	////if ((result == SOCKET_ERROR) || (result == 0))
	////{
	////	int error = WSAGetLastError();
	////	//printf("DEBUG// send is incorrect\n");
	////	return SETUP_ERROR;

	////}
	//
	////If error appeared during sendand was not caused by shutdown, returns DISCONNECT.
	//if (result != SHUTDOWN)
	//{
	//	return DISCONNECT;
	//}
	////If error appeared during sendand WAS caused by shutdown, returns SHUTDOWN.
	//if (result == SHUTDOWN)
	//{
	//	return SHUTDOWN;
	//}
	//
	////If length is less than 0 or greater than 255, returns PARAMETER_ERROR.
	//if (length < 0 || length > 255)
	//{
	//	return PARAMETER_ERROR;
	//}

	////result = tcp_send_whole(ComSocket, data,  y);
	////if ((result == SOCKET_ERROR) || (result == 0))
	////{
	////	int error = WSAGetLastError();
	////	//printf("DEBUG// send is incorrect\n");
	////	return SETUP_ERROR;

	////}
	//
	////If error appeared during sendand was not caused by shutdown, returns DISCONNECT.
	//if (result != SHUTDOWN)
	//{
	//	return DISCONNECT;
	//}
	////If error appeared during sendand WAS caused by shutdown, returns SHUTDOWN.
	//if (result == SHUTDOWN)
	//{
	//	return SHUTDOWN;
	//}
	////If length is less than 0 or greater than 255, returns PARAMETER_ERROR.
	//if (length < 0 || length > 255)
	//{
	//	return PARAMETER_ERROR;
	//}
	// 
	//if (result == SHUTDOWN) { return DISCONNECT; }
	char* y = new char[length + 1];
	y[0] = length;
	
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
	//
	//// encode
	//int encryptNum = 13;
	//for (size_t i = 0; i < length; i++)
	//{
	//	if (isupper(data[i])) {
	//		data[i] = (char)((int)(data[i] + encryptNum - 65) % 26 + 65);
	//	}
	//	else
	//	{
	//		data[i] = (char)((int)(data[i] + encryptNum - 97) % 26 + 97);
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
		//if (isupper(data[i]))
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
	//		y[i] = cC;
	//	}
	//	//Upper case
	//	if (cC >= 'A' && cC <= 'Z')
	//	{
	//		cC = cC + shift;
	//		if (cC > 'Z')
	//		{
	//			cC = cC - 'Z' + 'A' - 1;
	//		}
	//		y[i] = cC;
	//	}
	//}

	for (int32_t i = 1; i < length; i++) { y[i] = data[i - 1]; }
	if (length > 0) { y[length] = '\0'; }
	result = send(ComSocket, y, length + 1, 0);
	//result = tcp_send_whole(ComSocket, data, length);
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
	if (length < 0 || length > 255) 
	{ 
		return PARAMETER_ERROR; 
	}
	delete[] y;
	return SUCCESS;
}
void Client::stop()
{
	// close sockets
	shutdown(ComSocket, SD_BOTH);
	closesocket(ComSocket);
}