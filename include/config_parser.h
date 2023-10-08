#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <utility>

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
	net_addr hostname; 
};

struct main_task {
	struct balancer load_bal;
  	std::vector<server> server_pool;
};

bool parse(const std::filesystem::path &conf_path, struct main_task &parse_conf);
}; // namespace config
