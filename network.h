#ifndef NETWORK_H_
#define NETWORK_H_

#include <netinet/in.h>

struct sockaddr_in make_addr(int port);
int do_accept(int sd) {
	struct sockaddr addr;
	int addrlen = sizeof(addr);
	return accept(sd, &addr, &addrlen);
}

int do_listen(int port) {
	int sd;
	struct sockaddr_in addr = make_addr(port);
	if( (sd = socket(PF_INET, SOCK_STREAM, 0)) < 0 )
	{
		perror("socket error");
		return -1;
	}
	if (bind(sd, (struct sockaddr*) &addr, sizeof(addr)) != 0)
	{
		perror("bind error");
		return -1;
	}
	if (listen(sd, 2) < 0)
	{
		perror("listen error");
		return -1;
	}
	return sd;
}

struct sockaddr_in make_addr(int port) {
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	return addr;
}
#endif