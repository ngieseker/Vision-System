#include <opencv2/opencv.hpp>
#include "camera.h"
#include <cvsba/cvsba.h>

extern "C" {
    #include "apriltag.h"
    #include "image_u8.h"
    #include "tag36h11.h"
    #include "tag36h10.h"
    #include "zarray.h"
    #include "homography.h"
}

using namespace cam1394;
using namespace cv;
using namespace cvsba;


#define USE_CHESSBOARD
const Size chessboardSize(5, 8);

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

void generateChessboardPositions(Size boardSize, float squareSize, vector<Point3f>& corners)
{
    corners.clear();
    for(int i = 0; i < boardSize.height; ++i)
        for(int j = 0; j < boardSize.width; ++j)
            corners.push_back(Point3f(j*squareSize, i*squareSize, 0));
}

void convert_cv_mat_to_image_u8_t(Mat in, image_u8_t **out) {
    image_u8_t *ret = new image_u8_t;

    ret->width = in.cols;
    ret->height = in.rows;
    ret->stride = ret->width;

    ret->buf = in.data;

    *out = ret;
}

void castRays(std::vector<Point2d> &points, Mat R, Mat t, Mat intrinsic, std::vector<Ray> &rays) {
	Mat pos   = Mat(3, 1, CV_64FC1);
	Mat ray   = Mat(3, 1, CV_64FC1);
	Mat point = Mat(3, 1, CV_64FC1);

	Mat n = (Mat_<double>(3,1) << 0,0,1);

	Mat T = -1.0 * R.inv() * t;

	point.at<double>(2,0) = 1.0;

	rays.clear();
	for (auto m = points.begin(); m != points.end(); ++m) {
		point.at<double>(0,0) = m->x;
		point.at<double>(1,0) = m->y;

		ray = R.inv() * intrinsic.inv() * point;
		ray = ray / norm(ray);

		// Find intersection of the ray and the X-Y plane
		// ray = cam_pos + ray * (0.0 - cam_pos).dot(n) / ray.dot(n);

		Ray temp = Ray(0, ray.at<double>(0,0), ray.at<double>(1,0), ray.at<double>(2,0), 0);
        temp.setOrigin(T);
		rays.push_back(temp);
	}
}

Ray castRay(Point2d p, Mat R, Mat t, Mat intrinsic) {
	Mat pos   = Mat(3, 1, CV_64FC1);
	Mat ray   = Mat(3, 1, CV_64FC1);
	Mat point = Mat(3, 1, CV_64FC1);

	Mat n = (Mat_<double>(3,1) << 0,0,1);

	Mat T = -1.0 * R.inv() * t;

	point.at<double>(2,0) = 1.0;

    point.at<double>(0,0) = p.x;
    point.at<double>(1,0) = p.y;

    ray = R.inv() * intrinsic.inv() * point;
    ray = ray / norm(ray);

    // Find intersection of the ray and the X-Y plane
    // ray = cam_pos + ray * (0.0 - cam_pos).dot(n) / ray.dot(n);

    Ray temp = Ray(0, ray.at<double>(0,0), ray.at<double>(1,0), ray.at<double>(2,0), 0);
    temp.setOrigin(T);
    return temp;
}

Point3d intersectRays(std::vector<Ray> &rays) {
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


	return Point3d(x.at<double>(0,0), x.at<double>(1,0), x.at<double>(2,0));
}


