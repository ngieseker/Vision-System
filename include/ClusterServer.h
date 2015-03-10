#ifndef CLUSTER_SERVER_H
#define CLUSTER_SERVER_H

#include "common.h"
#include "network.h"
#include "Configuration.h"
#include "ClusterComms.h"

#include "DataManager.h"
#include "fxbcam.h"
#include "BlockingQueue.h"

#include <pthread.h>
#include <poll.h>
#include <vector>
#include <deque>


class ClusterServer {
public:
	ClusterServer();
	ClusterServer(const char *cfg_file);

	~ClusterServer();

	bool initialize(const char *cfg_file);

	bool run();

private:
	struct VisionClient {
		size_t id;
		bool active;
		bool initialized;
		ClientParams params;

		pthread_t *thread;
		pthread_mutex_t *mutex;
		socket_t socket;
		sockaddr_storage addr;

		FXBcam vis;

		ClusterServer *cs; // For cleanup
		DataManager *dm;
	};

	struct DataClient {
		size_t id;
		bool initialized;

		socket_t socket;
		sockaddr_storage addr;
	};

	struct DataMessage {
		Vec3 marker;
		std::vector<Ray> rays;

		DataMessage(Vec3 marker, const std::vector<Ray>& rays) : marker(marker), rays(rays) {}
	};


	bool initialized;
	Configuration config;

	socket_t vision_socket;
	socket_t data_socket;

	addrinfo vision_addr;
	addrinfo data_addr;

	//pthread_t listener_thread;
	pthread_t server_thread;
	pthread_t data_thread;

	DataManager dm;

	size_t next_vision_id;
	std::vector<VisionClient *> vision_clients;
	pthread_mutex_t vision_clients_mutex;

	size_t next_data_id;
	std::vector<DataClient> data_clients;
	pthread_mutex_t data_clients_mutex;

	BlockingQueue<std::vector<DataMessage> > data;

	/* Consumes the data received from vision clients */
	static void *run_server_thread(void *arg);
	/* Receives data from connected vision clients */
	static void *run_client_thread(void *arg);

	/* Sends data to connected data clients */
	static void *run_data_thread(void *arg);


	/** Helper functions **/
	void init_data_client(DataClient &dc);

};

#endif
