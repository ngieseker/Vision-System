#include "network.h"


void *get_in_addr(struct sockaddr_storage *sa)
{
	if (sa->ss_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

bool get_ip(struct sockaddr_storage *addr, char *buf, socklen_t len) {
	const char *res = inet_ntop(addr->ss_family, get_in_addr(addr), buf, len);
	if (res == NULL) {
		perror("inet_ntop");
		return false;
	}

	return true;
}


bool socket_open(socket_t *sock, const char *hostname, const char *port, const struct addrinfo *hints, struct addrinfo *serv, int flags) {
	socket_t sockfd;
	addrinfo *servinfo;
	
	int err;
	if ( (err = getaddrinfo(hostname, port, hints, &servinfo)) != 0 ) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
		return -1;
	}

	addrinfo *s;
	int yes = 1;
	for (s = servinfo; s != NULL; s = s->ai_next) {
		if ( (sockfd = socket(s->ai_family, s->ai_socktype | flags, 0)) == -1 ) {
			perror("socket");
			continue;
		}

		if (hostname == NULL) {
			if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
				perror("set SO_REUSEADDR");
				return -1;
			}

			if (bind(sockfd, s->ai_addr, s->ai_addrlen) == -1) {
				close(sockfd);
				perror("bind");
				continue;
			}
		} else {
			if (connect(sockfd, s->ai_addr, s->ai_addrlen) == -1) {
				close(sockfd);
				perror("connect");
				continue;
			}
		}

		break;
	}

	if (s == NULL) {
		if (hostname == NULL)
			fprintf(stderr, "failed to bind on port %s\n", port);
		else
			fprintf(stderr, "failed to connect to %s:%s\n", hostname, port);

		return false;
	}

	if (serv != NULL)
		memcpy(serv, s, sizeof(struct addrinfo));

	freeaddrinfo(servinfo);

	*sock = sockfd;
	return true;
}

bool socket_listen(socket_t sock, int backlog) {
	if (listen(sock, backlog) < 0) {
		perror("listen");
		return false;
	}

	return true;
}

bool socket_accept(socket_t sock, socket_t *accepted_sock, struct sockaddr *addr, socklen_t *addrlen) {
	socket_t new_sock;
	if ( (new_sock = accept(sock, addr, addrlen)) < 0 ) {
		//perror("accept");
		return false;
	}

	*accepted_sock = new_sock;

	return true;
}


