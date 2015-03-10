#include "ClusterComms.h"
#include "Configuration.h"
#include "common.h"
#include "network.h"
#include "camera.h"
#include "fxbcam.h"

#include "Timer.hpp"

using namespace cam1394;


int main(int argc, char *argv[]) {
	if (argc < 3 || argc > 4) {
		fprintf(stderr, "%s <hostname> <port> [shutter]\n", argv[0]);
		return 1;
	}

	int shutter = 600;
	if (argc == 4)
		shutter = atoi(argv[3]);

	double old_ts = 0;
	int under_ts = 0;

	/* Camera initialization */
	camera cam;
	if (cam.open("NONE", "1024x768_MONO8", 30, "HQLINEAR", "GRBG") < 0) {
		fprintf(stderr, "Error: Couldn't open camera\n");
		return 1;
	}

	Configuration config("config.json");
	CameraParams cp;
	if (!config.getCamera(cam.getGUID(), &cp)) {
		fprintf(stderr, "Error: Unconfigured camera\n");
		return 1;
	}

	cam.setTrigger(1);
	cam.setBrightness(0);
	cam.setGain(0);
	cam.setExposure(0);
	cam.setShutter(shutter);
	cam.setWhiteBalance(75, 32);

	/* Vision initialization */
	FXBcam vis;
	if (!vis.readCamPara(cp.calib.c_str()))
		return 1;
	vis.readMarkerData("marker.yaml");


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
	cl_init.set_cam_guid(cp.guid);
	msg_init.type = MSG_CLIENT_INIT;
	msg_init.size = cl_init.ByteSize();
	msg_init.data = new char[msg_init.size];
	cl_init.SerializeToArray(msg_init.data, msg_init.size);

	if (comms.write(&msg_init) != 1) {
		fprintf(stderr, "Error: couldn't send initialization message\n");
		return 1;
	}

	/* Start sending data */
#ifdef BENCHMARK
	Timer bench_t(CLOCK_THREAD_CPUTIME_ID);
#endif

	struct timespec timestamp;
	size_t frame_id = 0;
	std::vector<Marker> markers;
	std::vector<Ray> rays;

	Mat image, undistorted;

	size_t n_marks;
	int err = 1;
	char key = 0;
	do {
		image = cam.read();
		//undistort(image, undistorted, vis.intrinsic, vis.distCoeff);
		clock_gettime(CLOCK_REALTIME, &timestamp);

		frame_id += cam.getNumDroppedFrames();


		ClusterProto::Vision_Data vis_data;
#ifdef BENCHMARK
		bench_fxbcam bench;
		bench_t.start();
#endif

#ifndef BENCHMARK
		n_marks = vis.findMarkers(image, markers);
#else
	
		n_marks = vis.findMarkers(image, markers, bench);
#endif
#ifdef BENCHMARK
		bench_t.end();
		vis_data.set_bench_color_conversion (bench.color_conversion);
		vis_data.set_bench_thresholding     (bench.thresholding    );
		vis_data.set_bench_blob_detection   (bench.blob_detection  );
		vis_data.set_bench_marker_ident     (bench.marker_ident    );
		vis_data.set_bench_findmarkers_total(bench_t.elapsed());
#endif
		
		/* Find rays */
		if (n_marks > 0) {
#ifdef BENCHMARK
			bench_t.start();
#endif
			vis.castRays(markers, rays);


			/* Send markers to server */
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
#ifdef BENCHMARK
			bench_t.end();
			vis_data.set_bench_client_overhead(bench_t.elapsed());
#endif

			ClusterMessage message;
			message.type = MSG_VISION_DATA;
			message.size = vis_data.ByteSize();
			message.data = new char[message.size];
			vis_data.SerializeToArray(message.data, message.size);

			err = comms.write(&message);

			/* Send compressed jpeg to server */
#ifdef LOG_IMAGES
			char *jpeg_data;
			size_t jpeg_size = 0;
			encode_jpeg(image, (unsigned char **)&jpeg_data, &jpeg_size);

			ClusterProto::Image send_image;
			send_image.set_id(frame_id);
			send_image.set_time_sec(timestamp.tv_sec);
			send_image.set_time_nsec(timestamp.tv_nsec);
			send_image.set_image(jpeg_data, jpeg_size);
			delete jpeg_data;

			ClusterMessage image_msg;
			image_msg.type = MSG_IMAGE;
			image_msg.size = send_image.ByteSize();
			image_msg.data = new char[image_msg.size];
			send_image.SerializeToArray(image_msg.data, image_msg.size);

			comms.write(&image_msg);
#endif

			frame_id++;
		}
		//imshow("original", image);
		//imshow("undistorted", undistorted);
		//key = waitKey(5);

		double fps = 1/(1e-6*cam.getTimestamp()-old_ts);
		if (fps < 26)
			under_ts++;
		std::cout << "fps: " << fps << " under: " << under_ts-1 << std::endl;
		old_ts=1e-6*cam.getTimestamp();
	} while (err == 1 && key != 'q');
	

	//close(sockfd);

	return 0;
}
