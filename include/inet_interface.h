#pragma once

#include <string>
#include <sys/socket.h>
#include <netdb.h>

#include "config_parser.h"

namespace inet {
class inner_if {
	public:
		inner_if(const std::string host_, const unsigned service_)
  	  	  : host{host_}, service{service_} {}

		bool init();
		bool operator<<(std::string msg_to_send);

	private:
		int socket_fd;
		const std::string host;
		const unsigned service;
		struct addrinfo sockinfo;
};

class outer_if {
	public:
		outer_if(const std::string host_, const unsigned service_)
  	  	  : host{host_}, service{service_} {}

		bool init();
		bool operator>>(std::string msg_to_recv);

	private:
		int socket_fd;
		const std::string host;
		const unsigned service;
};

} // namespace inet
