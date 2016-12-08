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

bool handleClientRequest(int client) {
	char* buff = new char[200];
	memset(buff, '\0', 200);
	int receivedBytes = recv(client, buff, 200, 0);
	if (receivedBytes == 0) {
		cout << "Connection closed by peer." << endl;
		close(client);
		return false;
	} else if (receivedBytes < 0) {
		cout << "The recv call failed with error: " << errno << endl;
		return false;
	}

	char op = buff[0];
	char subbuff[receivedBytes];
	memcpy(subbuff, &buff[1], receivedBytes);
	subbuff[receivedBytes] = '\0';

	cout << "Received from client: " << op << subbuff << endl;
}

int main(int argc, char **argv)
{
	// create the socket file descriptor
    int server = socket(AF_INET, SOCK_STREAM, 0);
	if (server == SO_ERROR)
	{
  		cerr << "Opening stream socket failed. ERROR:" << errno << endl;
  		return 1;
	}

	// set to reuse the port
    int reuseAddrValue = 1;
	int result = setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (char *) &reuseAddrValue, sizeof(reuseAddrValue));
    if (result == SO_ERROR) cerr << "The call of setsockopt failed with error:" << errno << endl;
    else cerr << "Set SO_REUSEADDR: ON" << endl;

	// server address
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(PORT);
	memset(&(serverAddr.sin_zero), '\0', 8);

	// bind the socket to the specified port
	if (bind(server, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == SO_ERROR)
	{
    	cerr << "Binding stream socket failed." << endl;
    	return 2;
	}

    // print out server info
    struct sockaddr_in serverInfo;
    socklen_t serverInfoLength = sizeof(serverInfo);
	result = getsockname(server, (struct sockaddr *) &serverInfo, &serverInfoLength);
    if(result == SO_ERROR)
	{
        cerr << "Getting socket failed." << endl;
    	return 3;
    }
    cout << "------------------------------------------------------" << endl;
    cout << "--> IPv4 Address is " << inet_ntoa(serverInfo.sin_addr) << endl;
    cout << "--> Port number is " << ntohs(serverInfo.sin_port) << endl;
    cout << "------------------------------------------------------" << endl;

	// waiting for clients to connect
	if(listen(server, 4) == SO_ERROR)
	{
		cerr << "Listening failed to start." << endl;
		return 4;
	}
	cout << "Server is ready to accept connections..." << endl;

	// set up the socket descriptors
	fd_set master;
	fd_set readfds; // temp
	int maxfd = server; // current max socket descriptor, so far, it's this one
	FD_ZERO(&master); // clear the master socket descriptor set
	FD_SET(server, &master); // add the server socket descriptor to the master sd set

	srand(time(NULL));
	int random = rand() % 100 + 1;

	while (true)
	{
		readfds = master;
		select(maxfd + 1, &readfds, 0, 0, 0);
		for (int i = 0; i < maxfd + 1; ++i) {
			if (!FD_ISSET(i, &readfds)) {
				continue;
			}

			if (i == server) {
				struct sockaddr_in caller;
				socklen_t addrlen = sizeof(caller);
				int call = accept(server, (struct sockaddr *) &caller, &addrlen);
				cout << "A client joined..." << endl;
				FD_SET(call, &master);
				if (maxfd < call) maxfd = call;
			} else {
				bool guessed = handleClientRequest(i);
				// TODO
			}
		}
	}

	// terminating the server
	cout << "Server is now terminating... ";
	close(server);
	cout << "DONE" << endl;
}
