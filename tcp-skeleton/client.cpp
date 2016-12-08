#define PORT 42002
#define SO_ERROR -1

#include <stdio.h>
#include <iostream>
#include <sstream>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

using namespace std;

void communicateWithServer(int client) {

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

	// terminating the client
	cout << "Client is now terminating... ";
	close(client);
	cout << "DONE" << endl;

	return 0;
}
