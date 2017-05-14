#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>
#include <iostream>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 512

class UDPClient
{
	SOCKET s;
	struct sockaddr_in si_server;
	int slen = sizeof(si_server);
	WSADATA wsa;

	uint32_t dataSize;

	char buf[BUFLEN];
	char msg[BUFLEN];
public:
	UDPClient(const char* SERVER, short PORT);
	~UDPClient();
	bool connect();
	void send();
	void receive();
	void push(float f);
	//void setMsg(const char* message);
};