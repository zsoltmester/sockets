#define PORT 42002

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

int main(int argc, char **argv)
{
    int client = socket(AF_INET,SOCK_STREAM,0);
	if(client == -1) {
  		cerr << "Opening stream socket failed. ERROR:" << errno << endl;
  		return -1;
	}
	int sockError = -1;

	struct sockaddr_in server_name;
	server_name.sin_family = AF_INET;
	server_name.sin_addr.s_addr=INADDR_ANY;
	server_name.sin_port = htons(PORT);
	memset(&(server_name.sin_zero),'\0',8);

	if (connect(client, (struct sockaddr*) &server_name, sizeof server_name) < 0)
	{
		cerr << "Error - connection cannot be established..." << endl;
		close(client);
		return -2;
	}

	char buffer[1024];
	int nbytes;

	nbytes = recv(client, buffer, sizeof buffer, 0);

	if(nbytes<0){
		cout << "The recv call failed with error: " << errno << endl;
        close(client);
	}

	buffer[nbytes] = '\0';

	cout << "Incoming message: " << buffer << endl;

	string msg = "www.elte.hu";

	send(client, msg.c_str(), msg.length(), 0);

	cout << "Message sent: " << msg << endl;

	nbytes = recv(client, buffer, sizeof buffer, 0);

	if(nbytes<0){
		cout << "The recv call failed with error: " << errno << endl;
        close(client);
	}

	buffer[nbytes] = '\0';

	cout << "Incoming message-2: " << buffer << endl;

	close(client);
	cout << " << Client is now terminating." << endl;

	return 0;
}
