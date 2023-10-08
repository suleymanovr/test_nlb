#include <iostream>
#include <string>
#include <chrono>

#include "balancer.h"

using namespace std;

namespace error {
	void fail_init_socket(config::net_addr a) {
  	  	std::cerr << "Failed to init socker for " << a.host << " " << a.service
				  << std::endl;
	}
} // namespace error


// Fair distribution algorithm
template <typename where, typename what>
void dummy_round_robin(list<where> &target_pool, what to_send) {
	static auto i = target_pool.begin();

  	if (i == target_pool.end()) {
  		i = target_pool.begin();
  	}
  	if (!(*i << to_send)) {
		// Socket input failed, remove this node from pool
		i = target_pool.erase(i);
    	return;
  	}
  	i++;
}

balancer::balancer(const config::main_task &set) 
	// Initialize outer inet interface (as server) to receive client's datagramms
	: recv{set.load_bal.hostname.host, set.load_bal.hostname.service} 
{
	if (!recv.init()){
		error::fail_init_socket(set.load_bal.hostname);
		throw error::socket_init{};
	}

	// Initialize inner inet interfaces (as clients) to redirect datagramms
	for (auto conf_node : set.server_pool) {
		inet::inner_if node{conf_node.hostname.host, 
							conf_node.hostname.service};

		if (!node.init()) {
      		// Failed to initialize socket, skip this node
			error::fail_init_socket(conf_node.hostname);
      	  	continue;
		
		}
		redir.push_back(node);
	}

	if (redir.empty()) {
		throw error::socket_init{};	
	}

	max_load = set.load_bal.max_dg_load;
}


void balancer::run(void)
{
  	string msg;
	long dg_per_sec = chrono::nanoseconds(1s).count()/max_load;

	chrono::steady_clock::time_point t1{};

	// Looped until socket operation fail or server pool empty
	while (!(recv >> msg) && !redir.empty()) {
		auto t2 = chrono::steady_clock::now();
		auto diff = t2 - t1;
		if (diff.count() < dg_per_sec) {
    		/* cout << "DROPPED: \"" << msg << "\"" << endl; */
		} else {
			t1 = t2;
    		dummy_round_robin(redir, msg);
    		/* cout << msg << endl; */
		}
	}
}
