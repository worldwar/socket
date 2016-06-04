#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
struct sockaddr_in make_addr();

int main() {
	int PORT_NO = 4455;

	int sd = do_listen(PORT_NO);
	
	if (sd == -1) {
		return 0;
	}

	int BUFFER_LENGTH = 1024;
	char buffer[BUFFER_LENGTH];

	int fd_max;
	fd_max = sd + 1;	
	fd_set fs, afs;
	FD_ZERO(&afs);
	FD_SET(sd, &afs);
	while (1) {
		fs = afs;
		select(fd_max, &fs, NULL, NULL, NULL);
		int old_max = fd_max;
		for (int i = 0; i < old_max; i++) {
			if (FD_ISSET(i, &fs)) {
				if (i == sd) {
					int newsd = do_accept(sd);
					if (!newsd) {
						perror("accept error!");
					}
					FD_SET(newsd, &afs);
					if (newsd >= fd_max) {
						fd_max = newsd + 1;
					}
				} else {
					int length = recv(i, buffer, BUFFER_LENGTH, 0);
					if (!length) {
						perror("connect closed!");
						FD_CLR(i, &afs);
						break;
					}
					buffer[length] = '\0';
					send(i, buffer, length, 0);
					if (strncmp(buffer, "exit", 4) == 0) {
						FD_CLR(i, &afs);
						close(i);
					}
					if (strncmp(buffer, "shutdown", 8) == 0) {
						return 1;
					}
				}
			}
		}
	}
	return 1;
}

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