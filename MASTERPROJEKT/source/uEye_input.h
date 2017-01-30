#pragma once
#include <opencv2/opencv.hpp>
#include <uEye.h>
#include <uEye_tools.h>
#include <ueye_deprecated.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <Marker.h>

class uEye_input {

private:
	IplImage * img;
	HIDS  hCam = 1;
	int img_width;
	int img_height;
	cv::Mat frame;
	
public:
	int inituEyeCam();
	cv::Mat getCapturedFrame();
	int exitCamera();

	~uEye_input();
	uEye_input();

};