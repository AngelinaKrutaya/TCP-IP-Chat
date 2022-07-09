#include "Server.h"

void DataThread(Server& s) {
	std::vector<int> indexes_choosen_clients;
	s.clients_num++;
	s.indexes_clients.push_back(s.clients_num);
	std::cout << "Clients count " << s.clients_num << "\n";

	s.RecvClientName();
	s.SendClientsName(s.current_client);

	std::cout << "Waiting client choose...";
	while (true) {
		s.RecvIndexesClientsForSend(indexes_choosen_clients);
		char* msg = s.RecvMessage();
		while (strcmp(msg, "q") != 0) {
			for (auto& i : indexes_choosen_clients) {
				s.SendClientsName(s.connection[i - 1]);
				s.SendMessageWithSize(s.connection[i - 1], msg);
			}
			msg = s.RecvMessage();
			std::cout << "New message: " << msg << "\n";
		}
		delete[] msg;
	}
}

void Server::SendMessageWithSize(SOCKET socket, char* data)
{
	try {
		int data_size = sizeof(data);
		if (SOCKET_ERROR == send(socket, reinterpret_cast<char*>(&data_size), sizeof(int), 0))
			throw "Sending size of message to client";
		if (SOCKET_ERROR == send(socket, data, data_size, 0))
			throw "Sending message to client";
	}
	catch (const char* msg) {
		std::cout << msg << " error:" << WSAGetLastError() << "\n";
	}
}

void Server::SendClientsName(SOCKET socket)
{
	try {
		if (SOCKET_ERROR == send(socket, reinterpret_cast<char*>(&clients_num), sizeof(int), 0))
			throw "Sending number of clients";
		for (int i = 0; i < clients_num; ++i)
			SendMessageWithSize(socket, clients_name[i]);
	}
	catch (const char* msg) {
		std::cout << msg << " error:" << WSAGetLastError() << "\n";
	}
}

char* Server::RecvMessage()
{
	try {
		int msg_size = 0;
		if (SOCKET_ERROR == recv(current_client, reinterpret_cast<char*>(&msg_size), sizeof(int), 0))
			throw "Receiving message";
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		if (SOCKET_ERROR == recv(current_client, msg, msg_size, 0))
			throw "Receiving message";
		return msg;
	}
	catch (const char* msg) {
		std::cout << msg << " error:" << WSAGetLastError() << "\n";
	}
}

void Server::RecvClientName()
{
	char* name = RecvMessage();
	clients_name.push_back(name);
	std::cout << " Connected client with name: " << name << "\n";
}

void Server::RecvIndexesClientsForSend(std::vector<int>& indexes_choosen_clients)
{
	try {
		int index_choosen_client = 0;
		if (SOCKET_ERROR == recv(current_client, reinterpret_cast<char*>(&index_choosen_client), sizeof(int), 0))\
			throw "Receivig index choosen client";
		if (index_choosen_client != 0) {
			if (std::find(indexes_choosen_clients.begin(), indexes_choosen_clients.end(), index_choosen_client) == indexes_choosen_clients.end())
				indexes_choosen_clients.push_back(index_choosen_client);
		}
		else indexes_choosen_clients = indexes_clients;
	}
	catch (const char* msg) {
		std::cout << msg << " error:" << WSAGetLastError() << "\n";
	}
}

Server::Server(const char* addr, int port) {
	WSADATA ws;
	if (SOCKET_ERROR == WSAStartup(MAKEWORD(1, 1), &ws)) {
		std::cout << WSAGetLastError();
	}
	else {
		ZeroMemory(&name_, sizeof(name_));
		name_.sin_family = AF_INET;
		name_.sin_addr.S_un.S_addr = inet_addr(addr);
		name_.sin_port = htons(port);
	}
}

Server::~Server()
{
	for (auto& name : clients_name)
		delete[] name;

	for (auto& client : connection)
		closesocket(client);

	for (auto& th : threads)
		th.join();

	closesocket(server_);
	WSACleanup();
}

void Server::Start() {
	try {
		if (SOCKET_ERROR == (server_ = socket(AF_INET, SOCK_STREAM, 0)))
			throw "Creating server socket";

		if (SOCKET_ERROR == bind(server_, reinterpret_cast<sockaddr*>(&name_), sizeof(name_)))
			throw "Binding server";

		if (SOCKET_ERROR == listen(server_, client_count))
			throw "Listening server";

		Client client;
		while ((current_client = accept(server_, reinterpret_cast<sockaddr*>(&client.addr_), &client.addrlen_))) {
			std::cout << "Client connected" << "\n";
			connection.push_back(current_client);
			threads.push_back(std::thread(DataThread, std::ref(*this)));
		}
		current_client = -1;
	}
	catch (const char* msg)
	{
		std::cout << msg << " error:" << WSAGetLastError() << "\n";
	}
}



