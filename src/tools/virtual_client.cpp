#include "ClusterComms.h"
#include "Configuration.h"
#include "common.h"
#include "network.h"
#include "camera.h"
#include "fxbcam.h"

using namespace cam1394;


int main(int argc, char *argv[]) {
	if (argc != 4) {
		fprintf(stderr, "%s <hostname> <port> <guid>\n", argv[0]);
		return 1;
	}


	/* Network initialization */
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	struct addrinfo serv;
	socket_t sockfd;
	if (!socket_open(&sockfd, argv[1], argv[2], &hints, &serv, 0))
		return 1;

	char s[INET6_ADDRSTRLEN];
	get_ip((struct sockaddr_storage *)serv.ai_addr, s, sizeof(s));
	printf("client: connecting to %s\n", s);

	ClusterComms comms(sockfd);

	/* Send initialization message to server */
	ClusterMessage msg_init;
	ClusterProto::Client_Init cl_init;
	cl_init.set_cam_guid(atol(argv[3]));
	msg_init.type = MSG_CLIENT_INIT;
	msg_init.size = cl_init.ByteSize();
	msg_init.data = new char[msg_init.size];
	cl_init.SerializeToArray(msg_init.data, msg_init.size);

	if (comms.write(&msg_init) != 1) {
		fprintf(stderr, "Error: couldn't send initialization message\n");
		return 1;
	}

	/* Start sending data */

	struct timespec timestamp;
	size_t frame_id = 0;

	clock_gettime(CLOCK_REALTIME, &timestamp);
	srand(timestamp.tv_nsec);

	int err = 1;
	char key = 0;
	do {
		usleep(30000);
		clock_gettime(CLOCK_REALTIME, &timestamp);

		std::vector<Ray> rays;
		Mat ray = Mat(3, 1, CV_64FC1);
		for (int j = 0; j < 4; j++) {
			ray.at<double>(0,0) = rand() - RAND_MAX/2;
			ray.at<double>(1,0) = rand() - RAND_MAX/2;
			ray.at<double>(2,0) = rand() - RAND_MAX/2;
			ray = ray / norm(ray);

			Ray a(1, ray.at<double>(0,0), ray.at<double>(1,0), ray.at<double>(2,0), j);
			rays.push_back(a);
		}

		/* Send markers to server */
		ClusterProto::Vision_Data vis_data;
		vis_data.set_id(frame_id);
		vis_data.set_time_sec(timestamp.tv_sec);
		vis_data.set_time_nsec(timestamp.tv_nsec);

		for (size_t i = 0; i < rays.size(); i++) {
			ClusterProto::Vision_Data_Ray *ray = vis_data.add_ray();

			ray->set_x(rays[i].a);
			ray->set_y(rays[i].b);
			ray->set_z(rays[i].c);
			ray->set_marker(rays[i].marker);

			//circle(image, Point(blobs[i].x, blobs[i].y), 4, Scalar(0, 0, 255));
			//circle(undistorted, Point(markers[i].x, markers[i].y), 4, Scalar(0, 0, 255));
		}

		ClusterMessage message;
		message.type = MSG_VISION_DATA;
		message.size = vis_data.ByteSize();
		message.data = new char[message.size];
		vis_data.SerializeToArray(message.data, message.size);

		err = comms.write(&message);

	} while (err == 1 && key != 'q');
	
	return 0;
}
