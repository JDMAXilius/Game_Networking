// Server.cpp : Contains all functions of the server.
#include "Server.h"

// Initializes the server. (NOTE: Does not wait for player connections!)
int Server::init(unsigned short port)
{
	initState();

	// TODO:
	//       1) Set up a socket for listening.
	//       2) Mark the server as active.
	unsigned long val = 1;
	//sh1 = 0;
	//sh2 = 0;

	Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (Socket == INVALID_SOCKET)
	{
		return SETUP_ERROR;
	}

	ZeroMemory(&playerAddress[0], sizeof(playerAddress[0]));
	ZeroMemory(&playerAddress[1], sizeof(playerAddress[1]));

	playerAddress[0].sin_family = AF_INET;
	playerAddress[0].sin_port = htons(port);
	playerAddress[0].sin_addr.S_un.S_addr = INADDR_ANY;

	result = bind(Socket, (SOCKADDR*)&playerAddress[0], sizeof(playerAddress[0]));
	if (result == SOCKET_ERROR)
	{
		return ADDRESS_ERROR;
	}

	result = ioctlsocket(Socket, FIONBIO, &val);
	if (result == SOCKET_ERROR)
	{
		return DISCONNECT;
	}

	active = true;
	return SUCCESS;
}

// Updates the server; called each game "tick".
int Server::update()
{
	// TODO: 
	//        1) Get player input and process it.
	//        2) If any player's timer exceeds 50, "disconnect" the player.
	//        3) Update the state and send the current snapshot to each player.
	int error = WSAGetLastError();

	NetworkMessage msgIn(IO::_INPUT);
	//int rev = recvfromNetMessage(Socket, msgIn, &addSock);

	while (recvfromNetMessage(Socket, msgIn, &addSock) >= 0)
	{
		if (result == CONNECT_ERROR)
		{
			return DISCONNECT;
		}
		if (result == SHUTDOWN)
		{
			return SHUTDOWN;
		}
		if (result == SUCCESS)
		{
			parseMessage(addSock, msgIn);
		}
		
	}
	if (playerTimer[0] > 50 && error == EWOULDBLOCK)
	{
		disconnectClient(0);
	}
	else if (playerTimer[1] > 50 && error == EWOULDBLOCK)
	{
		disconnectClient(1);
	}

	updateState();
	sendState();
	return SUCCESS;
}

// Stops the server.
void Server::stop()
{
	// TODO:
	//       1) Sends a "close" message to each client.
	//       2) Shuts down the server gracefully (update method should exit with SHUTDOWN code.)
	sendClose();
	active = false;
	shutdown(Socket, SD_BOTH);
	close(Socket);
	state.gamePhase = DISCONNECTED;
}

// Parses a message and responds if necessary. (private, suggested)
int Server::parseMessage(sockaddr_in& source, NetworkMessage& message)
{	
	
	// TODO: Parse a message from client "source."
	char Read = ' ';
	//char Read = message.readByte();
	int player = -1;
	int soPort = source.sin_port;
	Read = message.readByte();
	int plr0Add = playerAddress[0].sin_port;
	int plr1Add = playerAddress[1].sin_port;
	/*switch (Read) {
	case CL_CONNECT:
		if (noOfPlayers == 0)
		{
			connectClient(0, source);
			sendOkay(source);
		}
		else if (noOfPlayers == 1)
		{
			connectClient(1, source);
			sendOkay(source);
		}

		sendFull(source);

		return SUCCESS;
	}*/
	//If the message is a “connect”, see if that player slot is available.If so, send an “okay”(SV_OKAY) message to the source.
	if (Read == CL_CONNECT)
	//if (CL_CONNECT)
	{

		if (noOfPlayers == 0)
		{
			connectClient(0, source);
			//If so, send an “okay”(SV_OKAY) message to the source.
			if (SV_OKAY)
			{
				sendOkay(source);
			}
			//sendOkay(source);
		}
		else if (noOfPlayers == 1)
		{
			connectClient(1, source);
			//If so, send an “okay”(SV_OKAY) message to the source.
			if (SV_OKAY)
			{
				sendOkay(source);
			}
			//sendOkay(source);
		}
		
		//	If not, send a “full”(SV_FULL) message to the source.Otherwise, determine which player the message is from(source.)
		if (SV_FULL)
		{
			sendFull(source);
		}
		//sendFull(source);

	}
	
	if (soPort == plr0Add)
	{
		player = 0;
	}
	else if (soPort == plr1Add)
	{
		player = 1;
	}

	//	If the message is an “input” message :
	//Apply the key changes to the proper player's input state.
	if (Read == CL_KEYS)
	{
		if (player == 0)
		{
			state.player0.keyUp = message.readByte();
			state.player0.keyDown = message.readByte();
		}
		else if (player == 1)
		{
			state.player1.keyUp = message.readByte();
			state.player1.keyDown = message.readByte();
		}
	}

	//	If the message is an “alive” message :
	//Reset the proper player's player timer.
	if (Read == CL_ALIVE) {
		playerTimer[player] = 0;
	}
	//	If the message is a “close” message : Disconnect the proper player.
	if (Read == SV_CL_CLOSE) {
		disconnectClient(player);
	}
	return SUCCESS;
}

