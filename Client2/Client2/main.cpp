#include "Client.h"

int main() {
	const char* address = "127.0.0.1";
	int port = 1280;
	Client client(address,port);
	client.Connect();
	return 0;
}
