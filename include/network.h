#ifndef NETWORK_H
#define NETWORK_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>


typedef int socket_t;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr_storage *sa);
bool get_ip(struct sockaddr_storage *addr, char *buf, socklen_t len);

bool socket_open(socket_t *sock, const char *hostname, const char *port, const struct addrinfo *hints, struct addrinfo *serv, int flags);
bool socket_listen(socket_t sock, int backlog);
bool socket_accept(socket_t sock, socket_t *accepted_sock, struct sockaddr *addr, socklen_t *addrlen);

#endif