// Sends the "SV_OKAY" message to destination. (private, suggested)
int Server::sendOkay(sockaddr_in& destination)
{
	// TODO: Send "SV_OKAY" to the destination.
	NetworkMessage msgOut(IO::_OUTPUT);
	int Port = destination.sin_port;
	if (Port == playerAddress[0].sin_port)
	{
		sh1++;
		msgOut.writeShort(sh1);
	}
	else if (Port == playerAddress[1].sin_port)
	{
		sh2++;
		msgOut.writeShort(sh2);
	}
	msgOut.writeByte(SV_OKAY);
	sendtoNetMessage(Socket, msgOut, &destination);
	return SUCCESS;
}

// Sends the "SV_FULL" message to destination. (private, suggested)
int Server::sendFull(sockaddr_in& destination)
{
	// TODO: Send "SV_FULL" to the destination.
	NetworkMessage msgOut(IO::_OUTPUT);
	int Port = destination.sin_port;
	if (Port == playerAddress[0].sin_port)
	{
		sh1++;
		msgOut.writeShort(sh1);
	}
	else if(Port == playerAddress[1].sin_port)
	{
		sh2++;
		msgOut.writeShort(sh2);
	}
	msgOut.writeByte(SV_FULL);
	sendtoNetMessage(Socket, msgOut, &destination);
	return SUCCESS;
}

// Sends the current snapshot to all players. (private, suggested)
int Server::sendState()
{
	// TODO: Send the game state to each client.
//player 1
	sh2++;
	NetworkMessage netOut0(IO::_OUTPUT);
	netOut0.writeShort(sh1);
	netOut0.writeByte(SV_SNAPSHOT);
	netOut0.writeByte(state.gamePhase);
	//
	netOut0.writeShort(state.ballX);
	netOut0.writeShort(state.ballY);
	//
	netOut0.writeShort(state.player0.y);
	netOut0.writeShort(state.player0.score);
	//
	netOut0.writeShort(state.player1.y);
	netOut0.writeShort(state.player1.score);
	sendtoNetMessage(Socket, netOut0, &playerAddress[0]);

	//player 2
	sh1++;
	NetworkMessage netOut1(IO::_OUTPUT);
	netOut1.writeShort(sh2);
	netOut1.writeByte(SV_SNAPSHOT);
	netOut1.writeByte(state.gamePhase);
	//
	netOut1.writeShort(state.ballX);
	netOut1.writeShort(state.ballY);
	//
	netOut1.writeShort(state.player0.y);
	netOut1.writeShort(state.player0.score);
	//
	netOut1.writeShort(state.player1.y);
	netOut1.writeShort(state.player1.score);
	sendtoNetMessage(Socket, netOut1, &playerAddress[1]);

	return SUCCESS;
}

// Sends the "SV_CL_CLOSE" message to all clients. (private, suggested)
void Server::sendClose()
{
	// TODO: Send the "SV_CL_CLOSE" message to each client
	sh1++;
	NetworkMessage netOut0(IO::_OUTPUT);
	netOut0.writeShort(sh1);
	netOut0.writeByte(SV_CL_CLOSE);
	sendtoNetMessage(Socket, netOut0, &playerAddress[0]);

	sh2++;
	NetworkMessage netOut1(IO::_OUTPUT);
	netOut1.writeShort(sh1);
	netOut1.writeByte(SV_CL_CLOSE);
	sendtoNetMessage(Socket, netOut1, &playerAddress[1]);
}

// Server message-sending helper method. (private, suggested)
int Server::sendMessage(sockaddr_in& destination, NetworkMessage& message)
{
	// TODO: Send the message in the buffer to the destination.
	sendtoNetMessage(Socket, message, &destination);
	return SUCCESS;
}

// Marks a client as connected and adjusts the game state.
void Server::connectClient(int player, sockaddr_in& source)
{
	playerAddress[player] = source;
	playerTimer[player] = 0;

	noOfPlayers++;
	if (noOfPlayers == 1)
		state.gamePhase = WAITING;
	else
		state.gamePhase = RUNNING;
}

// Marks a client as disconnected and adjusts the game state.
void Server::disconnectClient(int player)
{
	playerAddress[player].sin_addr.s_addr = INADDR_NONE;
	playerAddress[player].sin_port = 0;

	noOfPlayers--;
	if (noOfPlayers == 1)
		state.gamePhase = WAITING;
	else
		state.gamePhase = DISCONNECTED;
}

