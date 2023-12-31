#include <iostream>
#include <string>

#include "balancer.h"

namespace print {
namespace debug {
static void structured_input(const config::main_task &conf) {
	auto hostname = [](config::net_addr s) {
	std::stringstream p;
	p << "host: " << s.host << "\nservice: " << s.service;
	return p.str();
	};
	std::cout << "Balancer config:\n"
        	<< hostname(conf.nlb.outer) << "\n"
        	<< hostname(conf.nlb.inner) << "\n"
        	<< "max_dg_load: " << conf.nlb.max_dg_load << "\n\n";

	for (auto rec : conf.server_pool) {
	std::cout << "Server config:\n"
          	  << hostname(rec) << "\n\n";
	}
}
} // namespace debug
} // namespace print

int main(int argc, char *argv[])
{
	config::main_task conf{};

	std::filesystem::path conf_path{argv[1]};
	if (!config::parse(conf_path, conf)) {
		std::cerr << "Unable to parse input file: " << conf_path << std::endl;
		return 1;
	}

	balancer nlb{conf};
	nlb.run();

  	return 0;
}
