#include <fstream>
#include <iomanip>
#include <utility>
#include <cmath>
#include "fxbcam.h"
#include "Timer.hpp"

#include <cmath>
#include <algorithm>

using namespace cv;

void encode_jpeg(cv::Mat src, unsigned char **dest, size_t *size, int quality=80) {
	jpeg_compress_struct jpeg_cinfo;
	jpeg_error_mgr jpeg_err;
	jpeg_cinfo.err = jpeg_std_error(&jpeg_err);
	jpeg_create_compress(&jpeg_cinfo);
	
	jpeg_mem_dest(&jpeg_cinfo, dest, size);
	jpeg_cinfo.image_width      = src.size().width;
	jpeg_cinfo.image_height     = src.size().height;
	jpeg_cinfo.input_components = src.channels();
	jpeg_cinfo.in_color_space   = JCS_EXT_BGR;

	jpeg_set_defaults(&jpeg_cinfo);
	jpeg_set_quality(&jpeg_cinfo, quality, TRUE);

	jpeg_start_compress(&jpeg_cinfo, TRUE);

	JSAMPROW row_ptr[1];
	size_t stride = jpeg_cinfo.image_width * 3;
	while (jpeg_cinfo.next_scanline < jpeg_cinfo.image_height) {
		row_ptr[0] = (JSAMPLE *)(src.data + jpeg_cinfo.next_scanline*stride);
		jpeg_write_scanlines(&jpeg_cinfo, row_ptr, 1);
	}

	jpeg_finish_compress(&jpeg_cinfo);
	jpeg_destroy_compress(&jpeg_cinfo);
}

void decode_jpeg(cv::Mat &dest, unsigned char *src, size_t size) {
	jpeg_decompress_struct jpeg_cinfo;
	jpeg_error_mgr jpeg_err;
	jpeg_cinfo.err = jpeg_std_error(&jpeg_err);
	jpeg_create_decompress(&jpeg_cinfo);
	
	jpeg_mem_src(&jpeg_cinfo, src, size);
	jpeg_read_header(&jpeg_cinfo, TRUE);

	jpeg_cinfo.out_color_space = JCS_EXT_BGR;

	jpeg_start_decompress(&jpeg_cinfo);

	dest.create(jpeg_cinfo.output_height, jpeg_cinfo.output_width, CV_8UC3);
	//jpeg_cinfo.output_components;

	JSAMPROW row_ptr[1];
	size_t stride = jpeg_cinfo.output_width * 3;
	while (jpeg_cinfo.output_scanline < jpeg_cinfo.output_height) {
		row_ptr[0] = (JSAMPLE *)(dest.data + jpeg_cinfo.output_scanline*stride);
		jpeg_read_scanlines(&jpeg_cinfo, row_ptr, 1);
	}

	jpeg_finish_decompress(&jpeg_cinfo);
	jpeg_destroy_decompress(&jpeg_cinfo);
}



void inRange_table(InputArray _src, OutputArray _dst, unsigned char *table, size_t marker) {
	if (!table) {
		std::cerr << "inRange_table: Thresholding lookup table not initialized." << std::endl;
		exit(1);
	}

	Mat src = _src.getMat();

	if (src.channels() != 3) {
		std::cerr << "inRange_table: input does not have 3 channels." << std::endl;
		exit(1);
	}

	_dst.create(src.dims, src.size, CV_8U);
	Mat dst = _dst.getMat();

	const uchar* sp = src.data;
	uchar* dp = dst.data;

	size_t sz = dst.rows * dst.cols;
	for (; sz--; sp += 3, dp += 1) {
		*dp = (uchar)-( (table[sp[0] + 256 * sp[1] + 256 * 256 * sp[2]] & (unsigned char)(1 << marker)) != 0 );
	}
}