int main(int argc, char *argv[]) {
    if (argc < 5 || (argc-1) % 2 != 0) {
        fprintf(stderr, "%s: <im1 cal1> <im2 cal2> ...\n", argv[0]);
        return 1;
    }

    april_tag_family_t *tf = tag36h11_create();
    april_tag_detector_t *td = april_tag_detector_create(tf);

    std::vector<Mat> intrinsic;
    std::vector<Mat> distCoeff;
    std::vector<Mat> images;

    std::vector<Mat> pose;
    
    std::vector<std::vector<std::vector<Point2d>>> tagPoints;

    for (int i = 1; i < argc; i+=2) {
        Mat frame = imread(argv[i]);
        if (frame.empty()) {
            fprintf(stderr, "Failed to read '%s'\n", argv[1+i]);
            return 1;
        }

        FileStorage fs;
        fs.open(argv[i+1], FileStorage::READ);
        if (!fs.isOpened()) {
            fprintf(stderr, "Failed to open '%s'\n", argv[3+i]);
            return 1;
        }


        Mat intr, dist, im, p;
        std::vector<std::vector<Point2d>> tp;

        fs["camera_matrix"] >> intr;
        fs["distortion_coefficients"] >> dist;
        undistort(frame, im, intr, dist);

        Mat gray;
        cvtColor(im, gray, CV_BGR2GRAY);

        image_u8_t *u8image;
        convert_cv_mat_to_image_u8_t(gray, &u8image);
        zarray_t *detections = april_tag_detector_detect(td, u8image);

        int cur_id = 0;
        for (int d = 0; d < zarray_size(detections); d++) {
            april_tag_detection_t *det;
            zarray_get(detections, d, &det);

            if (det->goodness > 50 && det->hamming < 3) {
                //printf("id: %4d goodness: %10.5f hamming: %2d\n", det->id, det->goodness, det->hamming);

                std::vector<Point2d> det_points;
                for (int j = 0; j < 4; j++) {
                    det_points.push_back(Point2d(det->p[j][0], det->p[j][1]));

                    Scalar color;
                    if (j == 0)
                        color = Scalar(0, 255, 0);
                    else
                        color = Scalar(255, 0, 0);

                    circle(im, Point(det->p[j][0], det->p[j][1]), 2, color, -1, CV_AA);
                }
                det_points.push_back(Point2d(det->c[0], det->c[1]));
                
                for (int id = cur_id; id < det->id; id++) {
                    tp.push_back(std::vector<Point2d>());
                }

                tp.push_back(det_points);
                cur_id = det->id+1;
                printf("cur_id = %d\n", cur_id);

#ifndef USE_CHESSBOARD
                if (det->id == 0) {
                    //matd_t *matd_pose = homography_to_pose(det->H, -intr.at<double>(0,0), -intr.at<double>(1,1), 0.0, 0.0);
                    matd_t *matd_pose = homography_to_pose(det->H, intr.at<double>(0,0), intr.at<double>(1,1), intr.at<double>(0, 2), intr.at<double>(1, 2));
                    p = Mat::zeros(4, 4, CV_64F);
                    memcpy(p.data, matd_pose->data, 16 * sizeof(double));

                    Mat R = p(Range(0, 3), Range(0, 3));
                    SVD svd(R);
                    R = svd.u * svd.vt;

                    Mat t = p(Range(0, 3), Range(3, 4));
                    t = 1.0 * t * 0.1725/2;

                    R.copyTo(p(Range(0, 3), Range(0, 3)));
                    t.copyTo(p(Range(0, 3), Range(3, 4)));

                    matd_destroy(matd_pose);
                }
#endif
            }

            april_tag_detection_destroy(det);
        }

#ifdef USE_CHESSBOARD
        std::vector<Point3f> board_corners;
        std::vector<Point2f> point_buf;
        generateChessboardPositions(chessboardSize, 0.1, board_corners);

        bool chessboard_found = findChessboardCorners(im, chessboardSize, point_buf,
                                                      CV_CALIB_CB_ADAPTIVE_THRESH +
                                                      //CV_CALIB_CB_FAST_CHECK +
                                                      CV_CALIB_CB_NORMALIZE_IMAGE +
                                                      CV_CALIB_CB_FILTER_QUADS);

        if (!chessboard_found) {
            fprintf(stderr, "Error: no chessboard in image %d\n", i/2);
            return 1;
        }

        Mat frame_gray;
        cvtColor(frame, frame_gray, CV_BGR2GRAY);

        cornerSubPix(gray, point_buf, Size(5, 5), Size(-1, -1), 
                     TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 40, 0.001));
        drawChessboardCorners(im, chessboardSize, Mat(point_buf), chessboard_found);

        Mat Rvec, T, R;
        solvePnP(board_corners, point_buf, intr, dist, Rvec, T);
        Rodrigues(Rvec, R);
        //R = R.t();
        //T = 1.0 * R * T;

        p = Mat::zeros(4, 4, CV_64F);
        R.copyTo(p(Range(0, 3), Range(0, 3)));
        T.copyTo(p(Range(0, 3), Range(3, 4)));
#endif

        zarray_destroy(detections);
        delete u8image;

        intrinsic.push_back(intr);
        distCoeff.push_back(dist);
        images.push_back(im);
        pose.push_back(p);
        tagPoints.push_back(tp);
    }

#ifndef USE_CHESSBOARD
    for (auto p: pose) {
        if (p.empty()) {
            fprintf(stderr, "Error: tag 0 not visible in all views\n");
            return 1;
        }
    }
