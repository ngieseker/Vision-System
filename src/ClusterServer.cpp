#include "ClusterServer.h"
#include <stdint.h>
#include <algorithm>

#include <sys/types.h>
#include <sys/stat.h>


ClusterServer::ClusterServer() : next_vision_id(0), next_data_id(0) {
	initialized = false;
}

ClusterServer::ClusterServer(const char *cfg_file) : next_vision_id(0), next_data_id(0) {
	initialize(cfg_file);
}

ClusterServer::~ClusterServer() {
	// clean up
}


bool ClusterServer::initialize(const char *cfg_file) {
	initialized = config.read(cfg_file);
	return initialized;
}

bool ClusterServer::run() {
	char vision_port[32];
	char data_port[32];
	sprintf(vision_port, "%d", config.getInt("vision_port"));
	sprintf(data_port, "%d", config.getInt("data_port"));

	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags    = AI_PASSIVE;

	/* Open vision client socket */
	if (!socket_open(&vision_socket, NULL, vision_port, &hints, &vision_addr, 0))
		return false;

	if (!socket_listen(vision_socket, 8))
		return false;

	/* Open data client socket */
	if (!socket_open(&data_socket, NULL, data_port, &hints, &data_addr, 0))
		return false;

	if (!socket_listen(data_socket, 8))
		return false;


	pthread_mutex_init(&vision_clients_mutex, NULL);
	pthread_mutex_init(&data_clients_mutex, NULL);

	pthread_attr_t thread_attr;
	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

	/* Start the server thread */
	if (pthread_create(&server_thread, &thread_attr, run_server_thread, this) != 0) {
		perror("pthread_create");
		return false;
	}

	/* Start the data thread */
	if (pthread_create(&data_thread, &thread_attr, run_data_thread, this) != 0) {
		perror("pthread_create");
		return false;
	}


	/* Poll for vision clients and data clients */
	struct pollfd sockets[2];

	sockets[0].fd = vision_socket;
	sockets[0].events = POLLIN;
	sockets[1].fd = data_socket;
	sockets[1].events = POLLIN;


	/* Data structures for accepting either type of client */
	socket_t client_socket;
	sockaddr_storage client_addr;
	socklen_t addrlen = sizeof(client_addr);

	char client_ip[INET6_ADDRSTRLEN];


	/* Data structures for accepting vision clients */
	VisionClient *client;


	/*** Start listenting for clients ***/
	while (1) {
		if (poll(sockets, 2, -1) == -1) {
			perror("poll");
			return false;
		}

		/* Check sockets for events */
		if (sockets[0].revents & POLLIN) { // New vision client
			if (!socket_accept(vision_socket, &client_socket, (sockaddr *)&client_addr, &addrlen))
				continue;

			if (!get_ip(&client_addr, client_ip, addrlen))
				continue;

			ClientParams c;
			if (!config.getClient(client_ip, &c)) {
				close(client_socket);
				printf("Rejected vision client from %s\n", client_ip);
				continue;
			}
			
			//TODO: handshake or something
			
			client = new VisionClient;

			client->id          = next_vision_id;
			client->active      = true;
			client->initialized = false;
			client->params      = c;
			client->thread      = new pthread_t;
			client->mutex       = new pthread_mutex_t;
			client->socket      = client_socket;
			client->addr        = client_addr;
			client->cs          = this;
			client->dm          = &dm;

			pthread_mutex_init(client->mutex, NULL);

			printf("[%lu] Vision client from %s (%s)\n", client->id, c.name.c_str(), c.ip.c_str());

			if (pthread_create(client->thread, &thread_attr, run_client_thread, client) != 0) {
				perror("pthread_create");
				continue;
			}

			pthread_mutex_lock(&vision_clients_mutex);
				vision_clients.push_back(client);
				next_vision_id++;
			pthread_mutex_unlock(&vision_clients_mutex);

		} else if (sockets[1].revents & POLLIN) { // New data client
			if (!socket_accept(data_socket, &client_socket, (sockaddr *)&client_addr, &addrlen))
				continue;

			if (!get_ip(&client_addr, client_ip, addrlen))
				continue;

			DataClient dc;
			dc.id = next_data_id++;
			dc.initialized = false;
			dc.socket = client_socket;
			dc.addr = client_addr;

			printf("[%lu] Data client from %s\n", dc.id, client_ip);

			//usleep(10000); // MATLAB sucks

			pthread_mutex_lock(&data_clients_mutex);
				init_data_client(dc);
				data_clients.push_back(dc);
			pthread_mutex_unlock(&data_clients_mutex);
		}

	}

	return true;
}