void inRange_table_v(InputArray _src, OutputArrayOfArrays _dst, const size_t *markers, size_t num_markers, unsigned char *table) {
	if (!table) {
		std::cerr << "inRange_table: Thresholding lookup table not initialized." << std::endl;
		exit(1);
	}

	Mat src = _src.getMat();

	if (src.channels() != 3) {
		std::cerr << "inRange_table_v: input does not have 3 channels." << std::endl;
		exit(1);
	}

	if (num_markers <= 0 || num_markers > 8) {
		std::cerr << "inRange_table_v: no markers or too many markers." << std::endl;
		exit(1);
	}

	uchar *dst[8];

	for (size_t i = 0; i < num_markers; i++) {
		_dst.create(src.dims, src.size, CV_8U, i);
		dst[i] = _dst.getMat(i).data;
	}

	const uchar* sp = src.data;

	unsigned char v;
	size_t sz = src.rows * src.cols;
	for (; sz--; sp += 3) {
		v = table[sp[0] + 256 * sp[1] + 256 * 256 * sp[2]];

		/*
		switch (num_markers) {
			case 8:
				*dst[7]++ = (uchar)-( (v & (1 << markers[7])) != 0 );
			case 7:
				*dst[6]++ = (uchar)-( (v & (1 << markers[6])) != 0 );
			case 6:
				*dst[5]++ = (uchar)-( (v & (1 << markers[5])) != 0 );
			case 5:
				*dst[4]++ = (uchar)-( (v & (1 << markers[4])) != 0 );
			case 4:
				*dst[3]++ = (uchar)-( (v & (1 << markers[3])) != 0 );
			case 3:
				*dst[2]++ = (uchar)-( (v & (1 << markers[2])) != 0 );
			case 2:
				*dst[1]++ = (uchar)-( (v & (1 << markers[1])) != 0 );
			case 1:
				*dst[0]++ = (uchar)-( (v & (1 << markers[0])) != 0 );
		}
		*/

		for (size_t i = 0; i < num_markers; i++) {
			*dst[i]++ = (uchar)-( (v & (1 << markers[i])) != 0 );
		}
	}
}
				


FXBcam::FXBcam() : rgb_threshold(NULL) {}

FXBcam::~FXBcam() {
	if (rgb_threshold)
		delete[] rgb_threshold;
}

bool FXBcam::readCamPara(const char *filename)
{
	FileStorage fs;

	fs.open(filename, FileStorage::READ);
	if (!fs.isOpened())
	{
		std::cerr << "ERROR: Camera Calibration file not found" << std::endl;
		return false;
	}

	fs["intrinsic"] >> intrinsic;
	fs["distCoeff"] >> distCoeff;
	fs["Rc_ext"]    >> Rc_ext;
	fs["Tc_ext"]    >> Tc_ext;

	bool allParamAval = true;

	if (intrinsic.empty())
	{
		std::cerr << "ERROR: Camera Calibration file doesn't contain the intrinsic parameters" << std::endl;
		allParamAval = false;
	}

	if (distCoeff.empty())
	{
		std::cerr << "ERROR: Camera Calibration file doesn't contain the distortion coefficients" << std::endl;
		allParamAval = false;
	}
	
	if (Rc_ext.empty())
	{
		std::cerr << "ERROR: Camera Calibration file doesn't contain the extrinsic rotational matrix" << std::endl;
		allParamAval = false;
	}
	
	if (Tc_ext.empty())
	{
		std::cerr << "ERROR: Camera Calibration file doesn't contain the extrinsic translationi matrix" << std::endl;
		allParamAval = false;
	}

	if (!allParamAval)
		return false;

	Rc_ext_inv = Rc_ext.inv(DECOMP_SVD);
	intrinsic_inv = intrinsic.inv(DECOMP_SVD);

	cam_pos = -1.0 * Rc_ext_inv * Tc_ext;

	fs.release();

	return true;
}

