#pragma once
#pragma comment(lib,"ws2_32")
#pragma warning(disable:4996) 
#include <winsock2.h>
#include <iostream>
#include <vector>
#include <thread>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

class Client {
public:
	Client(const char* address,int port);
	~Client();
	void Connect();
	void SendData();
	void SetClientName();
	void RecvClients();
	char* RecvMessage();
	void Menu() const;
	void PrintClients() const;

private:
	SOCKET connection;
	int clients_num;
	std::vector<char*> clients_name;
	int number_client = 0;
	sockaddr_in addr_;
};