#ifndef WEBTEST_WEBSOCKET_H
#define WEBTEST_WEBSOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <sys/poll.h>
#include <vector>

namespace webserv {


	class webSocket {
	public:
		webSocket(int ip, int port);
		~webSocket() = default;
		void createSocket();
		void read();
		void respond(int socketfd);

	private:
		int m_socket{};
		const int m_ip;
		const int m_port;
		struct sockaddr_in m_serverAddress{};
		unsigned int  m_serverAddressLen{sizeof (m_serverAddress)};
		std::vector<pollfd> m_pollFds {};
		void addPoll(const int fd);
	};
}

#endif //WEBTEST_WEBSOCKET_H
