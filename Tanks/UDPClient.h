#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

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
	uint32_t bufPointer;

	char buf[BUFLEN];
	char msg[BUFLEN];
public:
	UDPClient(const char* SERVER, short PORT);
	~UDPClient();
	bool connect();
	void send();
	void receive();
	void buffer(float f);
	void buffer(const char* data);
	void read(uint8_t &data);
	void read(float &data);
	//void setMsg(const char* message);
};