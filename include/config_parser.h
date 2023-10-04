#pragma once
#include <string>
#include <vector>
#include <filesystem>

namespace config {
struct net_addr {
	std::string host; 				// IP address
	unsigned int service; 			// Port number
};

struct balancer {
	net_addr hostname; 
	unsigned int max_dg_load; 		// Max datagrams per second
};

struct server {
	std::string alias;
	net_addr hostname; 
};

struct main_task {
	struct balancer nlb;
  	std::vector<server> server_pool;
};

struct main_task parse(const std::filesystem::path &conf_path);
}; // namespace config
