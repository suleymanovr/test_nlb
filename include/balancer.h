#pragma once

#include <list>

#include "inet_interface.h"
#include "config_parser.h"

namespace error {
struct socket_init {};
} // namespace error

class balancer {
	public:
		balancer(const config::main_task &set); 
		void run(void);

	private:
		inet::outer_if recv;
		std::list<inet::inner_if> redir;
		unsigned int max_load;
};
