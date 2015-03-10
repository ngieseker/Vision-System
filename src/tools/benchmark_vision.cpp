#include "ClusterComms.h"
#include "Configuration.h"
#include "common.h"
#include "network.h"
#include "camera.h"
#include "fxbcam.h"

#include "Timer.hpp"

using namespace cam1394;


int main(int argc, char *argv[]) {
	int shutter = 540;
	if (argc == 2)
		shutter = atoi(argv[1]);

	/* Camera initialization */
	camera cam;
	if (cam.open("00b09d0100af05c1", "640x480_MONO8", 60, "SIMPLE", "BGGR") < 0) {
	//if (cam.open("NONE", "1024x768_MONO8", 30,  NULL, NULL) < 0) {
		fprintf(stderr, "Error: Couldn't open camera\n");
		return 1;
	}

	/*
	Configuration config("config.json");
	CameraParams cp;
	if (!config.getCamera(cam.getGUID(), &cp)) {
		fprintf(stderr, "Error: Unconfigured camera\n");
		return 1;
	}
	*/

	cam.setTrigger(0);
	cam.setBrightness(0);
	cam.setGain(2500);
	cam.setExposure(0);
	cam.setShutter(shutter);
	cam.setWhiteBalance(774, 480);

	
	/* Vision initialization */
	FXBcam vis;
	//if (!vis.readCamPara(cp.calib.c_str()))
	//	return 1;
	vis.readMarkerData("marker.yaml");


	//Timer bench_t(CLOCK_REALTIME);
	//Timer bench_loop(CLOCK_REALTIME);
	Timer bench_t(CLOCK_THREAD_CPUTIME_ID);
	Timer bench_loop(CLOCK_THREAD_CPUTIME_ID);

	int under_ts = 0;
	struct timespec prev_timestamp, timestamp;
	size_t frame_id = 0;
	std::vector<Marker> markers;
	std::vector<Ray> rays;

	Mat image, undistorted;

	clock_gettime(CLOCK_REALTIME, &timestamp);

	size_t n_marks;
	int err = 1;
	do {
		bench_fxbcam bench;
		bench_loop.start();

		bench_t.start();

		prev_timestamp = timestamp;
		image = cam.read();
		clock_gettime(CLOCK_REALTIME, &timestamp);
		if (image.empty()) {
			std::cerr << "Image read error." << std::endl;
			break;
		}

		frame_id += cam.getNumDroppedFrames();

		bench_t.end();
		bench.camera_read = bench_t.elapsed();

		ClusterProto::Vision_Data vis_data;

		bench_t.start();

		n_marks = vis.findMarkers(image, markers, bench);

		bench_t.end();
		bench.findmarkers_total = bench_t.elapsed();

		//imshow("bahl", image);
		//waitKey(5);
		goto skip;
		
		bench_t.start();

		/* Find rays */
		if (n_marks > 0) {

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
			}

			ClusterMessage message;
			message.type = MSG_VISION_DATA;
			message.size = vis_data.ByteSize();
			message.data = new char[message.size];
			vis_data.SerializeToArray(message.data, message.size);

			frame_id++;
		}

		bench_t.end();
		bench.find_rays = bench_t.elapsed();

skip:

		bench_loop.end();
		bench.client_total = bench_loop.elapsed();


		printf("\n===============================\n");
		printf("Camera Read:      %15.6lfms\n", 1000*bench.camera_read);
		printf("Color conversion: %15.6lfms\n", 1000*bench.color_conversion);
		printf("Thresholding:     %15.6lfms\n", 1000*bench.thresholding);
		printf("Blob Detection:   %15.6lfms\n", 1000*bench.blob_detection);
		printf("Marker Ident:     %15.6lfms\n", 1000*bench.marker_ident);
		printf("Find Rays:        %15.6lfms\n", 1000*bench.find_rays);
		printf("                          ---------\n");
		printf("Findmarkers:      %15.6lfms\n", 1000*bench.findmarkers_total);
		printf("Client Total:     %15.6lfms\n", 1000*bench.client_total);
		printf("\n");

		double fps = 1/elapsed(&prev_timestamp, &timestamp);
		if (fps < 56)
			under_ts++;
		std::cout << "fps: " << fps << " under: " << under_ts-1 << std::endl;
	} while (err == 1);
	

	//close(sockfd);

	return 0;
}