bool FXBcam::readMarkerData(const char *filename)
{
	FileStorage fs;
	fs.open(filename, FileStorage::READ);
	
	if (!fs.isOpened())
	{
		std::cout << "WARNING: Marker data file not found\n"
				  << "Detecting Nothing" << std::endl;

		numMarker = 0;
	}
	else
	{
		size_t count = 0;
		while (1)
		{
			//std::cout << count << std::endl;
			std::stringstream tempss;
			tempss << "marker_min" << count;

			FileNode fn = fs[tempss.str().c_str()];
			if (fn.type() == FileNode::SEQ)
			{
				Scalar s = Scalar(fn[0], fn[1], fn[2], 0);
				markers_min.push_back(s);
			}
			else
				break;

			tempss.str("");
			tempss << "marker_max" << count;
			fn = fs[tempss.str().c_str()];
			if (fn.type() == FileNode::SEQ)
			{
				Scalar s = Scalar(fn[0], fn[1], fn[2], 0);
				markers_max.push_back(s);
			}
			else
				break;

			count++;
		}
		numMarker = count;

		if (rgb_threshold)
			delete[] rgb_threshold;
		rgb_threshold = new unsigned char[256*256*256]();


		size_t colors = 256*256*256;

		Mat hsv(colors, 1, CV_8UC3);
		Mat bgr(colors, 1, CV_8UC3);

		size_t cur_color = 0;
		for (long b = 0; b < 256; b++) {
			for (long g = 0; g < 256; g++) {
				for (long r = 0; r < 256; r++) {
					bgr.data[3*cur_color + 0] = b;
					bgr.data[3*cur_color + 1] = g;
					bgr.data[3*cur_color + 2] = r;
					cur_color++;
				}
			}
		}

		cvtColor(bgr, hsv, CV_BGR2HSV);

		for (size_t c = 0; c < 256*256*256; c++) {
			for (size_t m = 0; m < numMarker; m++) {
				Scalar min = markers_min[m];
				Scalar max = markers_max[m];

				bool in = (min[0] <= hsv.data[c*3 + 0] && hsv.data[c*3 + 0] <= max[0]) && 
						  (min[1] <= hsv.data[c*3 + 1] && hsv.data[c*3 + 1] <= max[1]) && 
						  (min[2] <= hsv.data[c*3 + 2] && hsv.data[c*3 + 2] <= max[2]);
				rgb_threshold[bgr.data[c*3 + 0] + 256*bgr.data[c*3 + 1] + 256*256*bgr.data[c*3 + 2]] |= (in << m);
			}
		}
	}

#ifdef MARKERDEBUG
	std::cout << std::dec;
	std::cout << "Number of markers: " << numMarker << std::endl;
	std::cout << "Markers:" << std::endl;
	for (unsigned int i = 0; i < markers_min.size(); i++)
	{
		std::cout << "Marker " << i << ": " << std::endl;
		std::cout << std::setw(3) << markers_min[i][0] << " "
		  		  << std::setw(3) << markers_min[i][1] << " "
		  		  << std::setw(3) << markers_min[i][2] << " "<< std::endl;
		std::cout << std::setw(3) << markers_max[i][0] << " "
		  		  << std::setw(3) << markers_max[i][1] << " "
		  		  << std::setw(3) << markers_max[i][2] << " "<< std::endl;
	}
#endif
	return true;
}

#ifndef BENCHMARK
int FXBcam::findBlobs(Mat img, int mnum, 
						  vector<Blob> &list, double circularity, CvRect roi)
#else
int FXBcam::findBlobs(Mat img, int mnum, 
						  vector<Blob> &list, bench_fxbcam& bench, double circularity, CvRect roi)
