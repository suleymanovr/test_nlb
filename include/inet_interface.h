#pragma once

#include <string>
#include <sys/socket.h>

#include "config_parser.h"

namespace inet {
class inner_if {
	public:
		inner_if(const std::string host_, const unsigned service_)
  	  	  : host{host_}, service{service_} {}

		bool init();
		bool is_initialized() { return initialized; };
		bool operator<<(std::string msg_to_send);

	private:
		bool initialized;
		int socket_fd;
		const std::string host;
		const unsigned service;
		struct sockadd *dest_addr;
		socklen_t dest_len;
};

class outer_if {
	public:
		outer_if(const std::string host_, const unsigned service_)
  	  	  : host{host_}, service{service_} {}

		bool init();
		bool is_initialized() { return initialized; };
		bool operator>>(std::string msg_to_recv);

	private:
		bool initialized;
		int socket_fd;
		const std::string host;
		const unsigned service;
};

} // namespace inet
