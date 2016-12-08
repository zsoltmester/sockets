#define PORT 42002
#define SO_ERROR -1

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <time.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

using namespace std;

void communicateWithServer(int client) {
	srand(time(NULL));
	bool running = true;
	while (running) {
		int number = rand() % 100 + 1;
		int opIndex = rand() % 3;
		char op;
		if (opIndex == 0) op = '<';
		else if (opIndex == 1) op = '=';
		else if (opIndex == 2) op = '>';

		std::ostringstream oss;
		oss << op << number;
		string message = oss.str();
		cout << "Sending message: " << message << endl;
		send(client, message.c_str(), message.length(), 0);

		// TODO
		break;
	}
}

int main(int argc, char **argv)
{
	// create the socket file descriptor
    int client = socket(AF_INET, SOCK_STREAM, 0);
	if(client == SO_ERROR) {
  		cerr << "Opening stream socket failed. ERROR:" << errno << endl;
  		return 1;
	}

	// server address
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(PORT);
	memset(&(serverAddr.sin_zero), '\0', 8);

	// connect to the server
	if (connect(client, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0)
	{
		cerr << "Connection cannot be established..." << endl;
		close(client);
		return 2;
	}

	communicateWithServer(client);

	// terminating the client
	cout << "Client is now terminating... ";
	close(client);
	cout << "DONE" << endl;

	return 0;
}
