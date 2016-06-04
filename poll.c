#include <sys/poll.h>
#include <stdio.h>
#include <string.h>
#include "network.h"

void do_remove(struct pollfd pollfds[], int index, int number);
void do_remove_all(struct pollfd * pollfds, int number);

int main() {
	int sd = do_listen(4455);
	int POLL_SIZE = 1024;
	
	if (sd < 0) {
		perror("error when listen on port 4455");
		return 0;
	}
	
	struct pollfd pollfds[POLL_SIZE];
	int numfds = 0;
	int new_numfds = 0;
	pollfds[0].fd = sd;
	pollfds[0].events = POLLIN;
	new_numfds++;
	while (1) {
		numfds = new_numfds;
		poll(pollfds, numfds, -1);
		for (int i = 0; i < numfds; ++i) {
			int fd = pollfds[i].fd;
			int revents = pollfds[i].revents;
			if (revents & POLLIN) {
				if (fd == sd) {
					int conn_socket = do_accept(fd);
					if (conn_socket > -1) {
						new_numfds++;
						pollfds[new_numfds - 1].fd = conn_socket;
						pollfds[new_numfds - 1].events = POLLIN;
					}
				} else {
					int BUFFER_LENGTH = 1024;
					char buffer[BUFFER_LENGTH];
					int length = recv(fd, buffer, BUFFER_LENGTH, 0);
					if (length > 0) {
						send(fd, buffer, length, 0);
					} else {
						do_remove(pollfds, i, numfds);
						new_numfds--;
					}
					if (strncmp(buffer, "exit", 4) == 0) {
						do_remove(pollfds, i, numfds);
						new_numfds--;
					}
					if (strncmp(buffer, "shutdown", 8) == 0) {
						do_remove_all(pollfds, numfds);
						return 1;
					}
				}
			}
		}
	}
	return 1;
}

void do_remove(struct pollfd * pollfds, int index, int number) {
	close(pollfds[index].fd);
	pollfds[index] = pollfds[number - 1];
}

void do_remove_all(struct pollfd * pollfds, int number) {
	for (int i = 0; i < number; i++) {
		close(pollfds[i].fd);
	}
}