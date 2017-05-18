#include "SocketManager.h"


uint8_t ALIVE = 0b00000001;
uint8_t SEND_REQ = 0b00000010;
uint8_t RECEIVE_REQ = 0b00000100;

std::vector<SocketManager*> SocketManager::sockets;
std::mutex SocketManager::mtx_sockets;

uint8_t SocketManager::indexer = 0;

SocketManager::SocketManager() : status(false)
{
	slen = sizeof(address);
	dataSize = 0;

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
		printf("Could not create socket : %d", WSAGetLastError());

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

SocketManager::SocketManager(sockaddr_in other) : status(false)
{
	dataSize = 0;
	slen = sizeof(address);
	address = other;

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
		printf("Could not create socket : %d\n", WSAGetLastError());
	
	printf("Socket created.\n");

	buffer("hello");
	send();

	receive();
	printf(buf);
	printf("\n");

	m_index = indexer++;

	buffer(m_index);
	send();

	struct sockaddr_in sin;
	int len = sizeof(sin);
	getsockname(s, (struct sockaddr *)&sin, &len);
	printf("%d is receiving info from %d\n", ntohs(sin.sin_port), ntohs(address.sin_port));
}

SocketManager::~SocketManager()
{
	close();
	th.join();
}

uint8_t SocketManager::index()
{
	return m_index;
}

void SocketManager::manageConnections_loop()
{
	printf("Managing loop started\n");
	mtx_stat.lock();
	while (status & ALIVE)
	{
		mtx_stat.unlock();
		
		receive();

		if (strcmp("hello", buf) == 0)
		{
			mtx_sockets.lock();
			sockets.push_back(new SocketManager(address));
			sockets.back()->serveClient();
			mtx_sockets.unlock();
		}

		mtx_stat.lock();
	}
	mtx_stat.unlock();
	printf("Managing loop is dead\n");
}

void SocketManager::serveClient_loop()
{

	printf("Serving loop online!\n");
	mtx_stat.lock();
	while (status & ALIVE)
	{	
		auto stat_temp = status;
		mtx_stat.unlock();

		if (stat_temp & RECEIVE_REQ)
		{
			receive();
			mtx_stat.lock();
			status &= ~RECEIVE_REQ;
			mtx_stat.unlock();
		}

		if (stat_temp & SEND_REQ)
		{
			send();
			mtx_stat.lock();
			status &= ~SEND_REQ;
			mtx_stat.unlock();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
		mtx_stat.lock();
	}
	mtx_stat.unlock();
	printf("I'm dead now\n");
}

void SocketManager::manageConnections()
{
	status = ALIVE;
	th = std::thread(&SocketManager::manageConnections_loop, this);

}

void SocketManager::serveClient()
{
	status = ALIVE;
	th = std::thread(&SocketManager::serveClient_loop, this);
}

void SocketManager::send()
{
	mtx_msg.lock();
	if (sendto(s, msg, dataSize, 0, (struct sockaddr*) &address, slen) == SOCKET_ERROR)
	{
		//printf("sendto() failed with error code : %d\n", WSAGetLastError());
	}
	dataSize = 0;
	mtx_msg.unlock();
}

void SocketManager::receive()
{
	//printf("Waiting for data...\n");
	fflush(stdout);

	mtx_buf.lock();
	memset(buf, '\0', BUFLEN);
	if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &address, &slen)) == SOCKET_ERROR)
	{
		//printf("recvfrom() failed with error code : %d\n", WSAGetLastError());
	}
	bufPointer = 0;
	mtx_buf.unlock();
}

void SocketManager::receive_request()
{
	mtx_stat.lock();
	status |= RECEIVE_REQ;
	mtx_stat.unlock();
}

void SocketManager::send_request()
{
	mtx_stat.lock();
	status |= SEND_REQ;
	mtx_stat.unlock();
}

bool SocketManager::isSent()
{
	mtx_stat.lock();
	uint8_t r = !(status & SEND_REQ);
	mtx_stat.unlock();
	return r;
}

bool SocketManager::isReceived()
{
	mtx_stat.lock();
	uint8_t r = !(status & RECEIVE_REQ);
	mtx_stat.unlock();
	return r;
}



void SocketManager::buffer(uint8_t data)
{
	uint32_t newDataSize = dataSize + sizeof(uint8_t);
	if (newDataSize > BUFLEN)
	{
		printf("TOO MUCH DATA!");
		return;
	}
	mtx_msg.lock();
	memcpy(&msg[dataSize], &data, sizeof(uint8_t));
	mtx_msg.unlock();
	dataSize = newDataSize;
}

void SocketManager::buffer(float data)
{
	uint32_t newDataSize = dataSize + sizeof(float);
	if (newDataSize > BUFLEN)
	{
		printf("TOO MUCH DATA!");
		return;
	}
	mtx_msg.lock();
	memcpy(&msg[dataSize], &data, sizeof(float));
	mtx_msg.unlock();
	dataSize = newDataSize;
}

void SocketManager::buffer(const char * data)
{
	uint32_t newDataSize = dataSize + strlen(data);
	if (newDataSize > BUFLEN)
	{
		printf("TOO MUCH DATA!");
		return;
	}
	mtx_msg.lock();
	strcpy(&msg[dataSize], data);
	mtx_msg.unlock();
	dataSize = newDataSize;
}

void SocketManager::read(uint8_t & data)
{
	uint32_t newBufPointer = bufPointer + sizeof(uint8_t);
	if (newBufPointer > BUFLEN)
	{
		printf("Pointer out of bounds!");
		return;
	}
	mtx_buf.lock();
	memcpy(&data, &buf[bufPointer], sizeof(uint8_t));
	mtx_buf.unlock();
	bufPointer = newBufPointer;
}

void SocketManager::read(float & data)
{
	uint32_t newBufPointer = bufPointer + sizeof(float);
	if (newBufPointer > BUFLEN)
	{
		printf("Pointer out of bounds!");
		return;
	}
	mtx_buf.lock();
	memcpy( &data, &buf[bufPointer], sizeof(float));
	mtx_buf.unlock();
	bufPointer = newBufPointer;
}

void SocketManager::debug_print()
{
	mtx_buf.lock();
	printf("%f, %f, %f\n", ((float*)buf)[0], ((float*)buf)[1], ((float*)buf)[2]);
	//printf(buf);
	mtx_buf.unlock();
}

void SocketManager::printMyPort()
{
	struct sockaddr_in sin;
	int len = sizeof(sin);
	if (getsockname(s, (struct sockaddr *)&sin, &len) == -1)
		perror("getsockname");
	else
		printf("port number %d\n", ntohs(sin.sin_port));
}

void SocketManager::close()
{
	mtx_stat.lock();
	status = false;
	mtx_stat.unlock();
	closesocket(s);
}
