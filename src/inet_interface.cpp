#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

#include <iostream>

#include "inet_interface.h"

using namespace std;

#define IS_ADDR_STR_LEN (NI_MAXHOST + NI_MAXSERV + 10)
#define MAX_RD_BUF_SZ 500

static char* inet_addr_to_str(const struct sockaddr *addr, socklen_t addrlen,
						char *addr_str, int addr_str_len)
{
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	if (getnameinfo(addr, addrlen, host, NI_MAXHOST,
				   service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) == 0){
		snprintf(addr_str, addr_str_len, "%s, %s", host, service);
	} else {
		snprintf(addr_str, addr_str_len, "Unknown host\n");
	}
	addr_str[addr_str_len-1] = '\0';

	return addr_str;
}

static int get_addrinfo(const char *host, const char *service, const int type,
						struct addrinfo **ret)
{
	struct addrinfo hints;

	int s;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_canonname = NULL;
	hints.ai_addr  	   = NULL;
	hints.ai_next  	   = NULL;
	hints.ai_family    = AF_UNSPEC;
	hints.ai_socktype  = type;

	s = getaddrinfo(host, service, &hints, ret);
	if (s != 0){
		if (s == EAI_SYSTEM){
			printf("init_socket - %s\n", strerror(errno));
			return -1;
		} else {
			printf("init_socket - %s\n", gai_strerror(s));
			return -1;
		}
	}

	return 0;
}

static int init_socket(const char *host, const char *service, 
					   const int type, bool do_bind)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;

	int sfd;

	if (get_addrinfo(host, service, type, &result) == -1){
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next){
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1){
			cerr << "socket() failed: " << strerror(errno);
			continue;
		}
		if (do_bind){
			if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0){
				// Success
				cout << "Socket " << host << " " << service << ": bound success!\n";
				break;
			}
			// bind() failed: close this socket and try next address
			close(sfd);
		}
	}

	freeaddrinfo(result);
	return sfd;
}


bool inet::inner_if::init()
{	
	string s_service = to_string(service); 
	socket_fd = init_socket(host.c_str(), s_service.c_str(), SOCK_DGRAM, false);

	if (socket_fd == -1) {
		return false;
	}
	return true;
}

bool inet::inner_if::send(inet::msg msg_to_send)
{
	struct addrinfo *sockinfo;
	string service = to_string(msg_to_send.service); 

	get_addrinfo(msg_to_send.host.c_str(), service.c_str(), SOCK_DGRAM, &sockinfo);

	ssize_t res = sendto(socket_fd, msg_to_send.msg.c_str(), msg_to_send.msg.size(), 
						 0, sockinfo->ai_addr, sockinfo->ai_addrlen);

	if (res == -1){
		cerr << "sendto() failed: " << strerror(errno) << endl;
		return false;
	}

	freeaddrinfo(sockinfo);
	return true;
}

bool inet::outer_if::init()
{
	string s_service = to_string(service);
	socket_fd = init_socket(host.c_str(), s_service.c_str(), SOCK_DGRAM, true);

	if (socket_fd == -1) {
		return false;
	}
	return true;
}

bool inet::outer_if::receive(string &msg_to_recv)
{
	char buf[MAX_RD_BUF_SZ];

	ssize_t res = recvfrom(socket_fd, &buf, MAX_RD_BUF_SZ, 0, NULL, NULL);

	if (res == -1){
		cerr << "recvfrom() failed: " << strerror(errno) << endl;
		return false;
	}
	msg_to_recv.assign(buf, res);
	return true;
}
