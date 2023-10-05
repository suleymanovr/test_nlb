#include <config_parser.h>
#include <iostream>

static void print_structured_input(const config::main_task &conf)
{
	auto hostname = [](config::net_addr s)
	{
		std::stringstream p;
		p << "host: " << s.host << "\nservice: " << s.service;
		return p.str(); 
	};

	std::cout << "Balancer config:\n" 
			  << hostname(conf.nlb.hostname) << "\n"
			  << "max_dg_load: " << conf.nlb.max_dg_load << "\n\n";

	for (auto rec : conf.server_pool){
		std::cout << "Server config:\n"
				  << "alias: " << rec.alias << "\n"
				  << hostname(rec.hostname) << "\n\n";
	}
}

int main(int argc, char *argv[])
{
	config::main_task conf{};	
	
	std::filesystem::path conf_path{argv[1]};
	if (!config::parse(conf_path, conf)){
		std::cerr << "Unable to parse input file: " << conf_path << std::endl;
		return 1;
	}
	print_structured_input(conf);
	return 0;
}
