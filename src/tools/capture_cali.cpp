#include "camera.h"
#include "Configuration.h"

#include <iostream>
#include <cstring>
#include <ctime>

using namespace cam1394;
using namespace cv;
int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cout << "usage: ./capture <calibration folder>" << std::endl;
		return 1;
	}

	/* Camera initialization */
	camera cam;
	if (cam.open("NONE", "1024x768_MONO8", 30, "HQLINEAR", "GRBG") < 0) {
		fprintf(stderr, "Error: Couldn't open camera\n");
		return 1;
	}

	Configuration config("config.json");
	CameraParams cp;
	if (!config.getCamera(cam.getGUID(), &cp)) {
		fprintf(stderr, "Error: Unconfigured camera with GUID: %ld\n", cam.getGUID());
		return 1;
	}

	char out_folder[128];
	sprintf(out_folder, "%s/cam%d", argv[1], cp.id);

	printf("Output to %s/\n", out_folder);

	cam.setBrightness(0);
	cam.setGain(700);
	cam.setExposure(0);
	cam.setShutter(30);
	cam.setWhiteBalance(74, 25);


	Mat image;

	double old_ts = 0;

	char key = 0;
	char fn[256];
	int count = 1;
	while (key != 'q')
	{
		image = cam.read();
		imshow("Capture", image);
		key = waitKey(5);
	
		if (key == 'e') // Store single extrinsic image
		{
			sprintf(fn, "%s/extrinsic.tif", out_folder);
			imwrite(fn, image);
			printf("Extrinsic\n");
		}
		if (key == 'i') // Store multiple intrinsic images
		{
			sprintf(fn, "%s/intrinsic%d.tif", out_folder, count);
			imwrite(fn, image);
			printf("Intrinsic %d\n", count);
			count++;
		}
		double fps = 1/(1e-6*(cam.getTimestamp()-old_ts));
		std::cout << fps << std::endl;
		old_ts = cam.getTimestamp();
	}

	return 0;
}
