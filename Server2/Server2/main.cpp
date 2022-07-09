#include "Server.h"


int main() {
	const char* address = "127.0.0.1";
	int port = 1280;
	Server server(address, port);
	server.Start();
	return 0;
}