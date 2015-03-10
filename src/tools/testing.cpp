#include <iostream>
#include <cstring>
#include <ctime>
#include <map>
#include "camera.h"

using namespace cam1394;
using namespace cv;

#define PTGREY 45213
#define IMAGINGSOURCE 1864
static const std::map<uint64_t, int> cam_map = 
{
    //Flea3 GUID
    {0x00B09D0100BAE98B, 1},
    {0x00B09D0100BAE991, 2},
    {0x00B09D0100BAE98F, 3},
    {0x00B09D0100BAE98A, 4},
    {0x00B09D0100BAE985, 5},
    {0x00B09D0100BAE98E, 6},
    {0x00B09D0100BAE999, 7},
    {0x00B09D0100BAE97D, 8},
    // ImagingSource GUID
    {0x000748120254C6AB, 1},
    {0x0007481202AECEB6, 2},
    {0x0007481202ED5E3B, 3}, 
    {0x0007481202ED5E3C, 4},
    {0x0007481202ED5E3D, 5},
    {0x0007481202ED5E3E, 6},
    {0x0007481202ED5E3F, 7},
    {0x0007481202ED5E40, 8},
};

int main(int argc, char *argv[])
{
	char key = 0;
	Mat aimage;
    char openGUID[255] = {0};
	/* set up camera settings */	
	camera a;

    vector<camera_info> cams = a.getConnectedCameras();
    if (cams.size() == 0) {
        printf("ERROR: No cameras are connected\n");
        return -1;
    } else if (cams.size() < 2) {
        printf("ERROR: Only 1 camera is connected: %s\n", cams[0].vendor);
        return -1;
    } else if (cams.size() > 2) {
        printf("ERROR: More than 2 cameras are connected!\n");
        return -1;
    }

    int numOfPTGrey = 0;
    int numOfIS = 0;
    int numOfOther = 0;
    int ISBoxNum = 0;
    int PTGreyBoxNum = 0;
    for (size_t i = 0; i < cams.size(); i++) {
        int boxNum = 0;
        if (cam_map.find(cams[i].guid) != cam_map.end()) {
            boxNum = cam_map.at(cams[i].guid);
        } else {
            printf("ERROR: GUID of Camera is not one of the 16 cameras\n");
            printf("ERROR: %s(%i) GUID: %016lX\n", cams[i].vendor, cams[i].vendor_id, cams[i].guid);
            return 0;
        }

        if (cams[i].vendor_id == PTGREY) {
            PTGreyBoxNum = boxNum; 
            numOfPTGrey++;
        } else if (cams[i].vendor_id == IMAGINGSOURCE) {
            ISBoxNum = boxNum; 
            numOfIS++;
        } else {
            numOfOther++;
        }

        // to be removed
        // printf("%s(%i) GUID: %016lX Box: %d\n", cams[i].vendor, cams[i].vendor_id, cams[i].guid, boxNum);
    }
   
    if (PTGreyBoxNum != ISBoxNum || PTGreyBoxNum == 0 || ISBoxNum == 0) {
        printf("ERROR: mismatched cameras, ImagingSource Box #: %d, PTGrey Box #: %d\n", PTGreyBoxNum, ISBoxNum); 
        return 0;
    }
    
    if (numOfOther != 0) {
        printf("ERROR: Got a non Imaging Source or Point Grey Camera\n");
        return 0;
    }

    if (numOfPTGrey != 1 && numOfPTGrey != numOfIS) {
        printf("ERROR: Didn't get 1 of each camera\n");
        return 0;
    }
    a.close();

    for (size_t i = 0; i < cams.size(); i++) {
        int boxNum = cam_map.at(cams[i].guid);

        camera cam;
        sprintf(openGUID, "%016lX", cams[i].guid);
        if (cams[i].vendor_id == PTGREY) {
            if (cam.open(openGUID, "640x480_MONO8", 30, "HQLINEAR", "GRBG") < 0)
                return 0;
        } else if (cams[i].vendor_id == IMAGINGSOURCE) {
            if (cam.open(openGUID, "1024x768_MONO8", 30, "HQLINEAR", "GRBG") < 0)
                return 0;
        } else {
            continue;
        }
        
        printf("Displaying: %s(%i) GUID: %016lX Box: %d\n", cams[i].vendor, cams[i].vendor_id, cams[i].guid, boxNum);
        printf("Press n to go to next camera\n");
        key = ' ';
        while (key != 'n')
        {
            aimage = cam.read();
            imshow("image", aimage);
            key = waitKey(5);
        }

        cam.close();
    }

    printf("Done!\n");
	return 0;
}
