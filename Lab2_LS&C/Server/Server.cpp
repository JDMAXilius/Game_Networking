#include <iostream>
#include <string>
#include <string>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <sstream>
using namespace std;

// CREATE FILE log
std::fstream myfile("UserLog.txt", ios::app);
std::string logFile = "UserLog.txt";

string convert(string sbuffer, char buf[])
{
	// Converts buffer into string sbuffer.
	for (int i = 0; i < strlen(buf); i++)
	{
		sbuffer += buf[i];
		//return sbuffer;
	}
	return sbuffer;
}

void fileLog(SOCKET sock)
{
	myfile.close();
	myfile.open(logFile, ios::app);
	cout << "User is requesting a log file." << endl;
	string mess2;
	string space = "\n\n";
	send(sock, space.c_str(), 4096, 0);
	myfile.close();
	myfile.open(logFile, ios::in);
	while (getline(myfile, mess2)) 
	{
		cout << mess2 << endl;
		send(sock, mess2.c_str(), 4096, 0);
		//cout << string(buf, 0, bytesReceived) << "\n";
	}
	myfile.close();
	myfile.open(logFile, ios::app);
	cout << "User is requesting a log file." << endl;
}

int main()
{
	int listenerPort = 31337;
	string ipServer = "127.0.0.1";

	//
	SOCKET outSock1;
	//std::string msgSent;

	string message = "z";
	string rmsg;
	//string mess2 = "";
	string usernameEntered;

	//Server server(ipServer, 31337);
	vector<string> users{};
	WSADATA wsaData;
	WORD isWord = MAKEWORD(2, 2);

	int init = WSAStartup(isWord, &wsaData);
	// Checking to see if the winsock is working.
	if (init != 0) {
		cout << "Winsock isn't starting!" << endl;
	}

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	// Binds specific data like ip and port to socket, identifies IPv4 ips
	if (listenSocket == INVALID_SOCKET)
	{
		printf("DEBUG// Socket function incorrect\n");
		return WSACleanup();
	}

	sockaddr_in addSock;
	addSock.sin_family = AF_INET;
	addSock.sin_port = htons(31337);
	inet_pton(AF_INET, ipServer.c_str(), &addSock.sin_addr);

	int isBind = bind(listenSocket, (sockaddr*)&addSock, sizeof(addSock));
	// Checking to see if the bind is working
	if (isBind == SOCKET_ERROR)
	{
		printf("DEBUG// Bind function incorrect\n");
		return WSACleanup();
	}

	int sockCheck = listen(listenSocket, SOMAXCONN);
	if (sockCheck == SOCKET_ERROR)
	{
		printf("DEBUG// Listen function incorrect\n");
		return WSACleanup();
	}

	// Message buffer
	const int MAX_BUFFER_SIZE = 4096;
	char buf[MAX_BUFFER_SIZE];

	bool serverActive = true;
	//const int MAX_BUFFER_SIZE = 4096;
	//char buf[MAX_BUFFER_SIZE];

	// String conversion buffer
	string sbuffer = "";

	// Register user
	bool registerB = false;
	cout << "Listener on port " << addSock.sin_port;
	printf("\nWaiting for connections...\n");
	int clientAmount = 0;
	while (true) {
		if (listenSocket == INVALID_SOCKET) { break; }
		// Socket storage, clears the descriptor, adds the above listening socket.
		fd_set master;
		FD_ZERO(&master);
		FD_SET(listenSocket, &master);

		while (true) {
			// New descriptor.
			fd_set copy = master;
			// Using select status to return working sockets.
			int socketCount = select(0, &copy, nullptr, nullptr, nullptr);
			for (int i = 0; i < socketCount; i++) {
				// All active sockets in descriptor
				SOCKET sock = copy.fd_array[i];
				// Accept a new connection
				if (clientAmount <= 2) {
					if (sock == listenSocket) {
						SOCKET client = accept(listenSocket, nullptr, nullptr);
						FD_SET(client, &master);
						clientAmount++;
						// If user already exists in users vector while registering
						if (find(users.begin(), users.end(), buf) != users.end()) {
							cout << "User already exists. Client disconnected." << endl;
							myfile << "User already exists. Client disconnected." << endl;
							closesocket(sock);
							FD_CLR(sock, &master);
							clientAmount--;
						}
						// Greet new client if not already registered
						//    Makes note in server of new user
						else 
						{
							std::string welcomeMsg = "Welcome to the Chat press enter to continue.\n";			//Notify client that he entered the chat. 
							send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
							std::cout << "New user joined the chat." << std::endl;			//Log connection on server side. 
							myfile << "New user joined the chat." << std::endl;
							registerB = false;
						}
					}
					// Message is being received.
					else {
						// The buffer needs to clear.
						ZeroMemory(buf, MAX_BUFFER_SIZE);
						// Receiving buffer message.
						int bytesReceived = recv(sock, buf, MAX_BUFFER_SIZE, 0);
						// Checks to see if there is a message present or not.
						if (bytesReceived <= 0) {
							closesocket(sock);
							FD_CLR(sock, &master);
						}
						// Notifys other clients via looping through sockets
						else {
							for (int i = 0; i < master.fd_count; i++) {
								SOCKET outSock = master.fd_array[i];
								if (outSock != listenSocket) {
									if (outSock == sock) {
										//}
										std::string msgSent = "Message delivered.";
										send(outSock, msgSent.c_str(), msgSent.size() + 1, 0);	//Notify the client that the msg was delivered. 	
										myfile << msgSent << endl;
									}
									else {						//If the current sock is not the sender -> it should receive the msg. 
										//std::ostringstream ss;
										//ss << "SOCKET " << sock << ": " << buf << "\n";
										//std::string strOut = ss.str();
										send(outSock, buf, bytesReceived, 0);		//Send the msg to the current socket. 
										myfile << buf << endl;
									}
								}
							}
						}

						sbuffer = convert(sbuffer, buf);

						// Exit command.
						if (sbuffer == "exit" || sbuffer == "Exit" || sbuffer == "EXIT") {
							cout << "User had exit" << endl;
							if (find(users.begin(), users.end(), sbuffer) != users.end())
							closesocket(sock);
							//FD_CLR(sock, &master.fd_array[3]);
							FD_CLR(sock, &master);
							clientAmount--;

						}
						// Get list command.
						else if (sbuffer == "get list" || sbuffer == "Get List" || sbuffer == "GET LIST") {
							std::cout << "from server username list: " << std::endl;
							string list = "Users List: ";
							send(sock, list.c_str(), MAX_BUFFER_SIZE, 0);
							myfile << list << endl;

							for (int i = 0; i < users.size(); i++) {
							
								cout << users[i] << endl;
								myfile << users[i] << endl;
								send(sock, users[i].c_str(), bytesReceived, 0);
							}
							
						}
						// Get Log command.
						else if (sbuffer == "get log" || sbuffer == "Get Log" || sbuffer == "GET LOG") {
			
							fileLog( sock );
						}
				
						// How it knows to register or not.
						if (registerB == false) {
							users.push_back(sbuffer);
							cout << string(buf, 0, bytesReceived);
							myfile << buf << " -- has joined the chat! \n";
							cout << " -- has joined the chat!" << endl;
							registerB = true;
						}
						else 
						{ 
							myfile << string(buf, 0, bytesReceived) << endl;
							cout << string(buf, 0, bytesReceived) << "\n"; 
						}
					}
				}
				else {
					//FD_CLR(sock, &master);
					FD_CLR(sock, &master.fd_array[3]);
					printf("The server is full!");
					myfile << "The server is full!" << endl;
					clientAmount--;
				}
				sbuffer = "";
				// Clearing the buffer again before user register check
				ZeroMemory(buf, MAX_BUFFER_SIZE);
			}
		}

		FD_CLR(listenSocket, &master);
		shutdown(listenSocket, SD_BOTH);
		closesocket(listenSocket);
	}

	return WSACleanup();

}

