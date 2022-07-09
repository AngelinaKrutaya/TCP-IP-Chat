#pragma once
#pragma warning(disable:4996) 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#pragma comment(lib,"ws2_32")
#include <iostream>
#include <vector>
#include <thread>


class Server {
public:
	class Client {
	public:
		Client() {
			ZeroMemory(&addr_, sizeof(addrlen_));
		}

		SOCKET client_;
		sockaddr_in addr_;
		int addrlen_ = sizeof(addr_);
	};

	Server(const char* address, int port);
	~Server();
	void Start();
	void SendMessageWithSize(SOCKET socket, char* data);
	void SendClientsName(SOCKET socket);
	char* RecvMessage();
	void RecvClientName();
	void RecvIndexesClientsForSend(std::vector<int>& indexes_choosen_clients);

	std::vector<SOCKET> connection;
	std::vector<int> indexes_clients;
	int clients_num = 0;
	SOCKET current_client = -1;
private:
	SOCKET server_ = -1;
	sockaddr_in name_;
	std::vector<std::thread> threads;
	std::vector<char*> clients_name;
	const int client_count = 10;
};