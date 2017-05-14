#include "UDPClient.h"



UDPClient::UDPClient(const char* SERVER, short PORT)
{
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

void UDPClient::send()
{
	if (sendto(s, msg, dataSize, 0, (struct sockaddr *) &si_server, slen) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		//exit(EXIT_FAILURE);
	}
	dataSize = 0;

}

void UDPClient::receive()
{
	memset(buf, '\0', BUFLEN);
	//try to receive some data, this is a blocking call

	if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_server, &slen) == SOCKET_ERROR)
	{
		printf("recvfrom() failed with error code : %d", WSAGetLastError());
		//exit(EXIT_FAILURE);
	}

	puts(buf);
}

void UDPClient::push(float f)
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