void *ClusterServer::run_server_thread(void *arg) {
	ClusterServer *cs = (ClusterServer *)arg;
	DataManager *dm = &cs->dm;
	FXBcam vis;


	struct timespec starttime;
	clock_gettime(CLOCK_REALTIME, &starttime);
	struct tm * logtime = localtime((time_t *) &starttime.tv_sec);
	
	mkdir("log", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	char tempfile[128];
	char filename[128];
	sprintf(tempfile, "log/%04i.%02i.%02i-%02i.%02i.%02i.log", logtime->tm_year+1900, logtime->tm_mon+1, logtime->tm_mday, logtime->tm_hour, logtime->tm_min, logtime->tm_sec);
	memcpy(filename, tempfile, sizeof(filename));
	
	// We never want to erase an existing log file
	FILE* logfile;
	int n = 2;
	while ( (logfile = fopen(filename, "r")) ) {
		fclose(logfile);

		memcpy(filename, tempfile, sizeof(filename));
		char *app = strchr(filename, '.');
		sprintf(app, "-%d.log", n++);
	}

	logfile = fopen(filename, "w");
	printf("Logging to %s\n", filename);


	/*** Start main server loop ***/
	while (1) {
		usleep(500);
		DataResults results;
		dm->syncData(results);
		//dm->logStreams();


		timespec time;
		clock_gettime(CLOCK_REALTIME, &time);

		int printed = 0;
		char buf[1024] = {0};
		char *b = buf;
		b += sprintf(b, "%20.8f", (double)time.tv_sec + 1e-9*(double)time.tv_nsec);

		size_t m = 0;

		std::vector<DataMessage> data;
		for (size_t i = 0; i < results.size(); i++, m++) {
			Vec3 pos = vis.intersectRays(results[i]);
			data.push_back(DataMessage(pos, results[i]));

#ifdef BENCHMARK
			clock_gettime(CLOCK_REALTIME, &time);
			double server_time = time.tv_sec + 1e-9*(double)time.tv_nsec;

			double ray_time;
			double delay = 0;
			double overhead;

			//printf("%zu: (", i);
			for (std::vector<Ray>::iterator r = results[i].begin(); r != results[i].end(); ++r) {
				ray_time = r->time_sec + 1e-9*(double)r->time_nsec;

				//overhead = fabs(server_time - ray_time);
				overhead = fabs(server_time - ray_time) - r->bench_findmarkers_total - r->bench_client_overhead;
				delay += overhead;

				//printf("%zu|%9.4f ", r->client_id, 1000*overhead);
				if (results[i].size() == 3)
					fprintf(stderr, "%20.8f %20.6f %20.6f %20.6f %20.6f %20.6f %20.6f %20.6f %20.6f\n", server_time, ray_time, r->bench_color_conversion, r->bench_thresholding, r->bench_blob_detection, r->bench_marker_ident, r->bench_findmarkers_total, r->bench_client_overhead, overhead);
			}
			//printf(" ) mean = %8.4fms\n", 1000*delay / results[i].size());
#endif


			while (results[i][0].marker != m && m < 4) {
				b += sprintf(b, "%20.8s %20.8s %20.8s ", "NaN", "NaN", "NaN");
				m++;
			}

			b += sprintf(b, "%20.8f %20.8f %20.8f ", pos.x, pos.y, pos.z);
			printed++;
		}

		while (m < 4) {
			b += sprintf(b, "%20.8s %20.8s %20.8s ", "NaN", "NaN", "NaN");
			m++;
		}

		if (printed)
			fprintf(logfile, "%s\n", buf);


		if (data.size() != 0) {
#if defined(SYNC_DEBUG) || defined(INTER_DEBUG)
			printf("-----------------------------------------------------------------------\n");
#endif
			cs->data.push(data);
		}
	}

	fclose(logfile);

	return NULL;
}


void *ClusterServer::run_client_thread(void *arg) {
	VisionClient *c = (VisionClient *)arg;
	ClusterServer *cs = c->cs;
	DataManager *dm = c->dm;

	CameraParams cp;

	ClusterComms comm(c->socket);
	ClusterMessage message;

	ClusterProto::Vision_Data vis_data;
	ClusterProto::Client_Init init;
	ClusterProto::Image       image;

	while (true) {
		pthread_mutex_lock(c->mutex);
		if (!c->active) {
			pthread_mutex_unlock(c->mutex);
			break;
		}
		pthread_mutex_unlock(c->mutex);


		if (comm.read(&message) != 1) {
			pthread_mutex_lock(c->mutex);
			c->active = false;
			pthread_mutex_unlock(c->mutex);
			break;
		}

		switch (message.type) {
			case MSG_CONFIG:
				printf("Got config message\n");
				break;

			case MSG_CLIENT_INIT:
				init.ParseFromArray(message.data, message.size);

				if (!cs->config.getCamera(init.cam_guid(), &cp)) {
					fprintf(stderr, "Error: %s has unconfigured camera %lu\n", c->params.name.c_str(), init.cam_guid());
					break;
				}
				printf("%s has camera %d\n", c->params.name.c_str(), cp.id);

				if (!c->vis.readCamPara(cp.calib.c_str()))
					//FIXME: HANDLE THIS ERROR
					break;
				c->vis.readMarkerData("marker.yaml"); //TODO: Add to configuration

				pthread_mutex_lock(c->mutex);
				c->initialized = true;
				pthread_mutex_unlock(c->mutex);

				break;

			case MSG_VISION_DATA:
				vis_data.ParseFromArray(message.data, message.size);

				//printf("  Timestamp: %.8f (%lu)\n", (double)vis_data.time_sec() + 1e-9*(double)vis_data.time_nsec(), vis_data.time_counter());
				for (int r = 0; r < vis_data.ray_size(); r++) {
					ClusterProto::Vision_Data_Ray ray = vis_data.ray(r);

					Ray ry = Ray(vis_data.id(), ray.x(), ray.y(), ray.z(), ray.marker());
					ry.client_id = c->id;
					ry.time_sec  = vis_data.time_sec();
					ry.time_nsec = vis_data.time_nsec();
					ry.setOrigin(c->vis.cam_pos);

#ifdef BENCHMARK
					ry.bench_color_conversion  = vis_data.bench_color_conversion();
					ry.bench_thresholding      = vis_data.bench_thresholding();
					ry.bench_blob_detection    = vis_data.bench_blob_detection();
					ry.bench_marker_ident      = vis_data.bench_marker_ident();
					ry.bench_findmarkers_total = vis_data.bench_findmarkers_total();
					ry.bench_client_overhead   = vis_data.bench_client_overhead();
#endif

					dm->addRay(c->id, ry);

					//printf("   %3d (%8.3f,%8.3f,%8.3f )\n", ry.marker, ry.a, ry.b, ry.c);
				}

				break;

			case MSG_IMAGE:
#ifdef LOG_IMAGES
				image.ParseFromArray(message.data, message.size);

				dm->addImage(c->id, image.id(), image.image());
#endif

				break;
		}
	}

	close(c->socket);

	dm->removeClient(c->id);

	printf("Removing %s from connected vision clients\n", c->params.name.c_str());
	pthread_mutex_lock(&cs->vision_clients_mutex);
		cs->vision_clients.erase(std::find(cs->vision_clients.begin(), cs->vision_clients.end(), c));
	pthread_mutex_unlock(&cs->vision_clients_mutex);

	return NULL;
}



const message_type DATAMSG_INIT        = 1;
const message_type DATAMSG_MARKERS     = 2;
const message_type DATAMSG_POSITION    = 3;
const message_type DATAMSG_ORIENTATION = 4;

void ClusterServer::init_data_client(DataClient &dc) {
	pthread_mutex_lock(&vision_clients_mutex);

	bool cl_init;

	size_t num_cams = 0;
	for (size_t c = 0; c < vision_clients.size(); c++) {
		VisionClient *cl = vision_clients[c];

		pthread_mutex_lock(cl->mutex);
		cl_init = cl->initialized;
		pthread_mutex_unlock(cl->mutex);

		if (cl_init)
			num_cams++;
	}

	//size_t buf_size = 1 + 1 + num_cams * (3*3*8 + 3*8 + 3*3*8);
	size_t buf_size = 1024;
	char *buf = new char[buf_size];

	size_t b = 0;
	memcpy(buf + b, &DATAMSG_INIT, 1);
	b += 1;

	unsigned char nc = num_cams;
	memcpy(buf + b, &nc, 1);
	b += 1;

	for (size_t c = 0; c < vision_clients.size(); c++) {
		VisionClient *cl = vision_clients[c];

		pthread_mutex_lock(cl->mutex);
		cl_init = cl->initialized;
		pthread_mutex_unlock(cl->mutex);

		if (!cl_init)
			continue;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				memcpy(buf + b, &(cl->vis.intrinsic.at<double>(i,j)), 8);
				b += 8;
			}
		}

		for (int i = 0; i < 3; i++) {
			memcpy(buf + b, &(cl->vis.Tc_ext.at<double>(i,0)), 8);
			b += 8;
		}

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				memcpy(buf + b, &(cl->vis.Rc_ext.at<double>(i,j)), 8);
				b += 8;
			}
		}
	}
	pthread_mutex_unlock(&vision_clients_mutex);


	dc.initialized = true;

	size_t total = 0;
	ssize_t n_sent;
	while (total < buf_size) {
		n_sent = send(dc.socket, buf + total, buf_size - total, MSG_NOSIGNAL);

		if (n_sent <= 0) {
			dc.initialized = false;
			fprintf(stderr, "Couldn't initialize data client\n");
			break;
		}

		total += n_sent;
	}

	delete[] buf;
}

