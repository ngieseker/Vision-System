#include "network.h"
#include "common.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>


struct packet {
	size_t id;
	timespec ts;
};


bool sendPacket(socket_t socket, packet &p, timespec *t) {
	size_t total = 0;
	ssize_t n_sent;

	clock_gettime(CLOCK_MONOTONIC, &p.ts);

	if (t)
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, t);

	while (total < sizeof(packet)) {
		n_sent = send(socket, &p + total, sizeof(packet) - total, MSG_NOSIGNAL);

		if (n_sent <= 0) {
			fprintf(stderr, "Couldn't send ping packet\n");
			return false;
		}

		total += n_sent;
	}

	return true;
}

bool readPacket(socket_t socket, packet &p, timespec *t) {
	size_t total = 0;
	ssize_t n_read;

	bool set_time = false;
	while (total < sizeof(packet)) {
		n_read = recv(socket, &p + total, sizeof(packet) - total, 0);
		if (!set_time && t) {
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, t);
			set_time = true;
		}

		if (n_read <= 0) {
			fprintf(stderr, "Couldn't recv ping packet\n");
			return false;
		}

		total += n_read;
	}

	return true;
}


int main(int argc, char *argv[]) {
	char *target = NULL;
	if (argc >= 2) {
		target = (char *)malloc(strlen(argv[1]));
		strcpy(target, argv[1]);
	}

	char port[6] = "23581";


	socket_t socket;

	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags    = AI_PASSIVE;


	addrinfo addr;
	if (!socket_open(&socket, target, port, &hints, &addr, TCP_NODELAY))
		return 1;

	//int yes = 1;
	//if (setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(int)) == -1) {
		//perror("set SO_REUSEADDR");
		//return 1;
	//}

	if (target == NULL) {
		if (!socket_listen(socket, 8)) {
			fprintf(stderr, "Failed to listen\n");
		}

		socket_t client;
		sockaddr_storage client_addr;
		char client_ip[INET6_ADDRSTRLEN];
		socklen_t addrlen = sizeof(client_addr);

		if (!socket_accept(socket, &client, (sockaddr *)&client_addr, &addrlen)) {
			perror("Error accepting client");
			return 1;
		}

		if (!get_ip(&client_addr, client_ip, addrlen)) {
			perror("Error getting client IP");
			return 1;
		}

		printf("Testing latency to %s\n", client_ip);

		size_t cur_id = 0;
		timespec send_time, recv_time;
		
		while (1) {
			packet p_s, p_r;
			p_s.id = cur_id++;

			if (!sendPacket(client, p_s, &send_time))
				return 1;

			if (!readPacket(client, p_r, &recv_time))
				return 1;

			clock_gettime(CLOCK_MONOTONIC, &p_r.ts);

			printf("%4lu:  CPU Time: %11.6fms   Real Time: %11.6f\n", cur_id, 1000*elapsed(&send_time, &recv_time), 1000*elapsed(&p_s.ts, &p_r.ts));

			usleep(200000);
		}
	} else {
		while (1) {
			packet p;

			if (!readPacket(socket, p, NULL))
				return 1;

			if (!sendPacket(socket, p, NULL))
				return 1;
		}
	}


	return 0;
}
