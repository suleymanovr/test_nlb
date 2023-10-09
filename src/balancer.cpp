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
void dummy_round_robin(inet::inner_if dest, list<config::net_addr> &pool, 
					   string msg_to_send) 
{
	static auto i = pool.begin();
	
  	if (i == pool.end()) {
  		i = pool.begin();
  	}

	inet::msg m;
	m.msg.assign(msg_to_send);
	m.host.assign(i->host);
	m.service = i->service;

  	if (!(dest.send(m))) {
		// Socket input failed, remove this node from pool
		i = pool.erase(i);
    	return;
  	}
  	i++;
}

balancer::balancer(const config::main_task &set) 
	// Initialize outer inet interface (as server) to receive client's datagramms 
	// and inner inet interface (as client) to redirect datagramms
	: recv{set.nlb.outer.host, set.nlb.outer.service},
	  redir{set.nlb.inner.host, set.nlb.inner.service}
{
	if (!recv.init()){
		error::fail_init_socket(set.nlb.outer);
		throw error::socket_init{};
	}

	if (!redir.init()){
		error::fail_init_socket(set.nlb.inner);
		throw error::socket_init{};
	}

	for (auto node : set.server_pool) {
		server_pool.push_back(node);
	}

	if (server_pool.empty()) {
		throw error::socket_init{};	
	}

	max_load = set.nlb.max_dg_load;
}


void balancer::run(void)
{
  	string msg;
	long dg_per_sec = chrono::nanoseconds(1s).count()/max_load;

	chrono::steady_clock::time_point t1{};

	// Looped until socket operation fail or server pool empty
	while (recv.receive(msg) && !server_pool.empty()) {
		auto t2 = chrono::steady_clock::now();
		auto diff = t2 - t1;
		if (diff.count() < dg_per_sec) {
    		cout << "DROPPED: \"" << msg << "\"" << endl;
		} else {
			t1 = t2;
    		dummy_round_robin(redir, server_pool, msg);
		}
	}
}
