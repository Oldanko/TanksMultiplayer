#include "UDPClient.h"



UDPClient::UDPClient(const char* SERVER, short PORT)
{
	dataSize = 0;
	slen = sizeof(si_server);
	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//create socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	//setup address structure
	memset((char *)&si_server, 0, sizeof(si_server));
	si_server.sin_family = AF_INET;
	si_server.sin_port = htons(PORT);
	si_server.sin_addr.S_un.S_addr = inet_addr(SERVER);

}

UDPClient::~UDPClient()
{
	closesocket(s);
	WSACleanup();
}

uint8_t UDPClient::connect()
{
	printf("Actual server is here: %s:%d\n", inet_ntoa(si_server.sin_addr), ntohs(si_server.sin_port));
	buffer("hello");
	send();

	receive();
	printf(buf);
	printf("\n");

	buffer("AreYouMyDaddy?");
	send();

	receive();
	uint8_t id;
	read(id);
	printf("\n");

	printf("Actual server is here: %s:%d\n", inet_ntoa(si_server.sin_addr), ntohs(si_server.sin_port));

	u_long argp = 1;
	//ioctlsocket(s, FIONBIO, &argp);

	return id;
}

void UDPClient::send()
{
	if (sendto(s, msg, dataSize, 0, (struct sockaddr *) &si_server, slen) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
	}
	dataSize = 0;

}

void UDPClient::receive()
{
	memset(buf, '\0', BUFLEN);

	if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_server, &slen) == SOCKET_ERROR)
	{
		printf("recvfrom() failed with error code : %d", WSAGetLastError());
	}
	bufPointer = 0;
}

void UDPClient::buffer(float f)
{
	uint32_t newDataSize = dataSize + sizeof(float);
	if (newDataSize > BUFLEN)
	{
		printf("TOO MUCH DATA!!!");
		return;
	}
	memcpy(&msg[dataSize], &f, sizeof(float));
	dataSize = newDataSize;
}

void UDPClient::buffer(const char * data)
{
	uint32_t newDataSize = dataSize + strlen(data);
	
	if (newDataSize > BUFLEN)
	{
		printf("TOO MUCH DATA!");
		return;
	}
	strcpy(&msg[dataSize], data);
	dataSize = newDataSize;
}

void UDPClient::read(uint8_t & data)
{
	uint32_t newBufPointer = bufPointer + sizeof(uint8_t);

	if (newBufPointer > BUFLEN)
	{
		printf("Pointer went too far!");
		return;
	}
	memcpy(&data, &buf[bufPointer], sizeof(uint8_t));
	bufPointer = newBufPointer;
}

void UDPClient::read(float & data)
{
	uint32_t newBufPointer = bufPointer + sizeof(float);

	if (newBufPointer > BUFLEN)
	{
		printf("Pointer went too far!");
		return;
	}
	memcpy(&data, &buf[bufPointer], sizeof(float));
	bufPointer = newBufPointer;
}