#endif
{
#ifdef BENCHMARK
	Timer bench_t(CLOCK_THREAD_CPUTIME_ID);
#endif


/*
#ifdef BENCHMARK
	bench_t.start();
#endif

	Mat segmented;

	//inRange(img, getMarkerMin(mnum), getMarkerMax(mnum), segmented);
	inRange_table(img, segmented, rgb_threshold, mnum);

#ifdef BENCHMARK
	bench_t.end();
	bench.thresholding += bench_t.elapsed();
#endif
*/

	Mat segmented = img;

	//char title[50] = {0};
	//sprintf(title, "Segmented %d", mnum);
	//imshow(title, segmented);

#ifdef BENCHMARK
	bench_t.start();
#endif

	dilate(segmented, segmented, Mat(), Point(-1, -1), 2);
	std::vector< std::vector< Point > > contours;
	findContours(segmented, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

#ifdef DEBUG	
	Mat draw = Mat::zeros(img.size(), img.depth());
#endif

	for (size_t i = 0; i < contours.size(); i++)
	{
		Mat temp(contours[i]);
		Moments m = moments(temp);

		if (m.m00 > 5 && m.m00 < 1000000)
		{
			if (circularity >= 0)
			{
				double perim = arcLength(temp, true);
				double ratio = 4 * CV_PI * m.m00 / (perim * perim);
				if (ratio < circularity)
					continue;
			}
			Blob b(m.m10/m.m00, m.m01/m.m00, m.m00);
			list.push_back(b);
#ifdef DEBUG
			drawContours(draw, contours, i, CV_RGB(0,0,255));
#endif
		}
		
	}
#ifdef DEBUG
	char title[100];
	sprintf(title, "%d: contours\n", mnum);
	imshow(title, draw);
#endif


#ifdef BENCHMARK
	bench_t.end();
	bench.blob_detection += bench_t.elapsed();
#endif
	return 0;
}

#ifndef BENCHMARK
int FXBcam::findMarkers(Mat img, std::vector<Marker> &markers) {
#else
int FXBcam::findMarkers(Mat img, std::vector<Marker> &markers, bench_fxbcam &bench) {
#endif

#ifdef BENCHMARK
	Timer bench_t(CLOCK_THREAD_CPUTIME_ID);
#endif


	std::vector<Blob> marker_blue, marker_red, marker_green;


#ifdef BENCHMARK
	bench_t.start();
#endif

	std::vector<Mat> segmented(2);
	size_t marker_nums[2];
	marker_nums[0] = 0;
	marker_nums[1] = 1;

	inRange_table_v(img, segmented, marker_nums, 2, rgb_threshold);

#ifdef BENCHMARK
	bench_t.end();
	bench.thresholding = bench_t.elapsed();
#endif
	

#ifndef BENCHMARK
	findBlobs(segmented[0], 0, marker_blue);
	findBlobs(segmented[1], 1, marker_red);
#else
	findBlobs(segmented[0], 0, marker_blue, bench);
	findBlobs(segmented[1], 1, marker_red, bench);
#endif


#ifdef BENCHMARK
	bench_t.start();
#endif

	/* Sort blobs by decresing area */
	std::sort(marker_blue.begin(), marker_blue.end());
	//std::sort(marker_green.begin(), marker_green.end());
	std::sort(marker_red.begin(), marker_red.end());
	

#ifdef DEBUG
    printf("red: ");	   
	for (size_t i = 0; i < marker_red.size();i++)
		printf("%10.6lf ", marker_red[i].area);
	printf("\n");
    printf("blue: ");	   
	for (size_t i = 0; i < marker_blue.size();i++)
		printf("%10.6lf ", marker_blue[i].area);
	printf("\n");

	for (size_t i = 0; i < marker_blue.size(); i++)
	{
		double base = marker_blue[i].area;
		size_t j = i;
		for (j = i; j < marker_blue.size(); j++)
		{
			if (1 - marker_blue[j].area/base > .3 || 1 - base/marker_blue[j].area > .3)
				break;
		}
		printf("range: %zu\n", j-i);
	}
#endif


	/* Choose three blue, one red marker */
	markers.clear();
	size_t n = 0;
	for (std::vector<Blob>::iterator m = marker_blue.begin(); m != marker_blue.end(); ++m) {
		if (n < 3) {
			markers.push_back(Marker(m->x, m->y, 0));
#ifdef DEBUG
			circle(img, Point(m->x, m->y), 4, Scalar(255, 0, 0));
#endif
		} else
			break;
		n++;
	}

	for (std::vector<Blob>::iterator m = marker_red.begin(); m != marker_red.end(); ++m) {
		markers.push_back(Marker(m->x, m->y, 2));
#ifdef DEBUG
		circle(img, Point(m->x, m->y), 4, Scalar(0, 0, 255));
#endif
		break;
	}

	/*
	for (std::vector<Blob>::iterator m = marker_green.begin(); m != marker_green.end(); ++m) {
		markers.push_back(Marker(m->x, m->y, 1));
#ifdef DEBUG
		circle(img, Point(m->x, m->y), 4, Scalar(0, 255, 0));
#endif
		break;
	}
	*/


	/* Need at least 3 markers visible */
	if (markers.size() >= 3) {
		/* Find the centroid */
		Point2d centroid = Point2d(0.0, 0.0);

		for (size_t i = 0; i < markers.size(); i++) {
			centroid.x += markers[i].x;
			centroid.y += markers[i].y;
		}
		centroid.x /= markers.size();
		centroid.y /= markers.size();

#ifdef DEBUG
		circle(img, centroid, 3, Scalar(255, 255, 255), -1);
#endif


		/* Convert marker positions to polar coordinates */
		for (size_t i = 0; i < markers.size(); i++) {
			Scalar color;
			if (markers[i].color == 0) // Blue
				color = Scalar(255, 0, 0);
			else if (markers[i].color == 1) // Green
				color = Scalar(0, 255, 0);
			else if (markers[i].color == 2) // Red
				color = Scalar(0, 0, 255);

			markers[i].r = sqrt(pow(markers[i].x - centroid.x, 2) + pow(markers[i].y - centroid.y, 2));

			double angle = atan2(markers[i].y - centroid.y, markers[i].x - centroid.x);
			if (angle < 0)
				angle += 2*M_PI;
			markers[i].theta = angle;

#ifdef DEBUG
			ellipse(img,
					centroid,
					Size2f(markers[i].r, markers[i].r),
					0.0,
					0.0,
					180.0/M_PI * markers[i].theta,
					color);
#endif
		}

		/* Sort markers by increasing theta */
		std::sort(markers.begin(), markers.end());

		/* Look for patterns going clockwise through markers */
		LED led_pattern[8] = {{0,2}, {1,0}, {2,0}, {3,0},  {0,2}, {1,0}, {2,0}, {3,0}};
		size_t pattern_idx;
		size_t markers_idx;

		/* Start from the marker after the largest angle gap */
		double largest_gap = 0; // Biggest possible gap is 2*pi
		for (size_t i = 0; i < markers.size(); i++) {
			size_t n = (i+1) % markers.size();
			double diff;
			if (n > i)
				diff = markers[n].theta - markers[i].theta;
			else
				diff = markers[n].theta - markers[i].theta + 2*M_PI;

			if (diff > largest_gap) {
				markers_idx = n;
				largest_gap = diff;
			}
		}

		/* Put the starting marker at position 0 */
		std::rotate(markers.begin(), markers.begin() + markers_idx, markers.end());

		/* Match visible marker pattern to the reference */
		for (pattern_idx = 0; pattern_idx < 4; pattern_idx++) {
			size_t matched = 0;
			for (size_t m = 0; m < markers.size(); m++) {
				if (markers[m].color != led_pattern[pattern_idx + m].color)
					break;
				markers[m].id = led_pattern[pattern_idx + m].id;
				matched++;
			}
			if (matched == markers.size())
				break;
		}


		/* Reorder markers by theta */
		//std::sort(markers.begin(), markers.end());

		/*
		for (size_t i = 0; i < markers.size(); i++)
			std::cout << markers[i].id << " ";
		std::cout << std::endl;
		*/

#ifdef BENCHMARK
	bench_t.end();
	bench.marker_ident = bench_t.elapsed();
#endif

		return markers.size();
	} else {
		// Identify what markers we can
	}

#ifdef BENCHMARK
	bench_t.end();
	bench.marker_ident = bench_t.elapsed();
#endif
	return 0;
}


void FXBcam::castRays(std::vector<Marker> &markers, std::vector<Ray> &rays) {
	Mat pos   = Mat(3, 1, CV_64FC1);
	Mat ray   = Mat(3, 1, CV_64FC1);
	Mat point = Mat(3, 1, CV_64FC1);

	Mat n = (Mat_<double>(3,1) << 0,0,1);

	point.at<double>(2,0) = 1.0;

	rays.clear();
	for (std::vector<Marker>::iterator m = markers.begin(); m != markers.end(); ++m) {
		point.at<double>(0,0) = m->x;
		point.at<double>(1,0) = m->y;

		ray = Rc_ext_inv * intrinsic_inv * point;
		ray = ray / norm(ray);

		// Find intersection of the ray and the X-Y plane
		// ray = cam_pos + ray * (0.0 - cam_pos).dot(n) / ray.dot(n);

		rays.push_back(Ray(0, ray.at<double>(0,0), ray.at<double>(1,0), ray.at<double>(2,0), m->id));
	}
}

Vec3 FXBcam::intersectRays(std::vector<Ray> &rays) {
	// A*x = b

	Mat A = Mat::zeros(3, 3, CV_64FC1);
	Mat b = Mat::zeros(3, 1, CV_64FC1);
	for (std::vector<Ray>::iterator r = rays.begin(); r != rays.end(); ++r) {
		A.at<double>(0,0) +=  1.0 - r->a*r->a;
		A.at<double>(0,1) += -1.0 * r->a*r->b;
		A.at<double>(0,2) += -1.0 * r->a*r->c;

		A.at<double>(1,0) += -1.0 * r->a*r->b;
		A.at<double>(1,1) +=  1.0 - r->b*r->b;
		A.at<double>(1,2) += -1.0 * r->b*r->c;

		A.at<double>(2,0) += -1.0 * r->a*r->c;
		A.at<double>(2,1) += -1.0 * r->b*r->c;
		A.at<double>(2,2) +=  1.0 - r->c*r->c;


		b.at<double>(0,0) += (1.0 - r->a*r->a)*r->x - r->a*r->b*r->y - r->a*r->c*r->z;
		b.at<double>(1,0) += (1.0 - r->b*r->b)*r->y - r->a*r->b*r->x - r->b*r->c*r->z;
		b.at<double>(2,0) += (1.0 - r->c*r->c)*r->z - r->a*r->c*r->x - r->b*r->c*r->y;
	}

	Mat x = A.inv(DECOMP_SVD) * b;


#ifdef INTER_DEBUG
	double xx = x.at<double>(0,0);
	double yy = x.at<double>(1,0);
	double zz = x.at<double>(2,0);

	double error = 0, avg_error = 0;
	printf("  Error %lu: ", rays.begin()->marker);
	for (std::vector<Ray>::iterator r = rays.begin(); r != rays.end(); ++r) {
		error = sqrt((xx-r->x)*(xx-r->x) + (yy-r->y)*(yy-r->y) + (zz-r->z)*(zz-r->z) - pow(r->a*(xx-r->x)+r->b*(yy-r->y)+r->c*(zz-r->z),2));
		avg_error += error;
		printf("%lu|%12.6f ", r->client_id, error);
	}
	printf("  avg|%12.6f\n",  avg_error/rays.size());
#endif


	return Vec3(x.at<double>(0,0), x.at<double>(1,0), x.at<double>(2,0));
}


Scalar FXBcam::getMarkerMin(size_t num)
{
	if (num >= numMarker)
		return Scalar(-1, -1, -1, 0);
	else
		return markers_min[num];
}

Scalar FXBcam::getMarkerMax(size_t num)
{
	if (num >= numMarker)
		return Scalar(-1, -1, -1, 0);
	else
		return markers_max[num];
}
