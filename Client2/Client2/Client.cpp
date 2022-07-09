#include "Client.h"

void DataThread(Client& c) {
	while (true) {
		c.RecvClients();
		char* msg = c.RecvMessage();
		std::cout << "Get message: " << msg << "\n";
		delete[] msg;
	}
}

Client::Client(const char* address, int port) {
	WSADATA ws;
	if (SOCKET_ERROR == WSAStartup(MAKEWORD(1, 1), &ws)) {
		std::cout << "WSAStartup error: " << WSAGetLastError << "\n";
	}
	ZeroMemory(&addr_, sizeof(addr_));
	addr_.sin_family = AF_INET;
	addr_.sin_addr.S_un.S_addr = inet_addr(address);
	addr_.sin_port = htons(port);
}

Client::~Client()
{
	for (auto& name : clients_name)
		delete[] name;
	closesocket(connection);
	WSACleanup();
}

void Client::Menu() const
{
	std::cout << "Choose: " << "\n";
	std::cout << "1.Open chat" << "\n";
	std::cout << "2.Open group chat" << "\n";
	std::cout << "3. Exit" << "\n";
}

void Client::PrintClients() const
{
	std::cout << "Choose clients fo chat:" << "\n";
	for (int i = 0; i < clients_num; i++) {
		std::cout << i + 1 << " " << clients_name[i] << "\n";
	}
}

void Client::Connect() {
	try {
		if (SOCKET_ERROR == (connection = socket(AF_INET, SOCK_STREAM, 0)))
			throw "Creating client socket";
		if (SOCKET_ERROR == connect(connection, reinterpret_cast<sockaddr*>(&addr_), sizeof(addr_)))
			throw "Connecting";

		SetClientName();
		RecvClients();
		std::thread th(DataThread, std::ref(*this));

		int choose;
		while (true) {
			number_client = 0;
			Menu();
			std::cin >> choose;

			switch (choose) {
			case 1:
				PrintClients();
				std::cin >> number_client;
				while (number_client > clients_num || number_client == 0) {
					std::cout << "Enter correct number:" << "\n";
					std::cin >> number_client;
				}
				SendData();
				break;
			case 2:
				SendData();
				break;
			case 3:
				th.join();
				exit(0);
				break;
			default:
				break;
			}
		}
		th.join();
	}
	catch (const char* msg) {
		std::cout << msg << " error:" << WSAGetLastError() << "\n";
	}
}


void Client::SendData() {
	try {
		if (SOCKET_ERROR == send(connection, reinterpret_cast<char*>(&number_client), sizeof(int), 0))
			throw "Sending number client";
		std::string buf;
		std::cout << "Enter q for quit. Enter message:" << "\n";
		while (buf != "q") {
			std::cin >> buf;
			int msg_size = buf.size();
			if (SOCKET_ERROR == send(connection, reinterpret_cast<char*>(&msg_size), sizeof(int), 0))
				throw "Sending message";
			if (SOCKET_ERROR == send(connection, buf.c_str(), msg_size, 0))
				throw "Sending message";
			std::cout << "Message sended" << "\n";
		}
	}
	catch (const char* msg) {
		std::cout << msg << " error:" << WSAGetLastError() << "\n";
	}
}

void Client::SetClientName()
{
	try {
		std::string client_name;
		std::cout << "Enter your name:" << "\n";
		std::cin >> client_name;
		int name_size = client_name.size();
		if (SOCKET_ERROR == send(connection, reinterpret_cast<char*>(&name_size), sizeof(int), 0))
			throw "Sending client name";
		if (SOCKET_ERROR == send(connection, client_name.c_str(), name_size, 0))
			throw "Sending client name";
		std::cout << "Your name " << client_name << "\n";
	}
	catch (const char* msg) {
		std::cout << msg << " error:" << WSAGetLastError() << "\n";
	}
}

char* Client::RecvMessage(){
	try {
		int msg_size = 0;
		if (SOCKET_ERROR == recv(connection, reinterpret_cast<char*>(&msg_size), sizeof(int), 0))
			throw "Recieving message";
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		if (SOCKET_ERROR == recv(connection, msg, msg_size, 0))
			throw "Recieving message";
		return msg;
	}
	catch (const char* msg) {
		std::cout << msg << " error:" << WSAGetLastError() << "\n";
	}
}


void Client::RecvClients()
{
	try {
		if (SOCKET_ERROR == recv(connection, reinterpret_cast<char*>(&clients_num), sizeof(int), 0))
			throw "Recieving number of clients";
		for (int i = 0; i < clients_num; ++i) {
			char* name = RecvMessage();
			if (std::find(clients_name.begin(), clients_name.end(), name) == clients_name.end())
				clients_name.push_back(name);
		}
	}
	catch (const char* msg) {
		std::cout << msg << " error:" << WSAGetLastError() << "\n";
	}
}
