#pragma once

#include <list>

#include "inet_interface.h"
#include "config_parser.h"

namespace error {
struct socket_init {};
struct bad_max_dg_load {};
} // namespace error

class balancer {
	public:
		balancer(const config::main_task &set); 
		void run(void);

	private:
		inet::outer_if recv;
		inet::inner_if redir;
		std::list<config::net_addr> server_pool;
		unsigned int max_load;
};
