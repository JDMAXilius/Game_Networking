// Client.cpp : handles all client network functions.
#include "Client.h"
#include "../NetworkMessage.h"

// Initializes the client; connects to the server.
int Client::init(char* address, unsigned short port, byte _player)
{
	state.player0.keyUp = state.player0.keyDown = false;
	state.player1.keyUp = state.player1.keyDown = false;
	state.gamePhase = WAITING;
	// TODO:
	//       1) Set the player.
	//       2) Set up the connection.
	//       3) Connect to the server.
	//       4) Get response from server.
	//       5) Make sure to mark the client as running.
	player = _player;
	NetworkMessage msgOut(IO::_OUTPUT);
	NetworkMessage msgIn(IO::_INPUT);


	Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (Socket == INVALID_SOCKET)
	{
		return SETUP_ERROR;
	}

	addSock.sin_family = AF_INET;
	addSock.sin_port = htons(port);
	addSock.sin_addr.S_un.S_addr = inet_addr(address);

	int result = connect(Socket, (sockaddr*)&addSock, sizeof(sockaddr));
	if (result == SOCKET_ERROR)
	{
		return DISCONNECT;
	}

	msgOut.writeByte(1);
	msgOut.writeByte(1);
	msgOut.writeByte(CL_CONNECT);
	msgOut.writeByte(player);
	sendNetMessage(Socket, msgOut);
	recvNetMessage(Socket, msgIn);
	sh1 = msgIn.readShort();
	if (msgIn.readByte() != SV_OKAY)
	{
		active = false;
		return SHUTDOWN;
	}
	//else if (msgIn.readByte() == SV_FULL)
	//{
	//	active = false;
	//	state.gamePhase = RUNNING;
	//	//return SHUTDOWN;
	//}
	else
	{

		state.gamePhase = RUNNING;
		active = true;

	}
	
	return SUCCESS;
}

// Receive and process messages from the server.
int Client::run()
{
	// TODO: Continuously process messages from the server aslong as the client running.
	// HINT: Set game phase to DISCONNECTED on SV_CL_CLOSE! (Try calling stop().)
	// HINT: You can keep track of the number of snapshots with a static variable...
	//count = 0;
	int Recv;
	short Send;
	while (active && state.gamePhase != DISCONNECTED) {
		NetworkMessage NetMesgInput(IO::_INPUT);
		Recv = recvNetMessage(Socket, NetMesgInput);
		Send = NetMesgInput.readShort();
		if (Send > sh1) {
			sh1 = Send;
			if (Recv > 0) {
				count++;
				if (count % 10 == 0)
				{

					sendAlive();
				}
				switch (NetMesgInput.readByte()) {
				case SV_SNAPSHOT:
					//count++;
					state.gamePhase = NetMesgInput.readByte();	//	Phase

					state.ballX = NetMesgInput.readShort();	//	BallX
					state.ballY = NetMesgInput.readShort();	//	BallY

					state.player0.y = NetMesgInput.readShort();	//	P0 - Y
					state.player0.score = NetMesgInput.readShort();	//	P0 - Score

					state.player1.y = NetMesgInput.readShort();	//	P1 - Y
					state.player1.score = NetMesgInput.readShort();	//	P1 - Score
					break;

				case SV_CL_CLOSE:
					state.gamePhase = DISCONNECTED;
					return SHUTDOWN;
				}
				//if (NetMesgInput.readByte())
				//{
				//	if (SV_SNAPSHOT)
				//	{
				//		//count++;
				//		state.gamePhase = NetMesgInput.readByte();	//	Phase

				//		state.ballX = NetMesgInput.readShort();	//	BallX
				//		state.ballY = NetMesgInput.readShort();	//	BallY

				//		state.player0.y = NetMesgInput.readShort();	//	P0 - Y
				//		state.player0.score = NetMesgInput.readShort();	//	P0 - Score

				//		state.player1.y = NetMesgInput.readShort();	//	P1 - Y
				//		state.player1.score = NetMesgInput.readShort();	//	P1 - Score
				//		break;
				//	}
				//	if (SV_CL_CLOSE)
				//	{
				//		state.gamePhase = DISCONNECTED;
				//		return SHUTDOWN;
				//	}
				//}
			}
			else {

				return MESSAGE_ERROR;
			}
		}
	}
	//return DISCONNECT;
	return true;
}

// Clean up and shut down the client.
void Client::stop()
{
	// TODO:
	//       1) Make sure to send a SV_CL_CLOSE message.
	//       2) Make sure to mark the client as shutting down and close socket.
	//       3) Set the game phase to DISCONNECTED.
	sendClose();
	active = false;
	shutdown(Socket, SD_BOTH);
	closesocket(Socket);
	state.gamePhase = DISCONNECTED;
}

// Send the player's input to the server.
int Client::sendInput(byte keyUp, byte keyDown, byte keyQuit)
{
	if (keyQuit)
	{
		stop();
		return SHUTDOWN;
	}

	cs.enter();
	if (player == 0)
	{
		state.player0.keyUp = keyUp;
		state.player0.keyDown = keyDown;
	}
	else
	{
		state.player1.keyUp = keyUp;
		state.player1.keyDown = keyDown;
	}
	cs.leave();

	//TODO:	Transmit the player's input status.
	unsigned short key = CL_KEYS;
	NetworkMessage msgOut(IO::_OUTPUT);
	msgOut.writeByte(key);
	msgOut.writeByte(keyUp);
	msgOut.writeByte(keyDown);
	sendNetMessage(Socket, msgOut);
	return SUCCESS;
}

// Copies the current state into the struct pointed to by target.
void Client::getState(GameState* target)
{
	// TODO: Copy state into target.
	target->player0 = state.player0;
	target->ballX = state.ballX;

	target->gamePhase = state.gamePhase;

	target->ballY = state.ballY;
	target->player1 = state.player1;
}

// Sends a SV_CL_CLOSE message to the server (private, suggested)
void Client::sendClose()
{
	// TODO: Send a CL_CLOSE message to the server.
	unsigned short key = SV_CL_CLOSE;
	NetworkMessage msgOut(IO::_OUTPUT);
	msgOut.writeByte(key);
	sendNetMessage(Socket, msgOut);
}

// Sends a CL_ALIVE message to the server (private, suggested)
int Client::sendAlive()
{
	// TODO: Send a CL_ALIVE message to the server.
	unsigned short key = CL_ALIVE;
	NetworkMessage msgOut(IO::_OUTPUT);
	msgOut.writeByte(key);
	sendNetMessage(Socket, msgOut);
	return SUCCESS;
}
