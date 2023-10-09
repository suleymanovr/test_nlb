#pragma once

#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

namespace inet {
struct msg {
	std::string msg;
	std::string host;
	unsigned service;
};

class inner_if {
	public:
		inner_if(const std::string host_, const unsigned service_)
  	  	  : host{host_}, service{service_} {}

		bool init();
		bool send(msg msg_to_send);
		void shutdown() { close(socket_fd); }

	private:
		int socket_fd;
		const std::string host;
		const unsigned service;
};

class outer_if {
	public:
		outer_if(const std::string host_, const unsigned service_)
  	  	  : host{host_}, service{service_} {}

		bool init();
		bool receive(std::string &msg_to_recv);
		void shutdown() { close(socket_fd); }

	private:
		int socket_fd;
		const std::string host;
		const unsigned service;
};

} // namespace inet
