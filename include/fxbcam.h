#ifndef FXBCAM_H
#define FXBCAM_H

#include "camera.h"
#include "hash.h"
#include "cv.h"
#include "object.h"

#include <fstream>
#include <vector>
#include "jpeglib.h"

using namespace cv;
using namespace cam1394;

struct Blob
{
	double x;
	double y;
	double area;

	Blob(int x, int y, int area) : 
		x(x), y(y), area(area) {}

	bool operator<(const Blob &rhs) const {
		return area > rhs.area;
	}
};
/*
struct Marker {
	double x;
	double y;
	double angle;

	double r;
	double theta;

	size_t id;
	size_t color;

	Marker(int x, int y, int color) : 
		x(x), y(y), color(color) {}
};
*/
struct Ray {
	/* Direction */
	double a;
	double b;
	double c;

	/* Origin */
	double x;
	double y;
	double z;

	size_t client_id;
	size_t id;
	size_t marker;

	size_t time_sec;
	size_t time_nsec;

#ifdef BENCHMARK
	double bench_color_conversion;
	double bench_thresholding;
	double bench_blob_detection;
	double bench_marker_ident;
	double bench_findmarkers_total;
	double bench_client_overhead;
#endif

	Ray() {}
	Ray(size_t id, double a, double b, double c, size_t marker) :
		a(a), b(b), c(c), id(id), marker(marker) {}

	void setOrigin(Mat &o) {
		x = o.at<double>(0,0);
		y = o.at<double>(1,0);
		z = o.at<double>(2,0);
	}

	bool operator==(const Ray& rhs) const {
		return id == rhs.id &&
			   client_id == rhs.client_id &&
			   marker == rhs.marker;
	}

	bool operator<(const Ray& rhs) const {
		if (client_id < rhs.client_id)
			return true;
		else if (client_id == rhs.client_id) {
			if (id < rhs.id)
				return true;
			else if (id == rhs.id) {
				if (marker < rhs.marker)
					return true;
			}
		}

		return false;
	}
};

template<>
struct hash<Ray> {
	size_t operator()(const Ray& k) const {
		size_t key[3] = {k.client_id, k.id, k.marker};
		return hash_bytes(key, sizeof(key)*sizeof(size_t));
	}
};



struct LED {
	size_t id;
	size_t color;
};


struct Vec3 {
	double x;
	double y;
	double z;

	Vec3() {}
	Vec3(double x, double y, double z) :
		x(x), y(y), z(z) {}
};


#ifdef BENCHMARK
struct bench_fxbcam {
	double camera_read;
	double color_conversion;
	double thresholding;
	double blob_detection;
	double marker_ident;
	double findmarkers_total;
	double client_overhead;
	double find_rays;
	double client_total;

	bench_fxbcam() :
	    camera_read(0), color_conversion(0), thresholding(0), blob_detection(0), marker_ident(0),
	    findmarkers_total(0), client_overhead(0), find_rays(0), client_total(0) {}
};
#endif


void encode_jpeg(cv::Mat src, unsigned char **dest, size_t *size);
void decode_jpeg(cv::Mat &dest, unsigned char *src, size_t size);


class FXBcam
{
	public:
		FXBcam();
		~FXBcam();
		
		bool readCamPara(const char *filename);
		bool readMarkerData(const char *filename);

#ifndef BENCHMARK
		int findBlobs(Mat, int, std::vector<Blob>&, double = -1, CvRect = cvRect(0,0,0,0));
		int findMarkers(Mat, std::vector<Marker>&);
#else
		int findBlobs(Mat, int, std::vector<Blob>&, bench_fxbcam&, double = -1, CvRect = cvRect(0,0,0,0));
		int findMarkers(Mat, std::vector<Marker>&, bench_fxbcam&);
#endif

		void castRays(std::vector<Marker>&, std::vector<Ray>&);
		Vec3 intersectRays(std::vector<Ray>&);
			
		Mat intrinsic;
		Mat	distCoeff;
		Mat Rc_ext;
		Mat Tc_ext;

		Mat intrinsic_inv;
		Mat Rc_ext_inv;

		Mat cam_pos;

		unsigned char *rgb_threshold;

	private:
		vector<Scalar> markers_min;
		vector<Scalar> markers_max;
		size_t numMarker;

		Scalar getMarkerMin(size_t);
		Scalar getMarkerMax(size_t);
};
#endif
