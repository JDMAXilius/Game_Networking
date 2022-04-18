#include "ClientD.h"
#include <iostream>
#include <string>
#include <thread>

using namespace std;

//When we create the client, we do not want the thread to run & try to receive data from the server until 
ClientD::ClientD()
{
	recvTrun = false;
}


ClientD::~ClientD()
{
	//closesocket(sock);
	//WSACleanup();
	//if (recvTrun) {
	//	recvTrun = false;
	//	recvThread.join();	//Destroy safely to thread. 
	//}
}

SOCKET ClientD::createSocket() {

	SOCKET sSock = socket(AF_INET, SOCK_STREAM, 0);

	if (sSock == INVALID_SOCKET) {
		cout << "Error: can't create socket." << endl;
		WSACleanup();
		return -1;
	}

	// Specify data for add sock structure. 
	addSock.sin_family = AF_INET;
	addSock.sin_port = htons(Port);
	inet_pton(AF_INET, ip.c_str(), &addSock.sin_addr);
	cout << "Port:  " << addSock.sin_port << "  Socket:  " << sSock << endl;
	return sSock;

}
void ClientD::connectSock() {

	// Checking for connection to socket
	sock = createSocket();
	int con = connect(sock, (sockaddr*)&addSock, sizeof(addSock));
	if (con == SOCKET_ERROR) {
		cout << "Can't connect to server..." << endl;
		closesocket(sock);
		WSACleanup();
		return;
	}
}

void ClientD::threadRecv() {

	recvTrun = true;
	while (recvTrun) {

		char buf[4096];
		ZeroMemory(buf, 4096);

		int bytesReceived = recv(sock, buf, 4096, 0);
		if (bytesReceived > 0) {			//If client disconnects, bytesReceived = 0; if error, bytesReceived = -1;

			std::cout << string(buf, 0, bytesReceived) << std::endl;
		}

	}
}

void ClientD::sendMsg(string txt) {

	if (!txt.empty() && sock != INVALID_SOCKET) {

		send(sock, txt.c_str(), txt.size() + 1, 0);

		//It wouldn't work with the previous version bc while we were constantly listening for received msgs, we would keep caling this fct. 
		//This fct would send the message & try to handle the receiving too. It would get stuck while waiting for a received msg. 
	}

}