// Updates the state of the game.
void Server::updateState()
{
	// Tick counter.
	static int timer = 0;

	// Update the tick counter.
	timer++;

	// Next, update the game state.
	if (state.gamePhase == RUNNING)
	{
		// Update the player tick counters (for ALIVE messages.)
		playerTimer[0]++;
		playerTimer[1]++;

		// Update the positions of the player paddles
		if (state.player0.keyUp)
			state.player0.y--;
		if (state.player0.keyDown)
			state.player0.y++;

		if (state.player1.keyUp)
			state.player1.y--;
		if (state.player1.keyDown)
			state.player1.y++;
		
		// Make sure the paddle new positions are within the bounds.
		if (state.player0.y < 0)
			state.player0.y = 0;
		else if (state.player0.y > FIELDY - PADDLEY)
			state.player0.y = FIELDY - PADDLEY;

		if (state.player1.y < 0)
			state.player1.y = 0;
		else if (state.player1.y > FIELDY - PADDLEY)
			state.player1.y = FIELDY - PADDLEY;

		//just in case it get stuck...
		if (ballVecX)
			storedBallVecX = ballVecX;
		else
			ballVecX = storedBallVecX;

		if (ballVecY)
			storedBallVecY = ballVecY;
		else
			ballVecY = storedBallVecY;

		state.ballX += ballVecX;
		state.ballY += ballVecY;

		// Check for paddle collisions & scoring
		if (state.ballX < PADDLEX)
		{
			// If the ball has struck the paddle...
			if (state.ballY + BALLY > state.player0.y && state.ballY < state.player0.y + PADDLEY)
			{
				state.ballX = PADDLEX;
				ballVecX *= -1;
			}
			// Otherwise, the second player has scored.
			else
			{
				newBall();
				state.player1.score++;
				ballVecX *= -1;
			}
		}
		else if (state.ballX >= FIELDX - PADDLEX - BALLX)
		{
			// If the ball has struck the paddle...
			if (state.ballY + BALLY > state.player1.y && state.ballY < state.player1.y + PADDLEY)
			{
				state.ballX = FIELDX - PADDLEX - BALLX - 1;
				ballVecX *= -1;
			}
			// Otherwise, the first player has scored.
			else
			{
				newBall();
				state.player0.score++;
				ballVecX *= -1;
			}
		}

		// Check for Y position "bounce"
		if (state.ballY < 0)
		{
			state.ballY = 0;
			ballVecY *= -1;
		}
		else if (state.ballY >= FIELDY - BALLY)
		{
			state.ballY = FIELDY - BALLY - 1;
			ballVecY *= -1;
		}
	}

	// If the game is over...
	if ((state.player0.score > 10 || state.player1.score > 10) && state.gamePhase == RUNNING)
	{
		state.gamePhase = GAMEOVER;
		timer = 0;
	}
	if (state.gamePhase == GAMEOVER)
	{
		if (timer > 30)
		{
			initState();
			state.gamePhase = RUNNING;
		}
	}
}

// Initializes the state of the game.
void Server::initState()
{
	playerAddress[0].sin_addr.s_addr = INADDR_NONE;
	playerAddress[1].sin_addr.s_addr = INADDR_NONE;
	playerTimer[0] = playerTimer[1] = 0;
	noOfPlayers = 0;

	state.gamePhase = DISCONNECTED;

	state.player0.y = 0;
	state.player1.y = FIELDY - PADDLEY - 1;
	state.player0.score = state.player1.score = 0;
	state.player0.keyUp = state.player0.keyDown = false;
	state.player1.keyUp = state.player1.keyDown = false;

	newBall(); // Get new random ball position

	// Get a new random ball vector that is reasonable
	ballVecX = (rand() % 10) - 5;
	if ((ballVecX >= 0) && (ballVecX < 2))
		ballVecX = 2;
	if ((ballVecX < 0) && (ballVecX > -2))
		ballVecX = -2;

	ballVecY = (rand() % 10) - 5;
	if ((ballVecY >= 0) && (ballVecY < 2))
		ballVecY = 2;
	if ((ballVecY < 0) && (ballVecY > -2))
		ballVecY = -2;


}

// Places the ball randomly within the middle half of the field.
void Server::newBall()
{
	// (randomly in 1/2 playable area) + (1/4 playable area) + (left buffer) + (half ball)
	state.ballX = (rand() % ((FIELDX - 2 * PADDLEX - BALLX) / 2)) +
						((FIELDX - 2 * PADDLEX - BALLX) / 4) + PADDLEX + (BALLX / 2);

	// (randomly in 1/2 playable area) + (1/4 playable area) + (half ball)
	state.ballY = (rand() % ((FIELDY - BALLY) / 2)) + ((FIELDY - BALLY) / 4) + (BALLY / 2);
}
