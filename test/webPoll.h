#ifndef WEBTEST_WEBPOLL_H
#define WEBTEST_WEBPOLL_H

#include <poll.h>
#include <vector>

namespace webserv {
	class webPoll {
	public:
		webPoll() = default;
		~webPoll() = default;
		std::vector<pollfd> m_pollFds {};
		void createPoll(int fd) const;

	};
}


#endif //WEBTEST_WEBPOLL_H
