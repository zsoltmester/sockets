#define _WIN32_WINNT 0x501
#define PORT 42002
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <sstream>
using namespace std;

int main(int argc, char **argv)
{
	WSADATA wsaData;
	int iResult;
	/// windows socket inicializalasa
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}
	else {
		cout << " >> Winsock DLL is ready to use." << endl;
	}
	DWORD dwError;
	SOCKET client = socket(AF_INET, SOCK_STREAM, 0);
	if (client == INVALID_SOCKET) {
		cerr << "Opening stream socket failed. ERROR:" << WSAGetLastError() << endl;
		return -1;
	}

	struct sockaddr_in server_name;
	server_name.sin_family = AF_INET;
	server_name.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_name.sin_port = htons(PORT);
	memset(&(server_name.sin_zero), '\0', 8);

	if (connect(client, (struct sockaddr*) &server_name, sizeof server_name) < 0)
	{
		cerr << "Error - connection cannot be established..." << endl;
		closesocket(client);
		return -2;
	}

	char buffer[1024];
	int nbytes;

	nbytes = recv(client, buffer, sizeof buffer, 0);

	if(nbytes<0){
		cout << "The recv call failed with error: " << WSAGetLastError() << endl;
        closesocket(call);
	}

	buffer[nbytes] = '\0';

	cout << "Incoming message: " << buffer << endl;

	string msg = "www.elte.hu";

	send(client, msg.c_str(), msg.length(), 0);

	cout << "Message sent: " << msg << endl;

	nbytes = recv(client, buffer, sizeof buffer, 0);

	if(nbytes<0){
		cout << "The recv call failed with error: " << WSAGetLastError() << endl;
        closesocket(call);
	}

	buffer[nbytes] = '\0';

	cout << "Incoming message-2: " << buffer << endl;

	closesocket(client);
	if (WSACleanup() != 0) {
		dwError = WSAGetLastError();
		if (dwError == WSANOTINITIALISED) {
			cout << "A successful WSAStartup call must occur before using this function." << endl;
			return 1;
		}
		else if (dwError == WSAENETDOWN) {
			cout << "The network subsystem has failed." << endl;
			return 1;
		}
		else {
			cout << "Function failed with error: " << dwError << endl;
			return 1;
		}
	}
	cout << " << Winsock DLL is closed." << endl;
	return 0;
}
