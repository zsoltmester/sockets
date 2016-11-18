#define PORT 42002
#define SO_ERROR -1
#define REQ_MAX_LENGTH 200

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

const string REQ_MAX_LENGTH_MSG = "Too long peer request. Maximum length is 200 bytes.";

struct ReqMsg
{
	int32_t a;
	int32_t b;
	char op;
};

struct ReplyMsg
{
	int32_t result;
};

int main(int argc, char **argv)
{
	// create the socket file descriptor
    int server = socket(AF_INET, SOCK_STREAM, 0);
	if(server == SO_ERROR)
	{
  		cerr << "Opening stream socket failed. ERROR:" << errno << endl;
  		return 1;
	}

	// set to reuse the port
    int reuseAddrValue = 1;
	int result = setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (char *) &reuseAddrValue, sizeof(reuseAddrValue));
    if (result == SO_ERROR) cerr << "The call of setsockopt failed with error:" << errno << endl;
    else cerr << "\t\tSet SO_REUSEADDR: ON" << endl;

	// server address
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(PORT);
	memset(&(serverAddr.sin_zero), '\0', 8);

	// bind the socket to the specified port
	if(bind(server, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == SO_ERROR )
	{
    	cerr << "Binding stream socket failed." << endl;
    	return 2;
	}

    // print out server info
    struct sockaddr_in serverInfo;
    socklen_t serverInfoLength = sizeof(serverInfo);
	result = getsockname( server, (struct sockaddr *) &serverInfo, &serverInfoLength);
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

	while (true)
	{
		// accept a client
		struct sockaddr_in caller;
		socklen_t addrlen = sizeof(caller);
		int call = accept(server,(struct sockaddr *) &caller, &addrlen);

		// wait for a request and send the sesponse
		char* puffer = new char[REQ_MAX_LENGTH];
		memset(puffer, '\0', REQ_MAX_LENGTH);
		int receivedBytes = recv(call, puffer, REQ_MAX_LENGTH, 0);
		if(receivedBytes == 0) cout << "Connection closed by peer." << endl;
		else if(receivedBytes<  0) cout << "The recv call failed with error: " << errno << endl;
		else if (receivedBytes < REQ_MAX_LENGTH)
		{
			// parse the request
			struct ReqMsg req;
			memcpy(&req, puffer, sizeof(struct ReqMsg));
			req.a = ntohs(req.a);
			req.b = ntohs(req.b);
			cout << "Received: " << req.a << req.op << req.b << endl;

			// process the request
			struct ReplyMsg resp;
			resp.result = req.op == '+' ? req.a + req.b : req.a * req.b;
			cout << "Result: " << resp.result << endl;

			// send the response
			resp.result = htons(resp.result);
			int sentBytes = send(call,&resp,sizeof(resp),0);
			if(sentBytes == SO_ERROR) cerr << "Failed the send the reply to the client: " << errno << endl;
			else cout << "Result has been posted to client." << endl;
		}
		else
		{
			cerr << "Too long request." << endl;
			int sentBytes = send(call,REQ_MAX_LENGTH_MSG.c_str(),strlen(REQ_MAX_LENGTH_MSG.c_str()),0);
			if(sentBytes == SO_ERROR) cerr << "Info has not been send to client, because an error occured: "
				<< errno << endl;
			else cout << "Info has been posted to client." << endl;
		}
		delete puffer;

		close(call);
	}

	// terminating the server
	cout << "Server is now terminating... ";
	close(server);
	cout << "DONE" << endl;
}
