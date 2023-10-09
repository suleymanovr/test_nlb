#include <iostream>
#include <string>
#include <chrono>

#include "inet_interface.h"

using namespace std;

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
	if (argc < 4) {
		cerr << "Must specify IP address, port number and uptime(secs)!\n";
		return 1;
	}

	unsigned int port = atoi(argv[2]);
	inet::outer_if server{argv[1], port};

	if (!server.init()){
		cerr << "Unable to initilize server!\n";
		return 1;
	}

	unsigned int how_long = atoi(argv[3]);
	time_interval live{how_long};
	live.begin();

	string msg;
	while(!live.expired()){
		server.receive(msg);
		cout << msg << endl;
	}
	server.shutdown();
}
