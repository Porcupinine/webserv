#include <iostream>
#include "webSocket.h"
#include "defines.h"
#include <poll.h>

int main() {
	webserv::webSocket serv = webserv::webSocket(127, SERV_PORT);
	serv.createSocket();
	serv.read();
	nfds_t test;
	return 0;
}
