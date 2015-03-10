#include "ClusterComms.h"


int ClusterComms::read(ClusterMessage *m) {
	size_t total = 0;
	ssize_t n_read;

	while (total < MSG_HEADER_SIZE) {
		n_read = recv(socket, header_buf + total, MSG_HEADER_SIZE - total, 0);

		if (n_read < 0) {
			perror("recv");
			return -1;
		} else if (n_read == 0) {
			fprintf(stderr, "read: socket closed\n");
			return 0;
		}

		total += n_read;
	}

	memcpy(&m->type, header_buf + 0, sizeof(m->type));
	memcpy(&m->size, header_buf + 1, sizeof(m->size));
	m->data = new char[m->size];

	total = 0;
	while (total < m->size) {
		n_read = recv(socket, m->data + total, m->size - total, 0);

		if (n_read < 0) {
			perror("recv");
			return -1;
		} else if (n_read == 0) {
			fprintf(stderr, "read: socket closed\n");
			return 0;
		}

		total += n_read;
	}


	return 1;
}


int ClusterComms::write(ClusterMessage *m) {
	size_t total = 0;
	ssize_t n_sent;

	memcpy(header_buf + 0, &m->type, sizeof(m->type));
	memcpy(header_buf + 1, &m->size, sizeof(m->size));

	while (total < MSG_HEADER_SIZE) {
		n_sent = send(socket, header_buf + total, MSG_HEADER_SIZE - total, MSG_NOSIGNAL);

		if (n_sent < 0) {
			perror("send");
			return -1;
		} else if (n_sent == 0) {
			fprintf(stderr, "write: socket closed\n");
			return 0;
		}

		total += n_sent;
	}

	total = 0;
	while (total < m->size) {
		n_sent = send(socket, m->data + total, m->size - total, MSG_NOSIGNAL);

		if (n_sent < 0) {
			perror("send");
			return -1;
		} else if (n_sent == 0) {
			fprintf(stderr, "write: socket closed\n");
			return 0;
		}

		total += n_sent;
	}


	return 1;
}