#endif

    // triangulate points here derp
    int max_tags = 0;
    for (auto tp: tagPoints) {
        if (tp.size() > max_tags)
            max_tags = tp.size();
    }

    std::vector<std::vector<Point3d>> world_points_by_id(max_tags);
    for (int i = 0; i < max_tags; i++) {
        std::vector<Ray> rays_by_point[5];
        for (int j = 0; j < tagPoints.size(); j++) {
            std::vector<Point2d> points;
            if (i >= tagPoints[j].size()) {
                continue;
            } else {
                for (int k = 0; k < tagPoints[j][i].size(); k++) {
                    Ray ray = castRay(tagPoints[j][i][k], pose[j](Range(0, 3), Range(0, 3)), pose[j](Range(0, 3), Range(3, 4)), intrinsic[j]);
                    rays_by_point[k].push_back(ray);
                }
            }
        }

        for (int j = 0; j < 5; j++) {
            if (rays_by_point[j].size() > 1) {
                Point3d p3d = intersectRays(rays_by_point[j]);
                world_points_by_id[i].push_back(p3d);
            }
        }
    }


    //for (int i = 0 ; i < world_points_by_id.size(); i++) {
    //    for (int j = 0; j < world_points_by_id[i].size(); j++) {
    //        std::cout << world_points_by_id[i][j] << std::endl;
    //    }
    //}
    
    // Flattening structure for bundle adjustment
    std::vector<Point3d> points3D;
    std::vector<std::vector<Point2d>> imagePoints(tagPoints.size());
    std::vector<std::vector<int>> visibility(tagPoints.size());
    
    // loop through tags
    for (size_t tag_id = 0; tag_id < world_points_by_id.size(); tag_id++) {

        // check if tag exists in 3d
        if (world_points_by_id[tag_id].size() > 0) {

            // loop through 5 points of the tag
            for (int tp_id = 0; tp_id < 5; tp_id++) {
                // append world 3d point
                points3D.push_back(world_points_by_id[tag_id][tp_id]);
                for (size_t cam_id = 0; cam_id < tagPoints.size(); cam_id++) {
                    // check if camera can see this tag
                    if (tagPoints[cam_id][tag_id].size() > 0 && tag_id < tagPoints[cam_id].size()) {
                        // append 2d point
                        imagePoints[cam_id].push_back(tagPoints[cam_id][tag_id][tp_id]);
                        // add visibility
                        visibility[cam_id].push_back(1);
                    } else {
                        imagePoints[cam_id].push_back(Point2d(0, 0));
                        visibility[cam_id].push_back(0);
                    }
                }
            }
        }
    }

    assert(imagePoints[0].size() == points3D.size());
    assert(visibility[0].size() == points3D.size());

    // flatten R and T from pose
    std::vector<Mat> R;
    std::vector<Mat> T;
    for (auto p : pose) {
        R.push_back(p(Range(0, 3), Range(0, 3)));
        T.push_back(p(Range(0, 3), Range(3, 4)));
    }
 
    // ADJUST THE BUNDLES
    Sba sba;
    Sba::Params params = sba.getParams();



    printf("\n\n===== Before Adjustment =====\n");
    std::cout << "a = [";
    for (auto &p: points3D) {
        std::cout << p.x << " " << p.y << " " << p.z << std::endl;
    }
    std::cout << "];" << std::endl;

    for (int c = 0; c < R.size(); c++) {
        //T[c] = 1.0 * R[c].inv() * T[c];
        std::cout << "K" << c << " = " << intrinsic[c] << ";\n" << "R" << c << " = " << R[c] << ";\nT" << c << " = " << T[c] << ";" << std::endl;
    }

    //params.verbose = true;
    params.iterations = 10000;
    params.fixedIntrinsics = 5;
    params.fixedDistortion = 5;
    sba.setParams(params);
    sba.run(points3D, imagePoints, visibility, intrinsic, R, T, distCoeff);

    printf("\n\n===== After Adjustment =====\n");
    std::cout << "Reprojection Error " << sba.getInitialReprjError() << " -> " << sba.getFinalReprjError() << std::endl;
    std::cout << "b = [";
    for (auto p : points3D) {
        std::cout << p.x << " " << p.y << " " << p.z << std::endl;
    }
    std::cout << "];" << std::endl;

    for (int c = 0; c < R.size(); c++) {
        std::cout << "K" << c << " = " << intrinsic[c] << ";\n" << "R" << c << " = " << R[c] << ";\nT" << c << " = " << T[c] << ";" << std::endl;
    }


#if 0
    for (auto t : T) {
        std::cout << t << std::endl;
    }

    params.iterations = 10000;
    sba.setParams(params);
    sba.run(points3D, imagePoints, visibility, intrinsic, R, T, distCoeff);
    std::cout << sba.getInitialReprjError() << ", " << sba.getFinalReprjError() << std::endl;
#endif

    Mat all_im;
    for (auto im: images) {
        if (all_im.empty())
            im.copyTo(all_im);
        else
            hconcat(all_im, im, all_im);
    }
    imshow("detections", all_im);

    int key;
    while ((key & 0xFF) != 'q') {
        key = waitKey(10);
    }

    return 0;
}
