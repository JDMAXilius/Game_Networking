#include "../platform.h"
#include "../definitions.h"

class Client
{
private:
	byte player;
	SOCKET Socket;
	volatile bool active;//controls the activity of the clients run which is on its own thread.

	GameState state;
	CriticalSection cs;//just incase you need to protect data, you might not need to.
	short sh1;
	int count = 0;
	sockaddr_in addSock;


public:
	inline Client() { state.gamePhase = WAITING; }

	int init(char* address, unsigned short port, byte player);
	int run();
	void stop();

	int sendInput(byte keyUp, byte keyDown, byte keyQuit);
	void getState(GameState* target);

private:
	int sendAlive();
	void sendClose();
};
