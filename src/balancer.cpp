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

class time_interval {
	public:
		time_interval(unsigned set) : how_long{set}{}

		void begin(void) { start = chrono::steady_clock::now(); }
		bool expired(void) {
			auto now = chrono::steady_clock::now();

/* NOTE: Automatically resets start value if trigger time exceeded */
			if (now - start >= how_long) {
				start = now;
				return true;
			} else {
				return false;
			}
		}
	private:
		const chrono::duration<unsigned, ratio<1,1>> how_long;
		chrono::steady_clock::time_point start;
}; 

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
  	unsigned long long cnt = 0;

	time_interval working_window{1};
	working_window.begin();

	// Looped until socket operation fail or server pool empty
	while (recv.receive(msg) && !server_pool.empty()) {
		if (!working_window.expired()) {
			if ((++cnt) <= max_load) {			
    			dummy_round_robin(redir, server_pool, msg);
    			cout << msg << endl;
			} else {
				cerr << "DROPPED: \"" << msg << "\"" << endl;
			}
		} else {
			cnt = 0;
		}
	}
}
