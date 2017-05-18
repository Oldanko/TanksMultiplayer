#pragma once

#define _CRT_SECURE_NO_WARNINGS

#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

#include<stdio.h>
#include<winsock2.h>
#include <iostream>
#include <vector>

#include <thread>
#include <mutex>


class SocketManager
{
	static uint8_t indexer;
	uint8_t m_index;

	SOCKET s;
	struct sockaddr_in address;
	int slen, recv_len;

	uint32_t dataSize;
	uint32_t bufPointer;

	std::thread th;
	std::mutex mtx_buf;
	std::mutex mtx_msg;
	std::mutex mtx_stat;

	char buf[BUFLEN];
	char msg[BUFLEN];

	uint8_t status;

	void manageConnections_loop();
	void serveClient_loop();
public:
	static std::vector<SocketManager*> sockets;
	static std::mutex mtx_sockets;
	
	SocketManager();
	SocketManager(sockaddr_in other);
	~SocketManager();
	
	uint8_t index();

	void manageConnections();
	void serveClient();

	void send();
	void receive();

	void receive_request();
	void send_request();

	bool isSent();
	bool isReceived();

	void buffer(uint8_t data);
	void buffer(float data);
	void buffer(const char* data);

	void read(uint8_t& data);
	void read(float& data);

	void debug_print();
	void printMyPort();

	void close();
};

