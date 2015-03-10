#include <iostream>
#include <cstring>
#include <ctime>
#include <sstream>
#include "camera.h"
#include "fxbcam.h"
#include "Timer.hpp"

using namespace cam1394;
using namespace cv;
using namespace std;

int main(int, char **)
{
	char key = 0;

	Mat aimage;
	
	double old_timestamp = 0;
	double avg = 0;
	double avgc = 0;
	int under30c = 0;

	/* initialize camera */
	camera a;
	if (a.open("NONE", "1024x768_MONO8", 30, "HQLINEAR", "GRBG") < 0)
		return 0;
	a.setBrightness(0);
	a.setGain(0);
	a.setExposure(0);
	a.setShutter(600);
	a.setWhiteBalance(74, 25);

	FXBcam vis;
	
	if (!vis.readCamPara("camModel.yaml"))
	{
		return 1;
	}
	if (!vis.readMarkerData("marker.yaml"))
	{
		return 1;
	}

	std::vector<Marker> nothing;
		
	key = waitKey(5);
	
	while (key != 'q')
	{
		aimage = a.read();
		//cvtColor(aimage, hsv, CV_BGR2HSV);
		Timer t1;
		t1.start();
		vis.findMarkers(aimage, nothing);
		t1.end();
#ifdef DEBUG	
		imshow("a", aimage);
		key = waitKey(5);
#endif	

		avg += 1/(1e-6*a.getTimestamp() - old_timestamp);
		avgc++;
		cout << std::setprecision(4) << std::fixed << "avg: " << avg/avgc << " ";
		cout << std::setprecision(5) << std::fixed << "fps: " << 1/(1e-6*a.getTimestamp() - old_timestamp);
		if (1/(1e-6*a.getTimestamp() - old_timestamp) < 20)
			under30c++;
		cout << " under 30: " << under30c << " findMarkerTimer: " << t1.elapsed() << endl;
		old_timestamp = 1e-6*a.getTimestamp();
	}
	
	return 0;
}
