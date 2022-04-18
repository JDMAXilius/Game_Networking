#include <iostream>
#include "ClientD.h"
#include <string>
#include <sstream>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#include <cstdlib>
#include <fstream>
#include <ctime>

using namespace std;

bool recvTrun = false;
string ip = "127.0.0.1";
int Port = 31337;
sockaddr_in addSock;
const int MAX_BUFFER_SIZE = 4096;
SOCKET sock;
ostringstream sender;

void messageSend(string mes) {
	if (!mes.empty() && sock != INVALID_SOCKET)
	{
		send(sock, mes.c_str(), mes.size() + 1, 0);
	}
}

// Init the winsock & start the client
bool initSock() {
	WSADATA wsadata;
	WORD isWord = MAKEWORD(2, 2);

	int init = WSAStartup(isWord, &wsadata);
	// Checking to see if the winsock is working
	if (init != 0) {
		cout << "Winsock not started..." << endl;
		return false;
	}
	return true;
}

string messageType(string messageIsSend, string message)
{
	ostringstream sender;
	sender << message;
	messageIsSend = sender.str();
	return messageIsSend;
}

int main()
{
	ClientD* client = new ClientD;

	string message = "a";
	string rmsg;
	string mess2 = "";
	string usernameEntered;
	//ostringstream ss;


	cout << "Enter your username." << endl;
	cin >> usernameEntered;
	client->userName = usernameEntered;
	cout << "Conecting to server port: " << endl;// << serverAddr.sin_port;
	cout << "Waiting for registration..." << endl;
	bool registation = false;
	while (!registation)
	{
		cout << "register!" << endl;
		getline(cin, rmsg);
		if (rmsg == "register" || rmsg == "Register" || rmsg == "REGISTER")
		{
			cout << "Registration complete press enter to continue!" << endl;

			registation = true;
			//false;
		}
		else
		{
			cout << "need to register to continie; type: register" << endl;
		}
	}
	//initSock();
	WSADATA wsaData;
	WORD isWord = MAKEWORD(2, 2);
	bool initSock = false;
	int init = WSAStartup(isWord, &wsaData);
	// Checking to see if the winsock is working.
	if (init != 0) {
		cout << "Winsock isn't starting!" << endl;
	}
	else
	{
		initSock = true;
	}
	if(initSock)
	{
		client->connectSock();

		//client->recvThread = thread([&]
		//	{
		//		recvTrun = true;
		//		while (recvTrun) {
		//			// Starting message buffer and cleaning out
		//			char buffer[MAX_BUFFER_SIZE];
		//			ZeroMemory(buffer, MAX_BUFFER_SIZE);

		//			int byteRec = recv(sock, buffer, MAX_BUFFER_SIZE, 0);
		//			if (byteRec > 0)
		//			{
		//				cout << string(buffer, 0, byteRec) << endl;
		//			}
		//		}
		//	});
		client->recvThread = thread([&] {
			client->threadRecv();
			});

		while (true) 
		{
			getline(cin, message);
			string messageSend;
			// All User Commands .
			
			// User exit command
			if (client->joinedChat == false && (message == "exit" || message == "Exit" || message == "EXIT")) {
				cout << "User has quit the server chat!\n" << endl;
				messageSend = messageType(messageSend, message);
				string exitMsg = "User has quit the server chat!\n";
				client->sendMsg(exitMsg);
				PostMessage(GetConsoleWindow(), WM_CLOSE, 0, 0);
				exit(0);
			}
			// User get list command.
			else if (client->joinedChat == false && (message == "get list" || message == "Get List" || message == "GET LIST")) {

				messageSend = messageType(messageSend, message);
			}
			// User get log command.
			else if (client->joinedChat == false && (message == "get log" || message == "Get Log" || message == "GET LOG")) {

				messageSend = messageType(messageSend, message);
			}
			//Send normal message
			else if (client->joinedChat == false) {
				ostringstream sender;
				sender << client->userName << ": " << message;
				messageSend = sender.str();
				/*myfile << messageIsSend << "\n";*/
			}
			// Registering
			else if (client->joinedChat == true) {
				ostringstream sender;
				sender << client->userName;
				messageSend = sender.str();
				/*myfile << messageIsSend << "\n";*/
				client->joinedChat = false;
			}
			client->sendMsg(messageSend);
		}
	}
	closesocket(sock);
	if (recvTrun) {
		recvTrun = false;
		client->recvThread.join();
	}
	delete client;
	cin.get();
	return WSACleanup();

}

