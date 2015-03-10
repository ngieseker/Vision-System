#ifndef CLUSTER_COMMS_H
#define CLUSTER_COMMS_H

#include "network.h"
#include "pb/cluster.pb.h"


typedef char message_type;
const message_type MSG_CONFIG      = 0x1;
const message_type MSG_CLIENT_INIT = 0x2;
const message_type MSG_VISION_DATA = 0x3;
const message_type MSG_IMAGE       = 0x4;

const size_t MSG_HEADER_SIZE = sizeof(message_type) + sizeof(size_t);


struct ClusterMessage {
	message_type type;
	size_t       size;
	char        *data;

	ClusterMessage() : type(0), data(NULL) {}
	~ClusterMessage() { delete data; }
};

class ClusterComms {
public:
	ClusterComms(socket_t socket) : socket(socket) {}

	int read(ClusterMessage *m);
	int write(ClusterMessage *m);

private:
	socket_t socket;

	char header_buf[MSG_HEADER_SIZE];

};

#endif
