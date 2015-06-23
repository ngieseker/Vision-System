#include <stdio.h>
#include <iostream>
#include <vector>
#include "fxbcam.h"
#include "Timer.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sstream>
#include "camera.h"
#include <string>
#include <map>
#include <cmath>

using namespace cam1394;
using namespace cv;

using namespace std;
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

// Takes in a vector of markers and finds the orientation of the object
double get_ori(vector<Marker>& mak) {
  int sz = mak.size();


  // Part 1: Find indices of markers
  
  // searches through mak to find red maker (whose color value is 2)
  int red = -1;
  for (int i = 0; i < sz; i++) {
    int c = mak[i].color;
    if (c == 2) {
      red = i;
      break;
    }
  }
  
  // if red is not found, exit
  if (red < 0) {
    cerr << "ERROR: NO RED MARKER\n";
    cerr << "SIZE: " << sz << '\n';
    exit(0);
  }	

  // sets an initial value for the max distance from red marker
  double x = (mak[red].x - mak[0].x);
  double y = (mak[red].y - mak[0].y);
  double max_dist = x*x + y*y;
  int idx = 0;
  
  // finds the marker across from the red, by finding marker with max distance
  // and sets it as the origin
  for (int i = 1; i < sz ; i++) {
    if (i == red)
      continue;
    x = (mak[red].x - mak[i].x);
    y = (mak[red].y - mak[i].y);
    double dist = x*x + y*y;
    if (max_dist < dist) {
      idx = i;
      max_dist = dist;
    }
	}

  
  //cerr << "ORIGIN:" << idx << '\n';
  //cerr << "DIRECTIONAL:" << red << '\n';


  //PART 2: Find degree

  //casts a vector from origin point to directional, and finds the direction
  // in (degrees) of the vector
  // **NOTE** Degrees chosen over radians for display purposes

  double degree = 0;
  double d = mak[red].x - mak[idx].x;
  double n = mak[red].y - mak[idx].y;
  int q = 0;
 

  // these ensure that n and d are exactly 0;
  if (d < .0001 && d > .0001)
      d =  0;
  if (n < .0001 && n > .0001)
    n = 0;


  // this finds the quadrant of the orientation, and adds the displacement
  // represented as 'q'
  if (d < .0001 && .0001 < n) {

    // degree is set to -90 here, and below because for the two quadrants
    // the angle is calculated wrong (i.e. 179, 180, 269, 268) the 
    // offset accounts for this
    degree = -90;
    q = 90;
    d = abs(d);
  }
  else if (n < .0001 && d < .0001) {
    q = 180;
    d = abs(d);
    n = abs(n);
  }
  else if (n < .0001 && .0001 < d) {

    // see above
    degree = -90;
    q = 270;
    n = abs(n);
  }

  // if x or y magnitude of vector is 0, then skip calculating degree, as
  // answer will be offset accounted for
  if (n < .0001 || d < .0001) {
      degree = 0;
  }
  // otherwise arctan of n/d
  else {
    degree += 180* atan(n/d) / 3.14159;
  }


  // to ignore a negative degree
  return abs(degree) + q;

}


int main(int, char **)
{

  //OLD VARIABLES
    char key = 0;

  Mat aimage;
  	
  /* double old_timestamp = 0;
  double avg = 0;
  double avgc = 0;
  int under30c = 0;*/
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
      cam.setShutter(100);
      cam.setWhiteBalance(74, 25);
  
  printf("Displaying: %s(%i) GUID: %016lX Box: %d\n", cams[i].vendor, cams[i].vendor_id, cams[i].guid, boxNum);
  printf("Press n to go to next camera\n");
  key = ' ';
  while (key != 'n')
    {      
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
	  if(vis.findMarkers(aimage, nothing)) {
	    double dir = get_ori(nothing);
	     cout << "Round:\n";
		while (!nothing.empty()) {
		  cout << "ID: " << nothing.back().id << '\n';

		  cout << "x: " << nothing.back().x << '\n';
		  cout << "y: " << nothing.back().y << '\n';

		  cout << "r: " << nothing.back().r << '\n';
		  cout << "0: " << nothing.back().theta << '\n';
		
		  cout << "color: " << nothing.back().color << '\n';
		  nothing.pop_back();
		}
		cout << "Degrees: " << dir << '\n';
		  cam.close();
		   exit(0);
	  }

	
	
	  t1.end();
#ifdef DEBUG	
	  imshow("a", aimage);
	  //	  key = waitKey(5);
#endif	
	  
	  /*	  avg += 1/(1e-6*cam.getTimestamp() - old_timestamp);
	  avgc++;
	  cout << std::setprecision(4) << std::fixed << "avg: " << avg/avgc << " ";
	  cout << std::setprecision(5) << std::fixed << "fps: " << 1/(1e-6*cam.getTimestamp() - old_timestamp);
	  if (1/(1e-6*cam.getTimestamp() - old_timestamp) < 20)
	    under30c++;
	  cout << " under 30: " << under30c << " findMarkerTimer: " << t1.elapsed() << endl;
	  old_timestamp = 1e-6*cam.getTimestamp();*/
	}
      
      //END INSERTION
    
      key = waitKey(5);
    }
  
  cam.close();
  }

	//if (a.open("NONE", "1024x768_MONO8", 30, "HQLINEAR", "GRBG") < 0)
	//return 0;
  return 0;
	
	return 0;
}
