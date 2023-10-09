#include <iostream>
#include <sstream>
#include <string>
#include <random>
#include <chrono>

#include <unistd.h>

#include "inet_interface.h"

using namespace std;

class rand_uint {
	public:
		explicit rand_uint(unsigned low, unsigned high) : dist{low, high} {}
		int operator()() { return dist(re); }
	private:
		default_random_engine re;
		uniform_int_distribution<unsigned> dist;
};

struct load_gen {
	public:
		load_gen(unsigned int max_time) : rand_sleep_val{1, max_time} {}

		string operator()()
		{
			unsigned rand_n = rand_sleep_val();
			usleep(rand_n);

			stringstream ret_msg;
			ret_msg << "Hello, I slept for " << rand_n << " microsecs.";

			return ret_msg.str(); 
		}

	private:
		rand_uint rand_sleep_val;
};

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

int main(int argc, char *argv[])
{
	if (argc < 4){
		cerr << "Must specify server IP address, port number and uptime!\n";
		return 1;
	}

	inet::inner_if client{"171.0.0.6", 60006};

	if (!client.init()){
		cerr << "Unable to initilize client!\n";
		return 1;
	}
	
	// Generate message randomly in period from 1 to: 
	load_gen random_freq_msg{1000};  //microsecs

	inet::msg send;
	send.host = argv[1];
	send.service = atoi(argv[2]);

	unsigned int how_long = atoi(argv[3]);
	time_interval live{how_long};
	live.begin();

	while(!live.expired()){
		send.msg = random_freq_msg(); 
		client.send(send);
		cout << send.msg << endl;
	}
	client.shutdown();
}
