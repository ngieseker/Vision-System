#include "common.h"
#include "camera.h"
#include "Timer.hpp"
#include <cstdio>
#include <vector>

#include "jpeglib.h"

using namespace cam1394;


struct jpeg_frame {
	unsigned char *data;
	size_t         size;

	jpeg_frame() : data(NULL), size(0) {}
	~jpeg_frame() { free(data); }

	void reset() { free(data); size = 0; }
};


int main(int argc, char *argv[]) {
	camera cam;
	if (cam.open("NONE", "1024x768_MONO8", 30, "HQLINEAR", "GRBG") < 0)
		return 1;

	cam.setTrigger(1);
	cam.setBrightness(0);
	cam.setGain(0);
	cam.setExposure(0);
	cam.setShutter(600);
	cam.setWhiteBalance(75, 32);


	size_t vidw = 1024, vidh = 768, channels = 3;
	size_t stride = vidw * channels;

	cv::Mat cam_image;
	cv::Mat down_img(cv::Size(vidw/4, vidh/4), CV_8UC3);
	
	
	jpeg_frame jpeg_out;
	std::vector<uchar> jpeg;

	std::vector<int> jpeg_params;
	jpeg_params.push_back(CV_IMWRITE_JPEG_QUALITY);
	jpeg_params.push_back(70);


	Timer total, encode;
	double encode_time = 0;

	int frames = 0;
	size_t total_encoded = 0;
	total.start();
	do {
		cam_image = cam.read();

		encode.start();

		/** Downsampling **/
#if 0
		resize(cam_image, down_img, cv::Size(vidw/4, vidh/4), 0, 0, cv::INTER_AREA);
		total_encoded += down_img.total();
#endif
		

		/** libjpeg-turbo **/
#if 1
		jpeg_out.reset();
		
		jpeg_compress_struct jpeg_cinfo;
		jpeg_error_mgr jpeg_err;
		jpeg_cinfo.err = jpeg_std_error(&jpeg_err);
		jpeg_create_compress(&jpeg_cinfo);
		
		jpeg_mem_dest(&jpeg_cinfo, &jpeg_out.data, &jpeg_out.size);
		jpeg_cinfo.image_width      = vidw;
		jpeg_cinfo.image_height     = vidh;
		jpeg_cinfo.input_components = channels;
		jpeg_cinfo.in_color_space   = JCS_EXT_BGR;

		jpeg_set_defaults(&jpeg_cinfo);
		jpeg_set_quality(&jpeg_cinfo, 70, TRUE);

		jpeg_start_compress(&jpeg_cinfo, TRUE);

		JSAMPROW row_ptr[1];
		while (jpeg_cinfo.next_scanline < jpeg_cinfo.image_height) {
			row_ptr[0] = (JSAMPLE *)(cam_image.data + jpeg_cinfo.next_scanline*stride);
			jpeg_write_scanlines(&jpeg_cinfo, row_ptr, 1);
		}

		jpeg_finish_compress(&jpeg_cinfo);
		jpeg_destroy_compress(&jpeg_cinfo);


		total_encoded += jpeg_out.size;

		char filename[100];
		sprintf(filename, "test%03d.jpg", frames);
		FILE *out = fopen(filename, "w+");
		fwrite(jpeg_out.data, 1, jpeg_out.size, out);
		fclose(out);
#endif


		/** OpenCV jpeg **/
#if 0
		jpeg.clear();
		cv::imencode(".jpeg", cam_image, jpeg, jpeg_params);
		total_encoded += jpeg.size();
#endif
		
		encode.end();
		encode_time += encode.elapsed();

		frames++;
		
	} while (total.timePassed() < 5);
	total.end();

	printf("Encoded %d frames in %7fs\n", frames, encode_time);
	printf("Average %7f ms/frame, %5fHz, ratio %5f\n", 1000*encode_time / frames, frames / encode_time, (double)total_encoded / (vidw*vidh*channels*frames));

	//printf("Final frame: %lu bytes, ratio %5f\n", jpeg_out.size, (double)jpeg_out.size / (vidw*vidh*channels));
	FILE *out = fopen("test.jpeg", "w+");
	fwrite(jpeg_out.data, 1, jpeg_out.size, out);
	//fwrite(&jpeg[0], 1, jpeg.size(), out);
	fclose(out);

	
	return 0;
}