void *ClusterServer::run_data_thread(void *arg) {
	ClusterServer *cs = (ClusterServer *)arg;

	std::vector<DataMessage> d;

	for (;;) {
		d = cs->data.pop();

		//size_t buf_size = 1 + 3*8*d.size();
		size_t buf_size = 1024;
		char *buf = new char[buf_size];
		size_t b = 0;

		unsigned char num_markers = d.size();

		memcpy(buf + b, &DATAMSG_MARKERS, 1);
		b += 1;

		memcpy(buf + b, &num_markers, 1);
		b += 1;

		for (size_t i = 0; i < d.size(); i++) {
			memcpy(buf + b, &d[i].marker.x, 8);
			b += 8;
			memcpy(buf + b, &d[i].marker.y, 8);
			b += 8;
			memcpy(buf + b, &d[i].marker.z, 8);
			b += 8;

			unsigned char num_rays = d[i].rays.size();
			memcpy(buf + b, &num_rays, 1);
			b += 1;
			for (size_t j = 0; j < d[i].rays.size(); j++) {
				//printf("RAY %f %f %f %f %f %f\n", d[i].rays[j].x, d[i].rays[j].y, d[i].rays[j].z, d[i].rays[j].a, d[i].rays[j].b, d[i].rays[j].c);
				memcpy(buf + b, &d[i].rays[j].x, 8);
				b += 8;
				memcpy(buf + b, &d[i].rays[j].y, 8);
				b += 8;
				memcpy(buf + b, &d[i].rays[j].z, 8);
				b += 8;
				memcpy(buf + b, &d[i].rays[j].a, 8);
				b += 8;
				memcpy(buf + b, &d[i].rays[j].b, 8);
				b += 8;
				memcpy(buf + b, &d[i].rays[j].c, 8);
				b += 8;
			}
		}


		pthread_mutex_lock(&cs->data_clients_mutex);
		for (std::vector<DataClient>::iterator dc = cs->data_clients.begin(); dc != cs->data_clients.end();) {
			if (!dc->initialized)
				continue;

			bool disconnect = false;
			size_t total = 0;
			ssize_t n_sent;
			while (total < buf_size) {
				n_sent = send(dc->socket, buf + total, buf_size - total, MSG_NOSIGNAL);

				if (n_sent <= 0) {
					disconnect = true;
					printf("[%lu] Data client disconnected\n", dc->id);
					break;
				}

				total += n_sent;
			}

			if (!disconnect)
				++dc;
			else
				dc = cs->data_clients.erase(dc);
		}
		pthread_mutex_unlock(&cs->data_clients_mutex);

		delete[] buf;
	}

	return NULL;
}

