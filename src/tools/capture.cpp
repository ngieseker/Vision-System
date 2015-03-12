#include <iostream>
#include <cstring>
#include <ctime>
#include <cassert>
#include "camera.h"

using namespace cam1394;
using namespace cv;
int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		std::cout << "usage: ./capture folder_name file_prefix" << std::endl;
		return -1;
	}

	char key = 0;
	Mat aimage;

	/* set up camera settings */	
	camera a;
	if (a.open("0007481202ED5E3B", "1024x768_MONO8", 30, "HQLINEAR", "GRBG") < 0)
		return 0;

	a.setTrigger(0);
	a.setBrightness(0);
	a.setGain(-1);
	a.setExposure(1000);
	a.setShutter(600);
	a.setWhiteBalance(75, 32);

	double old_ts = 0;
	
	char fn[100];
	int count = 1;
	while (key != 'q')
	{
		aimage = a.read();
		std::cout << a.getNumDroppedFrames() << " ";
		imshow("a", aimage);
		
		key = waitKey(5);
	
		/* save images in folder_name with file_prefix*/	
		if (key == 'w')
		{
			sprintf(fn, "%s/%s%i.tif", argv[1], argv[2], count);
			assert(imwrite(fn, aimage));
			std::cout << count << std::endl;
			count++;
		}
		
		double fps = 1/(1e-6*(a.getTimestamp()-old_ts));
		std::cout << fps << std::endl;
		old_ts = a.getTimestamp();
	}

	return 0;
}
