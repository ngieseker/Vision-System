#include <iostream>
#include <cstring>
#include <ctime>
#include <sstream>
#include "camera.h"
#include "fxbcam.h"

using namespace cam1394;
using namespace cv;
int main(int argc, char *argv[])
{
	if (argc > 2) {
		fprintf(stderr, "%s [shutter]\n", argv[0]);
		return 1;
	}

	int shutter = 600;
	if (argc == 2)
		shutter = atoi(argv[3]);

	char key = 0;

	Mat aimage;
	Mat hsv;
	Mat bgr;
	Mat bin;

	/* initialize windows and trackbars */
	namedWindow("a", CV_WINDOW_AUTOSIZE);
	namedWindow("t", 1);
	int h1m = 0;//84;
	int s1m = 0;//125;
	int v1m = 0;//154;
	int h1M = 255;//115;
	int s1M = 255;//255;
	int v1M = 255;//255;

	createTrackbar("h1m", "t", &h1m, 255, NULL);
	createTrackbar("s1m", "t", &s1m, 255, NULL);
	createTrackbar("v1m", "t", &v1m, 255, NULL);
	createTrackbar("h1M", "t", &h1M, 255, NULL);
	createTrackbar("s1M", "t", &s1M, 255, NULL);
	createTrackbar("v1M", "t", &v1M, 255, NULL);

#ifdef RGBMARKER
	namedWindow("bgrt", 1);
	int b1m = 0;//84;
	int g1m = 0;//125;
	int r1m = 0;//154;
	int b1M = 255;//115;
	int g1M = 255;//255;
	int r1M = 255;//255;

	createTrackbar("bm", "bgrt", &b1m, 255, NULL);
	createTrackbar("gm", "bgrt", &g1m, 255, NULL);
	createTrackbar("rm", "bgrt", &r1m, 255, NULL);
	createTrackbar("bM", "bgrt", &b1M, 255, NULL);
	createTrackbar("gM", "bgrt", &g1M, 255, NULL);
	createTrackbar("rM", "bgrt", &r1M, 255, NULL);
#endif
	/* initialize camera */
	camera a;
	if (a.open("NONE", "1024x768_MONO8", 30, "HQLINEAR", "GRBG") < 0)
		return 0;
	a.setBrightness(0);
	a.setGain(0);
	a.setExposure(0);
	a.setShutter(shutter);
	a.setWhiteBalance(75, 32);

	/* Used for writing images
	std::string s = "images/";
	std::stringstream out;
	int temp = time(NULL);
	out << s << temp;
	*/

	FileStorage fs;

	int count = 0;
	while (key != 'q')
	{
		aimage = a.read();

#ifdef RGBMARKER
		inRange(aimage, Scalar(b1m, g1m, r1m, 0), Scalar(b1M, g1M, r1M, 0), bgr);
#endif

		cvtColor(aimage, hsv, CV_BGR2HSV);

		inRange(hsv, Scalar(h1m, s1m, v1m, 0), Scalar(h1M, s1M, v1M, 0), bin);
		
		dilate(bin, bin, Mat(), Point(1,-1), 2);
		//erode(bin, bin, Mat(), Point(1,-1), 3);

#ifdef RGBMARKER
		imshow("bgr", bgr);
#endif
		imshow("h", hsv);
		imshow("a", aimage);
		imshow("b", bin);
		key = waitKey(5);

		/* Used for writing images
		if (key == 'w')
		{
			s = out.str() + "bin.png";
			imwrite(s, bin);
			s = out.str() + "aimage.png";
			imwrite(s, aimage);
		}*/

		if (key == ' ')
		{
			if (!fs.isOpened())
			{
				fs.open("marker.yaml", FileStorage::WRITE);
			}
			Scalar s = Scalar(h1m, s1m, v1m, 0);
			std::stringstream tempss;
			tempss << "marker_min" << count;
			fs << tempss.str() << s;
			tempss.str("");
			s = Scalar(h1M, s1M, v1M, 0);
			tempss << "marker_max" << count;
			fs << tempss.str() << s;
			std::cout << "saved marker" << std::endl;
			count++;
		}
	}
	if (fs.isOpened())
		fs.release();

	FXBcam b;
	b.readMarkerData("marker.yaml");
	std::cout << "done" << std::endl;
	return 0;
}
