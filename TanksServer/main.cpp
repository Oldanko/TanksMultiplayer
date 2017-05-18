#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<stdio.h>
#include<winsock2.h>

#include <iostream>
#include <vector>

#include <thread>
#include <mutex>

#include <chrono>

#include <map>

#include "SocketManager.h"

struct Transform
{
	float m_x;
	float m_y;
	float m_a;
	Transform() : m_x(0), m_y(0), m_a(0) {};
	Transform(float x, float y, float a) : m_x(x), m_y(y), m_a(a) {};
	void print() { printf("%f, %f, %f;\n", m_x, m_y, m_a); }
	void prepareNetData(SocketManager & s)
	{
		s.buffer(m_x);
		s.buffer(m_y);
		s.buffer(m_a);
	}
	void readNetData(SocketManager & s)
	{
		s.read(m_x);
		s.read(m_y);
		s.read(m_a);
	}
};

std::map<uint8_t, Transform> netObjects;

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
	
	bool alive = true;
	std::mutex mtx_alive;
	
	std::thread console_thread([&alive, &mtx_alive]()
	{ 
		std::cin.ignore();
		mtx_alive.lock();
		alive = false;
		mtx_alive.unlock();
		return;
	});
	console_thread.detach();

	SocketManager mainSocket;
	mainSocket.manageConnections();

	std::chrono::high_resolution_clock::time_point time_start;
	std::chrono::high_resolution_clock::time_point time_now = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double, std::milli> time_span;

	mtx_alive.lock();

	while (alive)
	{
		mtx_alive.unlock();
	
		time_start = time_now;

		SocketManager::mtx_sockets.lock();

		for (auto s : SocketManager::sockets)
		{
			if (s->isReceived())
			{
				if (netObjects.find(s->index()) == netObjects.end())
					netObjects.emplace(s->index(), Transform());

				netObjects[s->index()].readNetData(*s);
				s->receive_request();
			}
		}

		for (auto s : SocketManager::sockets)
		{
			if (s->isSent())
			{
				//s->buffer((uint8_t)(netObjects.size() - 1));
				s->buffer((uint8_t)(netObjects.size()));
				
				for (auto n : netObjects)
				{
					//if (n.first == s->index())
						//continue;
					s->buffer(n.first);
					n.second.prepareNetData(*s);
				}
				s->send_request();
			}
		}

		

		//auto time_start = std::chrono::high_resolution_clock::now();
		
		

		//auto time_finish = std::chrono::high_resolution_clock::now();
		//printf("%f\n", std::chrono::duration<double, std::milli>(time_finish - time_start).count());

		SocketManager::mtx_sockets.unlock();

		Sleep(((1 / 61.0) * 500 - std::chrono::duration<double, std::milli>(time_now - time_start).count()));
		/*uint64_t times = 0;
		do
		{
			++times;
			time_now = std::chrono::high_resolution_clock::now();
		}
		while (std::chrono::duration<double, std::milli>(time_now - time_start).count() < (1 / 61.0));*/


		mtx_alive.lock();
	}
	mtx_alive.unlock();

	mainSocket.close();

	for (auto s : SocketManager::sockets)
		delete s;

	WSACleanup();
	return 0;
}