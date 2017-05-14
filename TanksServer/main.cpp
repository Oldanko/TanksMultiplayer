#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<stdio.h>
#include<winsock2.h>

#include <iostream>

#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

class SocketManager
{
	SOCKET s;
	struct sockaddr_in address;
	int slen, recv_len;
	char buf[BUFLEN];
public:
	SocketManager()
	{
		slen = sizeof(address);

		//Create a socket
		if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
		{
			printf("Could not create socket : %d", WSAGetLastError());
		}
		printf("Socket created.\n");

		//Prepare the sockaddr_in structure
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(PORT);

		//Bind
		if (bind(s, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR)
		{
			printf("Bind failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		puts("Bind done");

	}

	SocketManager(sockaddr_in other)
	{
		address = other;
		slen = sizeof(address);
		//Create a socket
		if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
		{
			printf("Could not create socket : %d", WSAGetLastError());
		}
		printf("Socket created.\n");

	}

	~SocketManager()
	{
		closesocket(s);
		WSACleanup();
	}

	void connect()
	{
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &address, &slen)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		/*
		Check if a client wants to connect
		Check if the client can connect
		Create a new socket for the client
		Ask the socket to send a connection request to the client
		Spawn a new thread for the socket
		*/
	}

	void receive()
	{
		printf("Waiting for data...");
		fflush(stdout);

		memset(buf, '\0', BUFLEN);

		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &address, &slen)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
	}

	void send()
	{
		//now reply the client with the same data
		if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &address, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
	}
};

int main()
{
	WSADATA wsa;

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");
	

	SocketManager udpman;

	//keep listening for data
	while (1)
	{	
		udpman.receive();
		udpman.send();
	}

	return 0;
}