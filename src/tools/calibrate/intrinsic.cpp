#include <iostream>
#include <cstring>
#include <ctime>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "camera.h"

using namespace std;
using namespace cam1394;
using namespace cv;

const Size chessboardSize(6, 8);

void generateChessboardPositions(Size boardSize, float squareSize, vector<Point3f>& corners)
{
    corners.clear();
    for(int i = 0; i < boardSize.height; ++i)
        for(int j = 0; j < boardSize.width; ++j)
            corners.push_back(Point3f(float(j*squareSize), float(i*squareSize), 0));
}

int main(int argc, char *argv[])
{
    char key = 0;
    Mat im;

    /* set up camera settings */    
    camera a;
    if (a.open("NONE", "640x480_MONO8", 30, "HQLINEAR", "GRBG") < 0)
        if (a.open("NONE", "1024x768_MONO8", 30, "HQLINEAR", "GRBG") < 0)
            return 0;

    //a.setTrigger(0);
    //a.setBrightness(0);
    //a.setGain(-1);
    //a.setExposure(0);
    //a.setShutter(-1);
    //a.setWhiteBalance(75, 32);

    Mat undistored;
    Mat cameraMatrix;
    Mat distCoeffs;
    vector< vector<Point2f> > imagePoints;    
    
    int time = 5;
    int saveIndex = 0;
    char outfile[255] = {0};
    while (key != 'q')
    {
        im = a.read();
        undistored = im.clone();
        Mat toSave = im.clone();

        vector<Point2f> pointBuf;
        bool chessboardFound = findChessboardCorners(im, chessboardSize, pointBuf, 
                        CV_CALIB_CB_ADAPTIVE_THRESH | 
                        CV_CALIB_CB_FAST_CHECK | 
                        CV_CALIB_CB_NORMALIZE_IMAGE); 
        
        //drawChessboardCorners(im, chessboardSize, Mat(pointBuf), chessboardFound);
        if (chessboardFound) {    
            Mat gray;
            cvtColor(im, gray, CV_BGR2GRAY);
            cornerSubPix(gray, pointBuf, Size(5, 5), Size(-1, -1), 
                         TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
            drawChessboardCorners(im, chessboardSize, Mat(pointBuf), chessboardFound);
        }

        if (!cameraMatrix.empty() && !distCoeffs.empty()) {
            undistort(im, undistored, cameraMatrix, distCoeffs);
            imshow("Undistorted", undistored);
        }
        imshow("Image", im);
        
        key = waitKey(time);

/*
        if (key == ' ') {
            if (time == 5) {
                time = 1000;
            } else {
                time = 5;
            }
        }
*/

        if (key == ' ' && chessboardFound) {
        // if (chessboardFound) {
            cout << "Chessboard Found" << endl;
            imagePoints.push_back(pointBuf);
	    sprintf(outfile, "%04d.tiff", saveIndex++);
	    imwrite(outfile, toSave);
        }

        if (key == 'c' && imagePoints.size() > 0) {
            vector< vector<Point3f> > objectPoints(1);
            vector<Mat> rvecs, tvecs;
            generateChessboardPositions(chessboardSize, 30, objectPoints[0]);
            objectPoints.resize(imagePoints.size(), objectPoints[0]);
            cameraMatrix = Mat::eye(3, 3, CV_64F);
            distCoeffs = Mat::zeros(8, 1, CV_64F);
            double rms = calibrateCamera(objectPoints, imagePoints, im.size(), cameraMatrix,
                                         distCoeffs, rvecs, tvecs, 0);
            
            cout << "Error: " << rms << endl;
            cout << cameraMatrix << endl;
            cout << distCoeffs << endl;
            FileStorage fs("camModel.yaml", FileStorage::WRITE);
            fs << "camera_matrix" << cameraMatrix;
            fs << "distortion_coefficients" << distCoeffs;
       }
    }

    return 0;
}
