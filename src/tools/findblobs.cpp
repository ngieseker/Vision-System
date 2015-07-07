#include <iostream>
#include <cstring>
#include <ctime>
#include <sstream>
#include "camera.h"
#include "fxbcam.h"
#include "Timer.hpp"
#include <map>

using namespace cam1394;
using namespace cv;
using namespace std;



#define multipleobjects
#define PTGREY 45213
#define IMAGINGSOURCE 1864
void init_map(std::map<uint64_t, int>& m)
{
    //Flea3 GUID
  m[0x00B09D0100BAE98B] = 1;
  m[0x00B09D0100BAE991] = 2;
  m[0x00B09D0100BAE98F] = 3;
  m[0x00B09D0100BAE98A] = 4;
  m[0x00B09D0100BAE985] = 5;
  m[0x00B09D0100BAE98E] = 6;
  m[0x00B09D0100BAE999] = 7;
  m[0x00B09D0100BAE97D] = 8;
    // ImagingSource GUID
  m[0x000748120254C6AB] = 1;
  m[0x0007481202AECEB6] = 2;
  m[0x0007481202ED5E3B] = 3; 
  m[0x0007481202ED5E3C] = 4;
  m[0x0007481202ED5E3D] = 5;
  m[0x0007481202ED5E3E] = 6;
  m[0x0007481202ED5E3F] = 7;
  m[0x0007481202ED5E40] = 8;

}

int main(int, char **)
{

  //OLD VARIABLES
    char key = 0;

  Mat aimage;
  	
  double old_timestamp = 0;
  double avg = 0;
  double avgc = 0;
  int under30c = 0;
   //END



  //initializes map with GUID values
  std::map<uint64_t, int> cam_map;
  init_map(cam_map);
  
  char openGUID[255] = {0};


  
  	/* initialize camera */
  camera a;

  vector<camera_info> cams = a.getConnectedCameras();
  if (cams.size() == 0) {
        printf("ERROR: No cameras are connected\n");
        return -1;
  }
  a.close();

    
  for (size_t i = 0; i < cams.size(); i++) {
    int boxNum = cam_map.at(cams[i].guid);
    camera cam;
    
    sprintf(openGUID, "%016lX", cams[i].guid);
    if (cams[i].vendor_id == PTGREY) {
      if (cam.open(openGUID, "640x480_MONO8", 30, "HQLINEAR", "GRBG") < 0)
	return 0;
      cout << "ptgrey\n";
    } else if (cams[i].vendor_id == IMAGINGSOURCE) {
      if (cam.open(openGUID, "1024x768_MONO8", 30, "HQLINEAR", "GRBG") < 0)
	return 0;
    } else {
      continue;
    }

      cam.setBrightness(0);
      cam.setGain(0);
      cam.setExposure(0);
      cam.setShutter(600);
      cam.setWhiteBalance(74, 25);
  
  printf("Displaying: %s(%i) GUID: %016lX Box: %d\n", cams[i].vendor, cams[i].vendor_id, cams[i].guid, boxNum);
  printf("Press n to go to next camera\n");
  key = ' ';
  while (key != 'n')
    {
      /*      aimage = cam.read();
	      imshow("image", aimage);*/
      
      //OLD CODE INSERTED HERE

      FXBcam vis;
	
      if (!vis.readCamPara("camModel.yaml")) {
	return 1;
      }
      if (!vis.readMarkerData("marker.yaml")) {
	return 1;
      }

      std::vector<Marker> nothing;
      
      key = waitKey(5);
      
      while (key != 'q')
	{
	  aimage = cam.read();
	  //cvtColor(aimage, hsv, CV_BGR2HSV);
	  Timer t1;
	  t1.start();

    vis.organizeMarkers(aimage, nothing);

#ifdef multipleobjects
    std::vector<Object> objects;
    vis.findMultipleMarkers(nothing, objects);
#else
	  vis.findSingleMarker(nothing);
#endif
	  t1.end();
#ifdef DEBUG	
	  imshow("a", aimage);
	  //	  key = waitKey(5);
#endif	
	  
	  avg += 1/(1e-6*cam.getTimestamp() - old_timestamp);
	  avgc++;
	  cout << std::setprecision(4) << std::fixed << "avg: " << avg/avgc << " ";
	  cout << std::setprecision(5) << std::fixed << "fps: " << 1/(1e-6*cam.getTimestamp() - old_timestamp);
	  if (1/(1e-6*cam.getTimestamp() - old_timestamp) < 20)
	    under30c++;
	  cout << " under 30: " << under30c << " findMarkerTimer: " << t1.elapsed() << endl;
	  old_timestamp = 1e-6*cam.getTimestamp();
	}
      
      //END INSERTION
        
      
      key = waitKey(5);
    }
  
  cam.close();
  }

	//if (a.open("NONE", "1024x768_MONO8", 30, "HQLINEAR", "GRBG") < 0)
	//return 0;
	
	return 0;
}
